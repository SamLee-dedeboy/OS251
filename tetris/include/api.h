#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef API_H_
#define API_H_
#include "graphic.h"
#include "systemcall.h"

// define sys call func parameters
#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))
#define CONTROLLER (*((volatile uint32_t *)0x40000018))


typedef uint32_t *TContext;
typedef void (*TEntry)(void *);

uint32_t systemcall(uint32_t funName);

uint32_t getTimer();
uint32_t getStatus();
uint32_t getMode();
uint32_t spriteDrop();
uint32_t initThread(void);
uint32_t getMachineTime();
uint32_t getMachinePeriod();
uint32_t getIntPendingReg();
uint32_t rand();

#endif