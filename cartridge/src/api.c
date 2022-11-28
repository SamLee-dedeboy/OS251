#include <stdint.h>
#include "api.h"
#include "systemcall.h"

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