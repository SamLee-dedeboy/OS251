#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

uint32_t getTimer();
uint32_t getStatus();
uint32_t enableCMD();
uint32_t disableCMD();
uint32_t getMode();
uint32_t spriteDrop();
uint32_t getMachineTime();
uint32_t getMachinePeriod();
uint32_t getIntPendingReg();
uint32_t Rand_sys(); // '_sys' to avoid nameing conflict with stdlib 'Rand()' function


#endif