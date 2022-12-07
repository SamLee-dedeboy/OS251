#ifndef TMP_H_
#define TMP_H_

#ifndef GRAPHIC_H
#define GRAPHIC_H
#include "video_api.h"
#endif


#define TEXT_DATA_ADDRESS 0x500FE800            // data size: char
#define BACKGROUND_PALLETE_ADDRESS 0x500FC000   // data size: uint32_t
#define BACKGROUND_DATA_ADDRESS 0x50000000      // data size: uint8_t
#define BACKGROUND_CONTROL_ADDRESS 0x500FF100   // data size: uint32_t
#define SPRITE_PALLETE_ADDRESS 0x500FD000       // data size: uint32_t
#define MODE_CONTROL (*((volatile uint32_t *)0x500FF414))
#define LARGE_SPRITE_DATA_ADDRESS 0x500B4000    // data size: uint8_t2
#define LARGE_SPRITE_CONTROL_ADDRESS 0x500FF114 // data size: uint32_t
#define SMALL_SPRITE_CONTROL_ADDRESS 0x500FF214 // data size: uint32_t
#define CONTROLLER (*((volatile uint32_t *)0x40000018))
#define MODE_CONTROL_REGISTER 0x500FF414

#define TEXT_BUFF_SIZE               2304
#define IMAGE_BUFF_SIZE              147456 
#define FULL_X                       512
#define FULL_Y                       288
#define TOP                          4
#define BOTTOM                       0
#define TEXT_MODE                    0
#define GRAPHICS_MODE                1

// --- 
// int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num);
// int setVideoMode(uint32_t mode);

// ---
int drawText(char* text, uint32_t length, uint32_t x, uint32_t y);
int setBackgroundPalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB);
int backgroundDrawRec(uint32_t background_num, uint32_t xy, uint32_t wh, uint32_t colorEntry);
int setSpritePalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB);
uint32_t getMode();
uint32_t getLARGE_SPRITE_DATA_ADDRESS(uint32_t large_sprite_count); 
uint32_t getLARGE_SPRITE_CONTROL_ADDRESS(uint32_t large_sprite_count); 
uint32_t calcLargeSpriteControl(uint32_t xy, uint32_t wh, uint32_t p);
int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y);
uint32_t getBACKGROUND_DATA_ADDRESS(uint32_t background_num); 
uint32_t merge_xy(uint32_t x, uint32_t y);
uint32_t unmerge(uint32_t xy, int flag);
int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num);
int setVideoMode(uint32_t mode);


#endif