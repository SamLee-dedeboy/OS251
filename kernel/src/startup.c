#include <stdint.h>
#include "Systemcall.h"

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
#define MACHINE_TIME_REGISTER (*((volatile uint32_t *)0x40000008))
#define MACHINE_PERIOD_REGISTER (*((volatile uint32_t *)0x40000040))
#define INT_ENABLE_REG (*(volatile uint32_t *)(0x40000000))
#define INT_PENDING_REG (*(volatile uint32_t *)(0x40000004))

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

    // /*Sprite Control and palette initialized
    //     ctr_bits = 0001 1111 1110 0001 0000 0000 0100 0000 */
    // volatile uint32_t *palette0 = (volatile uint32_t *)(0x500FD000);
    // volatile uint32_t *palette1 = (volatile uint32_t *)(0x500FD400);


    // for (int i = 0; i < 256; i++)
    // {
    //     palette0[i] = 0xff000000 + i;
    //     // palette1[i] = 0xff0000ff - i;
    // }

    // // init small sprite location
    // // 0x 000 1111 1110 000010000 0000010000 00
    // smallspritecontrol[0] = 0x1fc10040;
}

extern volatile int global;
extern volatile uint32_t controller_status;
int color = 1;
int color_counter = 0;

static unsigned int seed = 1;
void srand (uint32_t newseed) {
    seed = (unsigned)newseed & 0x7fffffffU;
}
uint32_t rand (void) {
    seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
    return (uint32_t)seed;
}

void c_interrupt_handler(uint32_t mcause)
{
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH) << 32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare >> 32;
    MTIMECMP_LOW = NewCompare;
    global++;
    controller_status = CONTROLLER;

    // CMD control
    // if ((((*INT_PENDING_REG) & 0x4) >> 2))
    // {
    //     if (MODE_CONTROL_REG == 0x1)
    //         MODE_CONTROL_REG = 0x00000000;
    //     else if (MODE_CONTROL_REG == 0x0) {
    //         MODE_CONTROL_REG = 0x00000001;
    //     }
    // }
    // (*INT_PENDING_REG) |= ~(1U << 2);
    // CMD control end
}

typedef uint32_t *TContext;
typedef void (*TEntry)(void *);

TContext InitContext(uint32_t *stacktop, TEntry entry, void *param);

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

    case READ_MACHINE_TIME:
        return MACHINE_TIME_REGISTER;

    case READ_MACHINE_PERIOD:
        return MACHINE_PERIOD_REGISTER;

    // CMD Interrupt 
    case READ_INT_PENDING_REG:
        INT_ENABLE_REG = INT_ENABLE_REG | 0x4;
        INT_PENDING_REG = 0x0;
        return INT_PENDING_REG;
    
    case RAND:
        srand(MACHINE_TIME_REGISTER);
        return rand();
    }

    // Thread
    if (call == 3)
    {
        uint32_t ThreadStack[128];
        return InitContext(ThreadStack + 128, (void *)0, (void *)0);
    }
    return -1;
}
