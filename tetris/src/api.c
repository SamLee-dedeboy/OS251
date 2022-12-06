#include "api.h"

uint32_t getTimer()
{
    return systemcall(SYSTIMER, 0, 0, 0, 0, 0);
}

uint32_t getStatus()
{
    return systemcall(CONTROLLER_STATUS, 0, 0, 0, 0, 0);
}

uint32_t getMode()
{
    return systemcall(MODE_STATUS, 0, 0, 0, 0, 0);
}

uint32_t spriteDrop()
{
    return systemcall(SMALL_SPRITE_DROP, 0, 0, 0, 0, 0);
}

uint32_t getMachineTime()
{
    return systemcall(READ_MACHINE_TIME, 0, 0, 0, 0, 0);
}
uint32_t getMachinePeriod()
{
    return systemcall(READ_MACHINE_PERIOD, 0, 0, 0, 0, 0);
}
uint32_t getIntPendingReg() 
{
    return systemcall(READ_INT_PENDING, 0, 0, 0, 0, 0);
}
uint32_t rand()
{
    return systemcall(RAND, 0, 0, 0, 0, 0);
}
uint32_t getBACKGROUND_DATA_ADDRESS(uint32_t background_num)
{
    return systemcall(get_BACKGROUND_DATA_ADDRESS, background_num, 0, 0, 0, 0);
}
uint32_t getLARGE_SPRITE_DATA_ADDRESS(uint32_t large_sprite_count)
{
    return systemcall(get_LARGE_SPRITE_DATA_ADDRESS, large_sprite_count, 0, 0, 0, 0);
}
uint32_t getSMALL_SPRITE_DATA_ADDRESS(uint32_t small_sprite_count)
{
    return systemcall(get_SMALL_SPRITE_DATA_ADDRESS, small_sprite_count, 0, 0, 0, 0);
}
uint32_t getBACKGROUND_PALLETE_ADDRESS(uint32_t palette_num_test, uint32_t entry_num) 
{
    return systemcall(get_BACKGROUND_PALLETE_ADDRESS, palette_num_test, entry_num, 0, 0, 0);
}
uint32_t getSPRITE_PALLETE_ADDRESS(uint32_t palette_num_test, uint32_t entry_num) 
{
    return systemcall(get_SPRITE_PALLETE_ADDRESS, palette_num_test, entry_num, 0, 0, 0);
}
uint32_t getTEXT_DATA_ADDRESS() 
{
    return systemcall(get_TEXT_DATA_ADDRESS, 0, 0, 0, 0, 0);
}
uint32_t getBACKGROUND_CONTROL_ADDRESS(uint32_t background_num) 
{
    return systemcall(get_BACKGROUND_CONTROL_ADDRESS, background_num, 0, 0, 0, 0);
}
uint32_t getLARGE_SPRITE_CONTROL_ADDRESS(uint32_t large_sprite_count)
{
    return systemcall(get_LARGE_SPRITE_CONTROL_ADDRESS, large_sprite_count, 0, 0, 0, 0);
}
uint32_t getSMALL_SPRITE_CONTROL_ADDRESS(uint32_t small_sprite_count)
{
    return systemcall(get_SMALL_SPRITE_CONTROL_ADDRESS, small_sprite_count, 0, 0, 0, 0);
}
uint32_t getMODE_CONTROL_REGISTER() 
{
    return systemcall(get_MODE_CONTROL_REGISTER, 0, 0, 0, 0, 0);
}