#include <stdint.h>
#include <stdio.h>
#include "Thread.h"
#include "Chip.h"
#include "Systemcall.h"

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

typedef int bool;
#define true 1
#define false 0

volatile uint32_t *smallspritecontrol = (volatile uint32_t *)(0x500FF214);

// Thread variable
TContext ThreadPointers[MAX_THREAD_NUM];
int current_thread_num = 1;
int running_thread_pointer = 0;

volatile uint32_t *IER = (volatile uint32_t *)(0x40000000);
volatile uint32_t *IPR = (volatile uint32_t *)(0x40000004);

volatile int global = 0;
volatile int video = 0;

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
    *IER = *IER | COMMAND_BIT;
    *IER = *IER | VIDEO_BIT;
    *IER = *IER | CART_BIT;

    // palette initialized
    volatile uint32_t *palette0 = (volatile uint32_t *)0x500FD000;
    volatile uint32_t *palette1 = (volatile uint32_t *)0x500FD400;

    for (int i = 0; i < 256; i++)
    {
        palette0[i] = 0xff000000 + i;
        // palette1[i] = 0xff0000ff - i;
    }

    // init small sprite location
    // 0x 000 1111 1110 000010000 0000010000 00
    smallspritecontrol[0] = 0x1fc10040;
}

void c_interrupt_handler(void)
{
    uint32_t mcause = csr_mcause_read();
    switch (mcause)
    {
    case TIMER_INTERRUPT:
        thread_timer_scheduel();
        break;
    case EXTERNAL_INTERRUPT:
    {
        if (*IPR & COMMAND_BIT)
        {
            cmd_interrupt();
            return;
        }
        if (*IPR & VIDEO_BIT)
        {
            video_interrupt();
            return;
        }
    }
    default:
        break;
    }
}

void thread_timer_scheduel()
{
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH) << 32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare >> 32;
    MTIMECMP_LOW = NewCompare;
    global++;
    if (global % 500 == 0 && current_thread_num > 1)
    {
        uint32_t mepc = csr_mepc_read();
        TContextState PrevState = CPUHALSuspendInterrupts();
        ContextSwitch(&ThreadPointers[running_thread_pointer], ThreadPointers[(running_thread_pointer + 1) % current_thread_num]);
        running_thread_pointer++;
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
        return MODE_CONTROL;
        break;
    case SMALL_SPRITE_DROP:
        smallspritecontrol[0] += 0x00001000;
        return 1;
        break;
    case WRITE_TEXT:
        printf((char *)param2, (int)param3);
        fflush(stdout);
        break;
    case Thread_INIT:
        uint32_t ThreadStack[128];
        if (current_thread_num <= MAX_THREAD_NUM - 1)
        {
            ThreadPointers[current_thread_num++] = ContextInitialize((TContext)(ThreadStack + 128), (TContextEntry)param2, (void *)param3);
        }
        printf((char *)current_thread_num, (int)current_thread_num);
        fflush(stdout);
        break;
    default:
        break;
    }
}