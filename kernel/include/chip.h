#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef CHIP_H
#define CHIP_H

typedef void (*Tfunc)(void);

// Define const address
#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))
#define CONTROLLER (*((volatile uint32_t *)0x40000018))
#define CARTIDGE_STATUS (*((volatile uint32_t *)0x4000001c))
#define MODE_CONTROL (*((volatile uint32_t *)0x500FF414))
#define MACHINE_TIME_REGISTER (*((volatile uint32_t *)0x40000008))
#define MACHINE_PERIOD_REGISTER (*((volatile uint32_t *)0x40000044))

#define CIS 0x00000001
#define TIMER_INTERRUPT 0x80000007
#define EXTERNAL_INTERRUPT 0x8000000b
#define ILLEGAL_INST_INTERRUPT 0x00000002
#define RESTART_ADDRESS 0x00000000
#define COMMAND_BIT 0x00000004
#define VIDEO_BIT 0x00000002
#define CART_BIT 0x00000001

// Adapted from https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code
__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void)
{
    uint32_t result;
    asm volatile("csrr %0, mstatus"
                 : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val)
{
    asm volatile("csrw mstatus, %0"
                 :
                 : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val)
{
    asm volatile("csrw mie, %0"
                 :
                 : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void)
{
    asm volatile("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void)
{
    asm volatile("csrci mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_write_mepc(uint32_t val)
{
    asm volatile("csrw mepc, %0"
                 :
                 : "r"(val));
}

__attribute__((always_inline)) inline uint32_t csr_mepc_read(void)
{
    uint32_t result;
    asm volatile("csrr %0, mepc"
                 : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline uint32_t csr_mcause_read(void)
{
    uint32_t result;
    asm volatile("csrr %0, mcause"
                 : "=r"(result));
    return result;
}

#endif