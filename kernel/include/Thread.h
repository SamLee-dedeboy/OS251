#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

#define MAX_THREAD_NUM 10

typedef uint32_t TContextState;
typedef uint32_t *TContext;
typedef uint32_t (*TContextEntry)(void *param);

TContext ContextInitialize(TContext stacktop, TContextEntry entry, void *param);
void ContextSwitch(TContext *storecurrent, TContext restore);

__attribute__((always_inline)) inline TContextState CPUHALSuspendInterrupts(void)
{
    uint32_t result;
    asm volatile("csrrci %0, mstatus, 0x8"
                 : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void CPUHALResumeInterrupts(TContextState state)
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