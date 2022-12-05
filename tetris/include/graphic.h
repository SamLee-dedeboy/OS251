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

#define BACKGROUND_DATA_ADDRESS 0x50000000      // data size: uint8_t
#define LARGE_SPRITE_DATA_ADDRESS 0x500B4000    // data size: uint8_t
#define SMALL_SPRITE_DATA_ADDRESS 0x500F4000    // data size: uint8_t
#define BACKGROUND_PALLETE_ADDRESS 0x500FC000   // data size: uint32_t
#define SPRITE_PALLETE_ADDRESS 0x500FD000       // data size: uint32_t
#define TEXT_DATA_ADDRESS 0x500FE800            // data size: char
#define BACKGROUND_CONTROL_ADDRESS 0x500FF100   // data size: uint32_t
#define LARGE_SPRITE_CONTROL_ADDRESS 0x500FF114 // data size: uint32_t
#define SMALL_SPRITE_CONTROL_ADDRESS 0x500FF214 // data size: uint32_t
#define MODE_CONTROL_REGISTER 0x500FF414

// Definitions for tetris game
#define UNIT 16 // 16 pixels or 8 pixels
#define BLOCK_SIZE UNIT*4 // a block consists of 4UNIT * 4UNIT
#define MARGIN 10*(16/UNIT) // how many amount of UNIT

#define S_type 0
#define I_type 1
#define J_type 2
#define L_type 3
#define O_type 4
#define T_type 5
#define Z_type 6

uint32_t SystemCall(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

// Video Controls
int setVideoMode(int mode);
void setRefreshRate(uint8_t rate);

// Backgrounds
int setBackgroundPalette(uint8_t palette_num, uint32_t entry_num, uint32_t ARGB);
int backgroundDrawRec(uint8_t background_num, int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t colorEntry);
int setBackgroundControl(uint8_t background_num, int32_t x, int32_t y, int32_t z, uint8_t palette_num);
int changeBackgroundPalette(uint8_t background_num, uint8_t palette_num);

// Sprites
int setSpritePalette(uint8_t palette_num, uint32_t entry_num, uint32_t ARGB);
uint16_t createRecSprite(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t palette_num, uint8_t colorEntry);
int moveSprite(uint16_t sprite_num, uint32_t d_x, uint32_t d_y);
int changeSpritePalette(uint16_t sprite_num, uint8_t palette_num);
uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint8_t p);
uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t p);

// Texts
int drawText(char* text, uint32_t length, int32_t x, int32_t y);
void clearTextScreen();

// ---------Functions for tetris game-----------
uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x);
void rotateBlock(uint8_t block_type, uint8_t rotation);
void setBlockControl(uint8_t block_type, int32_t x, int32_t y, uint8_t palette_num);
// int moveBlock(uint16_t sprite_num, int32_t d_x, int32_t d_y); // add border conditions
int checkCollide_X(uint8_t block_type, uint8_t rotation, int32_t d_x);
int checkCollide_Y(uint8_t block_type, uint8_t rotation, int32_t d_y);

#endif