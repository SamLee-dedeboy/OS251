#include "api.h"

uint32_t getTimer()
{
    return systemcall(SYSTIMER, 0, 0, 0, 0, 0);
}
uint32_t getStatus()
{
    return systemcall(CONTROLLER_STATUS, 0, 0, 0, 0, 0);
}
uint32_t enableCMD() {
    return systemcall(ENABLE_CMD, 0, 0, 0, 0, 0);
}
uint32_t disableCMD() {
    return systemcall(DISABLE_CMD, 0, 0, 0, 0, 0);
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
uint32_t Rand_sys()
{
    return systemcall(RAND, 0, 0, 0, 0, 0);
}
uint32_t getBACKGROUND_DATA_ADDRESS(uint32_t background_num)
{
    return systemcall(get_BACKGROUND_DATA_ADDRESS, background_num, 0, 0, 0, 0);
}
uint32_t getSPRITE_DATA_ADDRESS(uint32_t sprite_num)
{
    if(sprite_num < 128) return systemcall(get_SMALL_SPRITE_DATA_ADDRESS, sprite_num, 0, 0, 0, 0);
    return systemcall(get_LARGE_SPRITE_DATA_ADDRESS, sprite_num-128, 0, 0, 0, 0);
}
uint32_t getBACKGROUND_PALLETE_ADDRESS(uint32_t palette_num, uint32_t entry_num) 
{
    return systemcall(get_BACKGROUND_PALLETE_ADDRESS, palette_num, entry_num, 0, 0, 0);
}
uint32_t getSPRITE_PALLETE_ADDRESS(uint32_t palette_num, uint32_t entry_num) 
{
    return systemcall(get_SPRITE_PALLETE_ADDRESS, palette_num, entry_num, 0, 0, 0);
}
uint32_t getTEXT_DATA_ADDRESS() 
{
    return systemcall(get_TEXT_DATA_ADDRESS, 0, 0, 0, 0, 0);
}
uint32_t getBACKGROUND_CONTROL_ADDRESS(uint32_t background_num) 
{
    return systemcall(get_BACKGROUND_CONTROL_ADDRESS, background_num, 0, 0, 0, 0);
}
uint32_t getSPRITE_CONTROL_ADDRESS(uint32_t sprite_num)
{
    if(sprite_num < 128) return systemcall(get_SMALL_SPRITE_CONTROL_ADDRESS, sprite_num, 0, 0, 0, 0);
    return systemcall(get_LARGE_SPRITE_CONTROL_ADDRESS, sprite_num-128, 0, 0, 0, 0);
}
uint32_t getMODE_CONTROL_REGISTER() 
{
    return systemcall(get_MODE_CONTROL_REGISTER, 0, 0, 0, 0, 0);
}
int setVideoMode(uint32_t mode) 
{
    return systemcall(VideoMode, mode, 0, 0, 0, 0);
}
void setRefreshRate(uint32_t rate)
{
    systemcall(RefreshRate, rate, 0, 0, 0, 0);
    return;
}
int setBackgroundPalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB)
{
    return systemcall(setBackgroundPalette_id, palette_num, entry_num, ARGB, 0, 0);
}
int backgroundDrawRec(uint32_t background_num, uint32_t xy, uint32_t wh, uint32_t colorEntry)
{
    return systemcall(backgroundDrawRec_id, background_num, xy, wh, colorEntry, 0);
}
int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num)
{
    return systemcall(setBackgroundControl_id, background_num, xy, z, palette_num, 0);
}
int changeBackgroundPalette(uint32_t background_num, uint32_t palette_num)
{
    return systemcall(changeBackgroundPalette_id, background_num, palette_num, 0, 0, 0);
}
int setSpritePalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB)
{
    return systemcall(setSpritePalette_id, palette_num, entry_num, ARGB, 0, 0);
}
uint16_t createRecSprite(uint32_t xy, uint32_t wh, uint32_t palette_num, uint32_t colorEntry)
{
    return systemcall(createRecSprite_id, xy, wh, palette_num, colorEntry, 0);
}
int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y)
{
    return systemcall(moveSprite_id, sprite_num, d_x, d_y, 0, 0);
}
int changeSpritePalette(uint32_t sprite_num, uint32_t palette_num)
{
    return systemcall(changeSpritePalette_id, sprite_num, palette_num, 0, 0, 0);
}
uint32_t calcSmallSpriteControl(uint32_t xy, uint32_t wh, uint32_t p)
{
    return systemcall(calcSmallSpriteControl_id, xy, wh, p, 0, 0);
}
uint32_t calcLargeSpriteControl(uint32_t xy, uint32_t wh, uint32_t p)
{
    return systemcall(calcLargeSpriteControl_id, xy, wh, p, 0, 0);
}
int drawText(char* text, uint32_t length, uint32_t x, uint32_t y)
{
    return systemcall(drawText_id, length, x, y, 0, text);
}
void clearTextScreen()
{
    systemcall(clearTextScreen_id, 0, 0, 0, 0, 0);
    return;
}