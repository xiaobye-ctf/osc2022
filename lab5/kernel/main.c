#include "types.h"
#include "peripherals/mini_uart.h"
#include "init/kernel_init.h"
#include "debug/debug.h"
#include "kernel/shell.h"
#include "kernel/sched/sched.h"
#include "kernel/sched/kthread.h"

void kernel_main_thread(void){
    INFO("create kthread kthread_idle");
    kthread_create(kthread_idle);
    for(uint32_t i = 0 ; i < 10 ; i++){
        INFO("create kthread kthread_test");
        kthread_create(kthread_test);
    }
    INFO("create kthread simple_shell");
    kthread_create(simple_shell);
    kthread_exit();
}
void kernel_main(void *dtb){
    kernel_init(dtb);
    DEBUG_KERNEL_START();
    
    INFO("create kernel main thread");
    kthread_create(kernel_main_thread);
}

