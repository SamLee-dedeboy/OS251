#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef GRAPHIC_H
#define GRAPHIC_H 

#define TEXT_BUFF_SIZE               2304
#define IMAGE_BUFF_SIZE              147456 
#define FULL_X                       512
#define FULL_Y                       288
#define TOP                          4
#define BOTTOM                       0
#define TEXT_MODE                    0
#define GRAPHICS_MODE                1

#define merge_arg(a, b) ((a<<16) | (b))

// Video Controls
int setVideoMode(uint32_t mode);
void setRefreshRate(uint32_t rate);

// Get video memory addresses.
uint32_t getBACKGROUND_DATA_ADDRESS(uint32_t background_num);
uint32_t getSPRITE_DATA_ADDRESS(uint32_t sprite_num); 
uint32_t getBACKGROUND_PALLETE_ADDRESS(uint32_t palette_num, uint32_t entry_num); 
uint32_t getSPRITE_PALLETE_ADDRESS(uint32_t palette_num, uint32_t entry_num); 
uint32_t getTEXT_DATA_ADDRESS(); 
uint32_t getBACKGROUND_CONTROL_ADDRESS(uint32_t background_num);
uint32_t getSPRITE_CONTROL_ADDRESS(uint32_t sprite_num);
uint32_t getMODE_CONTROL_REGISTER(); 

// Backgrounds
int setBackgroundPalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB);
int backgroundDrawRec(uint32_t background_num, uint32_t xy, uint32_t wh, uint32_t colorEntry);
int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num);
int changeBackgroundPalette(uint32_t background_num, uint32_t palette_num);

// Sprites
int setSpritePalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB);
uint16_t createRecSprite(uint32_t xy, uint32_t wh, uint32_t palette_num, uint32_t colorEntry);
int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y);
int changeSpritePalette(uint32_t sprite_num, uint32_t palette_num);
uint32_t calcSmallSpriteControl(uint32_t xy, uint32_t wh, uint32_t p);
uint32_t calcLargeSpriteControl(uint32_t xy, uint32_t wh, uint32_t p);

// Text 
int drawText(char* text, uint32_t length, uint32_t x, uint32_t y);
void clearTextScreen();

#endif