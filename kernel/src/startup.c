#include <stdint.h>
#include <stdio.h>
#include "chip.h"
#include "syscallinterr.h"
#include "thread.h"

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

typedef void (*Tfunc)(void);

volatile uint32_t *smallspritecontrol = (volatile uint32_t *)(0x500FF214);
volatile uint32_t *BGpalette0 = (volatile uint32_t *)0x500FC000;
volatile uint32_t *IER = (volatile uint32_t *)(0x40000000);
volatile uint32_t *IPR = (volatile uint32_t *)(0x40000004);
// Thread variable
uint32_t ThreadStack[9][2048];
TStackRef ThreadPointers[10];
int current_thread_num = 1;
int running_thread_pointer = 0;

volatile int video = 0;

extern volatile int global;
extern volatile uint32_t controller_status;
volatile uint32_t *INT_PENDING_REG = (volatile uint32_t *)(0x40000004);
int color = 1;
int color_counter = 0;

static unsigned int seed = 1;

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

    // *IER = *IER | COMMAND_BIT;
    // *IER = *IER | VIDEO_BIT;
    // *IER = *IER | CART_BIT;
}

void srand(uint32_t newseed)
{
    seed = (unsigned)newseed & 0x7fffffffU;
}
uint32_t rand(void)
{
    seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
    return (uint32_t)seed;
}

void c_interrupt_handler(void)
{
    uint32_t mcause = csr_mcause_read();
    switch (mcause)
    {
    case TIMER_INTERRUPT:
        timer_interrupt();
        break;
    case ILLEGAL_INST_INTERRUPT:
        illegal_inst_interrupt();
        break;
    case EXTERNAL_INTERRUPT:
    {
        // if (*IPR & COMMAND_BIT)
        // {
        //     cmd_interrupt();
        //     return;
        // }
        if (*IPR & VIDEO_BIT)
        {
            video_interrupt();
            return;
        }
        if (*IPR & CART_BIT)
        {
            cart_interrupt();
            return;
        }
    }
    default:
        break;
    }
    (*INT_PENDING_REG) |= ~(1U << 2);
}

void illegal_inst_interrupt()
{
    csr_write_mepc(RESTART_ADDRESS);
}

void timer_interrupt()
{
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH) << 32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare >> 32;
    MTIMECMP_LOW = NewCompare;
    global++;
    if (current_thread_num >= 2)
    {
        uint32_t mepc = csr_mepc_read();
        printf("\n");
        TInterruptState PrevState = CPUHALSuspendInterrupts();
        int t1, t2;
        t1 = running_thread_pointer;
        t2 = (running_thread_pointer + 1) % current_thread_num;
        running_thread_pointer = t2;
        ContextSwitch(&ThreadPointers[t1], ThreadPointers[t2]);
        csr_write_mepc(mepc);
        CPUHALResumeInterrupts(PrevState);
    }
    if (global == 9999)
    {
        global = 0;
    }
}

void cmd_interrupt()
{
    if (MODE_CONTROL & 1)
    {
        MODE_CONTROL = 0x00000000;
    }
    else
    {
        MODE_CONTROL = 0x00000001;
    }
    *IPR = *IPR & COMMAND_BIT;
}
void video_interrupt()
{
    video++;
    if (video >= 1000)
    {
        video = 0;
    }
    *IPR = *IPR & VIDEO_BIT;
}
void cart_interrupt()
{
    if (CARTIDGE_STATUS & CIS)
    {
        *IPR = *IPR & CART_BIT;
    }
    else
    {
        *IPR = *IPR & CART_BIT;
    }
}

uint32_t c_syscall(uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4)
{
    switch (param1)
    {
    case SYSTIMER:
        return global;
        break;

    case CONTROLLER_STATUS:
        return CONTROLLER;
        break;

    case MODE_STATUS:
        if (MODE_CONTROL & 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;

    case SMALL_SPRITE_DROP:
        smallspritecontrol[0] += 0x00001000;
        return 1;
        break;

    case READ_MACHINE_TIME:
        return MACHINE_TIME_REGISTER;

    case READ_MACHINE_PERIOD:
        return MACHINE_PERIOD_REGISTER;

    case READ_INT_PENDING_REG:
        return INT_PENDING_REG;

    case RAND:
        srand(MACHINE_TIME_REGISTER);
        return rand();
    case SYSVIDEO:
        return video;
        break;
    case WRITE_TEXT:
        printf((char *)param2, (int)param3);
        fflush(stdout);
        break;

    case THREAD_INITIALLIZE:
        if (current_thread_num <= 10)
        {
            ThreadPointers[current_thread_num] = ContextInitialize((TStackRef)(ThreadStack[current_thread_num - 1] + 2048), (TContextEntry)param2, (void *)param3);
            current_thread_num++;
        }
        break;

    default:
        break;
    }
}
