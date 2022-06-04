#ifndef _SCHED_H_
#define _SCHED_H_

#include "lib/list.h"
#include "kernel/irq_handler.h"

typedef uint64_t pid_t;


#define TASK_RUNNING        0
#define TASK_INTERRUPTIBLE  1
#define TASK_UNINTERRUPTIBLE    2
#define TASK_DEAD		64
struct thread_info{
    pid_t pid;
    volatile int64_t state;
};

struct sched_info{
    uint64_t rticks; // running ticks = user + system ticks
    uint64_t priority;
    uint64_t counter; // reset counter -> counter = priority
    struct list_head sched_list; 
};

struct task_ctx{
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp; 
    uint64_t lr;
    uint64_t sp;
    
}__attribute__((packed));

struct mm_struct{
    void *mmap;
};
struct task_struct{
    struct task_ctx ctx;

    struct thread_info thread_info;

    struct mm_struct* mm;

    // kernel stack
    void* stack;

    // schedule info
    struct sched_info sched_info;

    // relationship
    struct task_struct* parent;
    struct task_struct* child;
    struct list_head siblings;
};

extern int need_sched;

extern void add_task_to_rq(struct task_struct *task);
extern struct task_struct* pick_next_task_from_rq();
extern void update_sched_info(struct task_struct*);
extern void schedule();
extern void switch_to(struct task_struct*, struct task_struct*);
extern struct task_struct* get_current();
extern pid_t get_pid_counter(void);
#endif
