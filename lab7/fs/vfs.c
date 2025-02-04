#include "mm/mm.h"
#include "fs/tmpfs.h"
#include "fs/vfs.h"

struct mount* rootfs;
LIST_HEAD(mount_list);
LIST_HEAD(filesystem_type_list);

struct file* get_file_by_fd(struct files_struct* files, int fd){
    struct file* ret;
    uint64_t daif = local_irq_disable_save(); 
    ret = files->fd_array[fd];
    local_irq_restore(daif);
    return ret;
}

int get_unused_fd(struct files_struct* files){
    int fd = -1;
    uint64_t daif = local_irq_disable_save();
    for(int i = 0 ; i < NR_OPEN_DEFAULT ; i++){
        if(files->fd_array[i] == NULL){
            fd = i;
            files->fd_array[i] = (struct file*)1;
            break;
        }
    }
    local_irq_restore(daif);
    return fd;
}

int put_unused_fd(struct files_struct* files, int fd){
    uint64_t daif = local_irq_disable_save();
    files->fd_array[fd] = NULL;
    local_irq_restore(daif);
}

int fd_install(struct files_struct* files, int fd, struct file *f){
    uint64_t daif = local_irq_disable_save();
    files->fd_array[fd] = f;
    local_irq_restore(daif);
}

int vfs_lseek64(struct file * file, loff_t offset, int whence){
    return file->f_ops->lseek64(file, offset, whence);
}

void vfs_init(struct filesystem_type* fs_type){
    struct inode* inode_root;
    struct dentry* root, *link;
    struct tmpfs_dir* root_dir;
    struct mount* root_mnt;

    // create root filesystem
    root = create_tmpfs_file("/", NULL, S_IFDIR); 
    root->d_parent = root;

    root_dir = root->d_inode->private_data;
    root_dir->count += 2;
    link = root;
    root_dir->entries[0] = create_tmpfs_file(".", link, S_IFLNK);
    root_dir->entries[1] = create_tmpfs_file("..", link, S_IFLNK);


    root_mnt = fs_type->mount(fs_type, root); 
    root->d_flags |= DENTRY_FLAG_MOUNTED;  

    // add root mount to mount list
    list_add_tail(&root_mnt->list, &mount_list);

    // set rootfs
    rootfs = root_mnt;
}

struct inode* create_inode(struct file_operations *f_ops, struct inode_operations *i_ops, umode_t i_modes){
    struct inode* ret = (struct inode*)kmalloc(sizeof(struct inode));
    ret->i_modes = i_modes;
    ret->i_ops = i_ops;
    ret->f_ops = f_ops;
    ret->private_data = NULL;
    return ret;
}

struct dentry* create_dentry(char *d_name, int d_flags, struct inode* d_inode){
    struct dentry* ret = (struct dentry*)kmalloc(sizeof(struct dentry));
    ret->d_name = d_name;
    ret->d_flags = d_flags;

    ret->d_inode = d_inode;
    return ret;
}

struct file* create_file(struct dentry* d, unsigned int flags, unsigned mode){
    struct file* ret = (struct file*)kmalloc(sizeof(struct file));
    ret->f_dentry = d;
    ret->f_count = 1;
    ret->f_flags = flags;
    ret->f_pos = 0;
    ret->f_ops = d->d_inode->f_ops;
    ret->f_mode = mode;

    return ret;
}

struct file* vfs_open(const char* pathname, int flags, umode_t mode){
    const char* delim = "/";
    char* name, *prev_tok;
    struct task_struct* current = get_current();
    struct dentry* prev = NULL, *next, *pwd;
    char* tok, *new_file_name;
    FS_LOG("vfs_open(%s, %p, %p)", pathname, flags, mode); 
    if(pathname[0] == '\0') goto error; 
    
    if(pathname[0] == '/'){
        next = rootfs->mnt_root; 
        pathname++;
    }else{
        current = get_current();
        pwd = current->fs->pwd;
        next = pwd;
    }
    
    name = (char*)kmalloc(strlen(pathname));
    strcpy(name, pathname);
    FS_LOG("target pathname: %s", name);
    FS_LOG("lookup start from: %s", next->d_name);

    tok = strtok(name, delim);
    while (tok != NULL) {
        FS_LOG("%s", tok);
        if(next == NULL || !S_ISDIR(next->d_inode->i_modes)) goto free;
        prev_tok = tok;
        prev = next;
        next = next->d_inode->i_ops->lookup(next, tok);  
        FS_LOG("next: %p, next->d_name: %s", next, next == NULL ? "[NULL]" : next->d_name);
        tok = strtok(NULL, delim);
    }
    
    if(next == NULL && (flags | O_CREAT)){
        next = prev->d_inode->i_ops->create(prev, prev_tok);
    }

    FS_LOG("open file: %p, i_modes: %p", next, next->d_inode->i_modes);
    if(next != NULL && (S_ISREG(next->d_inode->i_modes) || S_ISCHR(next->d_inode->i_modes))) goto found;

free:
    kfree(name);
error:
    FS_LOG("not found");
    return NULL;
found:
    FS_LOG("found");
    kfree(name);
    return prev->d_inode->f_ops->open(next, flags, mode);
}

int vfs_close(struct file* file){
    struct inode* inode = file->f_dentry->d_inode;
    int ret = -1;
    
    do{
        if(file == NULL) break;

        ret = file->f_ops->flush(file);
        if(ret == -1) break;

        ret = file->f_ops->release(inode, file);
        if(ret == -1) break;
        
        

        ret = 0;
    }while(0);
    return ret;
}

long vfs_write(struct file* file, char* buf, ssize_t len){
    int ret;
    ret = file->f_ops->write(file, buf, len, &file->f_pos);
    return ret;
}

long vfs_read(struct file* file, char* buf, ssize_t len){
    int ret;
    ret = file->f_ops->read(file, buf, len, &file->f_pos);
    return ret;
}

struct dentry* vfs_lookup(const char* pathname){
    const char* delim = "/";
    char* name, *prev_tok;
    struct task_struct* current = get_current();
    struct dentry* prev = NULL, *next, *pwd;
    char* tok, *new_file_name;
    uint64_t daif;
    FS_LOG("vfs_lookup(%s)", pathname); 
    if(pathname[0] == '\0') goto error; 
    
    if(pathname[0] == '/'){
        next = rootfs->mnt_root; 
        pathname++;
    }else{
        current = get_current();
        pwd = current->fs->pwd;
        next = pwd;
    }
    
    name = (char*)kmalloc(strlen(pathname));
    strcpy(name, pathname);
    FS_LOG("pathname: %s", name);
    FS_LOG("next->d_name: %s", next->d_name);

    tok = strtok(name, delim);
    
    daif = local_irq_disable_save();
    while (tok != NULL) {
        FS_LOG("%s", tok);
        if(next == NULL || !S_ISDIR(next->d_inode->i_modes)) goto free;
        prev_tok = tok;
        prev = next;
        next = next->d_inode->i_ops->lookup(next, tok);  
        FS_LOG("next: %p, next->d_name: %s", next, next == NULL ? "[NULL]" : next->d_name);
        tok = strtok(NULL, delim);
    }
    local_irq_restore(daif); 
    if(next != NULL) goto found;

free:
    kfree(name);
error:
    FS_LOG("not found");
    return NULL;
found:
    FS_LOG("found");
    kfree(name);
    return next;
}

int vfs_mount(struct dentry* target, const char* filesystem){
    FS_LOG("vfs_mount(%s, %s)", target->d_name, filesystem);
    struct list_head* node;  
    struct filesystem_type* fs_type; 
    struct mount* mount;
    int ret = -1;
    uint64_t daif = local_irq_disable_save();
    if(target->d_flags & DENTRY_FLAG_MOUNTED) goto end;

    list_for_each(node, &filesystem_type_list){
        fs_type = list_entry(node, struct filesystem_type, list);
        if(strcmp(fs_type->fs_name,(char*) filesystem) == 0) break;
    }
     
    mount = fs_type->mount(fs_type, target);
    if(mount){
        list_add_tail(&mount->list, &mount_list);
        target->d_flags |= DENTRY_FLAG_MOUNTED;
        ret = 0;
    }
end:
    if(!ret)FS_LOG("vfs_mount success!");
    else FS_LOG("vfs_mount failed!");
    local_irq_restore(daif);
    return ret;
}

int register_filesystem(struct filesystem_type* fs){
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    list_add_tail(&fs->list, &filesystem_type_list);
}

int vfs_mkdir(const char* pathname, umode_t mode){
    const char* delim = "/";
    char* name, *prev_tok;
    struct task_struct* current = get_current();
    struct dentry* prev = NULL, *next, *pwd;
    char* tok, *new_file_name;
    FS_LOG("vfs_mkdir(%s, %p)", pathname, mode); 
    if(pathname[0] == '\0') goto error; 
    
    if(pathname[0] == '/'){
        next = rootfs->mnt_root; 
        pathname++;
    }else{
        current = get_current();
        pwd = current->fs->pwd;
        next = pwd;
    }
    
    name = (char*)kmalloc(strlen(pathname));
    strcpy(name, pathname);
    FS_LOG("pathname: %s", name);
    FS_LOG("next->d_name: %s", next->d_name);

    tok = strtok(name, delim);
    while (tok != NULL) {
        FS_LOG("%s", tok);
        if(next == NULL || !S_ISDIR(next->d_inode->i_modes)) goto free;
        prev_tok = tok;
        prev = next;
        next = next->d_inode->i_ops->lookup(next, tok);  
        FS_LOG("next: %p, next->d_name: %s", next, (next == NULL) ? "[NULL]" : next->d_name);
        tok = strtok(NULL, delim);
    }
    
    if(next == NULL){
        if(!prev->d_inode->i_ops->mkdir(prev, prev_tok, mode|S_IFDIR)) goto success;
    }

free:
    kfree(name);
error:
    return -1;
success:
    kfree(name);
    return 0;
}

int vfs_ioctl(struct file *file, uint32_t request, va_list args){
    return file->f_ops->ioctl(file, request, args);
}
