#ifndef _FS_H_
#define _FS_H_
#include "fs/tmpfs.h"
#include "fs/uartfs.h"
#include "fs/framebuffer.h"

extern int sys_open(const char *, int);
extern int sys_close(int);
extern long sys_write(int, char *, unsigned long);
extern long sys_read(int, char *, unsigned long);
extern int sys_mkdir(const char *, unsigned);
extern int sys_mount(const char *, const char *, const char *, unsigned long, const void *);
extern int sys_chdir(const char *);
extern long sys_lseek64(int, long, int);
extern long sys_ioctl(int, long, ...);
extern void fs_init();


#endif
