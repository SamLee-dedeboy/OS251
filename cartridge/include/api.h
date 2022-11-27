#ifndef API_H_
#define API_H_
#include <stdint.h>

typedef uint32_t *TContext;
typedef void (*TEntry)(void *);

// define sys call func parameters
#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))
#define CONTROLLER (*((volatile uint32_t *)0x40000018))

uint32_t initThread(void);
uint32_t systemcall(uint32_t funName);

uint32_t getTimer();
uint32_t getStatus();
uint32_t getMode();
uint32_t spriteDrop();

#endif