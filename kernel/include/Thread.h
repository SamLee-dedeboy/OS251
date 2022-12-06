#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

#define MAX_THREAD_NUM 10

typedef uint32_t TInterruptState, *TInterruptStateRef;
typedef uint32_t *TStackRef;
typedef uint32_t (*TContextEntry)(void *param);

TStackRef ContextInitialize(TStackRef stacktop, TContextEntry entry, void *param);
void ContextSwitch(TStackRef *storecurrent, TStackRef restore);

__attribute__((always_inline)) inline TInterruptState CPUHALSuspendInterrupts(void)
{
    uint32_t result;
    asm volatile("csrrci %0, mstatus, 0x8"
                 : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void CPUHALResumeInterrupts(TInterruptState state)
{
    asm volatile("csrs mstatus, %0"
                 :
                 : "r"(state & 0x8));
}

__attribute__((always_inline)) inline void CPUHALEnableInterrupts(void)
{
    asm volatile("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void CPUHALDisableInterrupts(void)
{
    asm volatile("csrci mstatus, 0x8");
}

#endif