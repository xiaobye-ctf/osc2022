#ifndef __UART_H__
#define __UART_H__

#include "gpio.h"
#include "mmio.h"

#define AUX_BASE            (MMIO_BASE + 0x00215000)
#define AUX_IRQ             (AUX_BASE + 0x0)
#define AUX_ENABLES         (AUX_BASE + 0x4)
#define AUX_MU_IO_REG       (AUX_BASE + 0x40)
#define AUX_MU_IER_REG      (AUX_BASE + 0x44)
#define AUX_MU_IIR_REG      (AUX_BASE + 0x48)
#define AUX_MU_LCR_REG      (AUX_BASE + 0x4c)
#define AUX_MU_MCR_REG      (AUX_BASE + 0x50)
#define AUX_MU_LSR_REG      (AUX_BASE + 0x54)
#define AUX_MU_MSR_REG      (AUX_BASE + 0x58)
#define AUX_MU_SCRATCH      (AUX_BASE + 0x5c)
#define AUX_MU_CNTL_REG     (AUX_BASE + 0x60)
#define AUX_MU_STAT_REG     (AUX_BASE + 0x64)
#define AUX_MU_BAUD_REG     (AUX_BASE + 0x68)
#define AUX_SPI0_CNTL0_REG  (AUX_BASE + 0x80)
#define AUX_SPI0_CNTL1_REG  (AUX_BASE + 0x84)
#define AUX_SPI0_STAT_REG   (AUX_BASE + 0x88)
#define AUX_SPI0_IO_REG     (AUX_BASE + 0x90)
#define AUX_SPI0_PEEK_REG   (AUX_BASE + 0x94)
#define AUX_SPI1_CNTL0_REG  (AUX_BASE + 0xc0)
#define AUX_SPI1_CNTL1_REG  (AUX_BASE + 0xc4)
#define AUX_SPI1_STAT_REG   (AUX_BASE + 0xc8)
#define AUX_SPI1_IO_REG     (AUX_BASE + 0xd0)
#define AUX_SPI1_PEEK_REG   (AUX_BASE + 0xd4)

void uart_init();
void uart_write(char c);
void uart_read(char* buf, uint32_t size);
void uart_flush();
void uart_write_string(char* str);
void uart_puth(uint32_t d);
void uart_putc(char* buf, uint32_t size);
void delay(uint32_t t) {
    for (uint32_t i = 0; i < t; i++)
        asm volatile("nop");
}

#endif