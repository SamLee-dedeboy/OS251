#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef GRAPHIC_H
#define GRAPHIC_H
#include "graphic.h"
#endif

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

// ---------Functions for tetris game-----------
uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x);
void rotateBlock(uint8_t block_type, uint8_t rotation);
void setBlockControl(uint8_t block_type, int32_t x, int32_t y, uint8_t palette_num);
bool checkCollide_X(int32_t d_x);
bool checkCollide_Y();

// int checkCollide_X(uint8_t block_type, uint8_t rotation, int32_t d_x);
// int checkCollide_Y(uint8_t block_type, uint8_t rotation, int32_t d_y);
// int moveBlock(uint16_t sprite_num, int32_t d_x, int32_t d_y); // add border conditions