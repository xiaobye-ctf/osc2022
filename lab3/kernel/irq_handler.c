#include "kernel/irq_handler.h"

struct softirq_status softirq_s = {
    .pending  = 0,
    .in_softirq = 0
};

irq_funcptr softirq_vec[END_OF_LIST] = {
    timer_softirq_callback,
    mini_uart_rx_softirq_callback,
    mini_uart_tx_softirq_callback
};
uint64_t irq_count[END_OF_LIST] = {0, 0, 0};

void enter_softirq(){
   softirq_s.in_softirq = 1; 
}

void exit_softirq(){
   softirq_s.in_softirq = 0; 
}

uint8_t in_softirq(){
    return softirq_s.in_softirq; 
}

uint8_t has_softirq_pending(){
    return softirq_s.pending ? 1 : 0;
} 

uint16_t get_softirq_pending(){
    return softirq_s.pending;
}

void set_softirq_pending(uint16_t val){
    softirq_s.pending = val;
}


void add_softirq_task(uint32_t softirq_nr){
    softirq_s.pending |= (1 << softirq_nr);
}

void do_softirq(){
    uint16_t try = 0;
    uint16_t pending;
    uint16_t softirq_bit;
    irq_funcptr softirq_handler;

    while(try < MAX_SOFTIRQ_TRY){
        pending = get_softirq_pending();
        set_softirq_pending(0);

        local_irq_enable();
        while(pending){
            softirq_bit = ffs16(pending);
            softirq_handler = softirq_vec[softirq_bit];
            softirq_handler();
            pending &= ~(1 << softirq_bit);
        }
        local_irq_disable();
        try++;
    }
}

void do_irq(uint32_t nr, irq_funcptr do_hardirq,irq_funcptr enable_device_irq , irq_funcptr disable_device_irq){
    disable_device_irq();
    irq_count[nr]++;
    do_hardirq();
    add_softirq_task(nr);

    if(!in_softirq() && has_softirq_pending()){
        enter_softirq();
        do_softirq();
        exit_softirq();
    }

    enable_device_irq();
}

void irq_handler(){
    uint32_t irq_pending_1 = IO_MMIO_read32(IRQ_PENDING_1);
    uint32_t core0_irq_source = IO_MMIO_read32(CORE0_INTERRUPT_SOURCE);
    uint32_t auxirq, uart_irq_type;
    if(core0_irq_source & 2){
        //core timer interrupt
        do_irq(CORE0_TIMER, core_timer_irq_handler, enable_core_timer_irq, disable_core_timer_irq);
    }else if(irq_pending_1 & (1 << 29)){
        auxirq = IO_MMIO_read32(AUX_IRQ); 
        if(auxirq & 1){
            uart_irq_type = (IO_MMIO_read32(AUX_MU_IIR_REG) >> 1) & (0b11);
            if(uart_irq_type == RX){
                // Receiver holds valid byte
                do_irq(MINI_UART_RX, mini_uart_irq_read, enable_mini_uart_rx_irq, disable_mini_uart_rx_irq);
            }else if(uart_irq_type == TX){
                // Transmit holding register empty
                do_irq(MINI_UART_TX, mini_uart_irq_write, enable_mini_uart_tx_irq, disable_mini_uart_tx_irq);
            }else{
                LOG("UART interrupt");
            }
        }else{
            LOG("Unkown AUX interrupt, DAIF: %x", get_DAIF());
        }
    }
    else{
        LOG("Unkown interrupt, DAIF: %x", get_DAIF());
    }
}


