#include "api.h"

uint32_t getTimer()
{
    return systemcall(SYSTIMER);
}

uint32_t getStatus()
{
    return systemcall(CONTROLLER_STATUS);
}

uint32_t getMode()
{
    return systemcall(MODE_STATUS);
}

uint32_t spriteDrop()
{
    return systemcall(SMALL_SPRITE_DROP);
}

uint32_t getMachineTime()
{
    return systemcall(READ_MACHINE_TIME);
}
uint32_t getMachinePeriod()
{
    return systemcall(READ_MACHINE_PERIOD);
}
uint32_t getIntPendingReg() 
{
    return systemcall(READ_INT_PENDING);
}
uint32_t rand()
{
    return systemcall(RAND);
}