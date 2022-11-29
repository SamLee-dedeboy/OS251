#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

typedef uint32_t *TContext;
typedef uint32_t (*TEntry)(void *param);

// typedef uint32_t TCPUInterruptState, *TCPUInterruptStateRef;
// typedef uint32_t *TCPUStackRef;
// typedef uint32_t (*TCPUContextEntry)(void *param);

// __attribute__((always_inline)) inline TCPUInterruptState CPUHALSuspendInterrupts(void)
// {
//     uint32_t result;
//     asm volatile("csrrci %0, mstatus, 0x8"
//                  : "=r"(result));
//     return result;
// }
// __attribute__((always_inline)) inline void CPUHALResumeInterrupts(TCPUInterruptState state)
// {
//     asm volatile("csrs mstatus, %0"
//                  :
//                  : "r"(state & 0x8));
// }

// __attribute__((always_inline)) inline void CPUHALEnableInterrupts(void)
// {
//     asm volatile("csrsi mstatus, 0x8");
// }

// __attribute__((always_inline)) inline void CPUHALDisableInterrupts(void)
// {
//     asm volatile("csrci mstatus, 0x8");
// }

TContext CPUContextInitialize(uint32_t *stacktop, TContext entry, void *param);
void CPUContextSwitch(TContext *old, TContext new);

#endif