#include <stdint.h>
#include "Systemcall.h"
#include "Thread.h"

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

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

#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))
#define CONTROLLER (*((volatile uint32_t *)0x40000018))
#define MODE_CONTROL_REG (*((volatile uint32_t *)0x500FF414))

volatile uint32_t *smallspritecontrol = (volatile uint32_t *)(0x500FF214);

void init(void)
{
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while (Base < End)
    {
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while (Base < End)
    {
        *Base++ = 0;
    }

    csr_write_mie(0x888);    // Enable all interrupt soruces
    csr_enable_interrupts(); // Global interrupt enable
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;

    /*Sprite Control and palette initialized
        ctr_bits = 0001 1111 1110 0001 0000 0000 0100 0000 */
    volatile uint32_t *palette0 = (volatile uint32_t *)(0x500FD000);
    volatile uint32_t *palette1 = (volatile uint32_t *)(0x500FD400);

    for (int i = 0; i < 256; i++)
    {
        palette0[i] = 0xff000000 + i;
        // palette1[i] = 0xff0000ff - i;
    }

    // init small sprite location
    // 0x 000 1111 1110 000010000 0000010000 00
    smallspritecontrol[0] = 0x1fc10040;
}

extern volatile int global;
extern volatile uint32_t controller_status;

volatile uint32_t *INT_PENDING_REG = (volatile uint32_t *)(0x40000004);
void c_interrupt_handler(uint32_t mcause)
{
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH) << 32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare >> 32;
    MTIMECMP_LOW = NewCompare;
    global++;
    controller_status = CONTROLLER;

    // CMD control
    if ((((*INT_PENDING_REG) & 0x4) >> 2))
    {
        if (MODE_CONTROL_REG == 0x1)
            MODE_CONTROL_REG = 0x00000000;
        else if (MODE_CONTROL_REG == 0x0)
            MODE_CONTROL_REG = 0x00000001;
    }
    (*INT_PENDING_REG) |= ~(1U << 2);
    // CMD control end
}

uint32_t c_system_call(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t call)
{
    switch (a0)
    {
    case SYSTIMER:
        return global;
        break;

    case CONTROLLER_STATUS:
        return CONTROLLER;
        break;

    case MODE_STATUS:
        return MODE_CONTROL_REG;
        break;

    case SMALL_SPRITE_DROP:
        smallspritecontrol[0] += 0x00001000;
        return 1;
        break;

    case Thread_INIT:
        uint32_t ThreadStack[128];
        return CPUContextInitialize(ThreadStack + 128, (TContext)a1, (void *)a2);
        break;

    case Thread_SWITCH:
        return 1;
        break;
    }

    return -1;
}
