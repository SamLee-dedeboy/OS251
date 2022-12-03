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

#define BACKGROUND_DATA_ADDRESS 0x50000000 // data size: uint8_t
#define LARGE_SPRITE_DATA_ADDRESS 0x500B4000 // data size: uint8_t
#define SMALL_SPRITE_DATA_ADDRESS 0x500F4000 // data size: uint8_t

#define BACKGROUND_PALLETE_ADDRESS 0x500FC000 // uint32_t
#define SPRITE_PALLETE_ADDRESS 0x500FD000 // uint32_t

#define TEXT_DATA_ADDRESS 0x500FE800 // data size: char

#define BACKGROUND_CONTROL_ADDRESS 0x500FF100 // data size: uint32_t
#define LARGE_SPRITE_CONTROL_ADDRESS 0x500FF114 // data size: uint32_t
#define SMALL_SPRITE_CONTROL_ADDRESS 0x500FF214 // data size: uint32_t

volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500FF414);

uint32_t SystemCall(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

void setVideoMode(int mode);
// uint32_t setBackgroundPalette(uint32_t image, uint32_t palette);
void setSpritePalette(uint32_t num, uint32_t ARGB);

uint32_t createSprite(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num);
void changeSpriteColor(uint32_t sprite_num, uint32_t color_num);
uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint32_t p);

void drawText(char* text, uint32_t length, int32_t x, int32_t y);
void clearTextScreen();

// void moveSprite(int sprite_num, int d_x, int d_y);
// uint32_t moveBackground(uint32_t image, uint32_t x, uint32_t y, uint32_t z);
// uint32_t backgroundFill(uint32_t background, uint8_t* buff, uint32_t size);

// uint32_t setRefreshRate(uint32_t rate);

#endif