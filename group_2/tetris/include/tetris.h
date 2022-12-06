#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef GRAPHIC_H
#define GRAPHIC_H
// #include "graphic.h"
#endif

// ---------Definitions for tetris game-----------

// block types
#define S_type 0
#define I_type 1
#define J_type 2
#define L_type 3
#define O_type 4
#define T_type 5
#define Z_type 6

// game states
#define WELCOME_PAGE_STATE 0
#define INIT_GAME_STATE 1
#define CREATE_BLOCK_STATE 2
#define DROP_BLOCK_STATE 3
#define DRAW_TO_BG_STATE 4
#define DELETE_FULL_LINE_STATE 5
#define GAME_OVER_STATE 6

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
#define MODE_CONTROL (*((volatile uint32_t *)0x500FF414))

// ---------Functions for tetris game-----------
void welcome_page_state();
void init_game_state(int *rotation);
void drop_block_state(int32_t sprite_num, int *rotation);
void delete_full_line_state();
uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x);
void rotateBlock(uint8_t block_type, uint8_t rotation);
void setBlockControl(uint8_t block_type, int32_t x, int32_t y, uint8_t palette_num);
bool checkCollide_X(int32_t d_x);
bool checkCollide_Y();

// ---------Functions complement other groups' missing APIs-----------
// int drawText(char* text, uint32_t length, uint32_t x, uint32_t y);

uint32_t getMode();

// Video Controls
int setVideoMode(uint32_t mode);
void setRefreshRate(uint32_t rate);

// Backgrounds
uint32_t merge_xy(uint32_t x, uint32_t y);
// int setBackgroundPalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB);
int backgroundDrawRec(uint32_t background_num, uint32_t xy, uint32_t wh, uint32_t colorEntry);
int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num);
int changeBackgroundPalette(uint32_t background_num, uint32_t palette_num);

// Sprites
int setSpritePalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB);
uint16_t createRecSprite(uint32_t xy, uint32_t wh, uint32_t palette_num, uint32_t colorEntry);
int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y);
int changeSpritePalette(uint32_t sprite_num, uint32_t palette_num);
uint32_t calcSmallSpriteControl_ours(uint32_t xy, uint32_t wh, uint32_t p);
uint32_t calcLargeSpriteControl_ours(uint32_t xy, uint32_t wh, uint32_t p);

//Text 
int drawText(char* text, uint32_t length, uint32_t x, uint32_t y);
void clearTextScreen();

