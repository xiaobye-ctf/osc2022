#ifndef _TASK_H_
#define _TASK_H_

#include "kernel/sched/sched.h"
#include "fs/initrdfs.h"
#include "mm/page_alloc.h"
#include "debug/debug.h"
#include "mm/slab.h"

extern void task_init();
extern uint64_t task_dup();
extern void task_exit();
extern void task_destroy(struct task_struct*);
extern void run_init_task(char*);
extern struct trap_frame* get_current_trap_frame();
extern void sys_kill(uint64_t);
extern struct vm_area_struct* create_vma_stack(struct mm_struct*);
extern struct vm_area_struct* create_vma_code(struct mm_struct*, char*);
extern struct mm_struct* mm_struct_create();
extern void dup_mm_struct(struct mm_struct*, struct mm_struct*);
extern void mm_struct_destroy(struct mm_struct*);
extern void dup_vma_stack(struct mm_struct*, struct mm_struct*);
uint64_t mmap(void*, size_t, int, int, int, int);
extern struct vm_area_struct* create_vma_vc(struct mm_struct*);
extern struct vm_area_struct* find_vma(struct mm_struct*, uint64_t);
extern void* sys_mmap(void*, size_t, int, int, int, int);
#endif
