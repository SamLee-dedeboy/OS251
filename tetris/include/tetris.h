#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef GRAPHIC_H
#define GRAPHIC_H
#include "graphic.h"
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
#define INIT_GAME_STATE 0
#define CREATE_BLOCK_STATE 1
#define DROP_BLOCK_STATE 2
#define CHECK_FULL_LINE_STATE 3
#define GAME_OVER_STATE 4

// ---------Functions for tetris game-----------
void init_game_state(int *rotation);
void drop_block_state(int32_t sprite_num, int *rotation);
uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x);
void rotateBlock(uint8_t block_type, uint8_t rotation);
void setBlockControl(uint8_t block_type, int32_t x, int32_t y, uint8_t palette_num);
bool checkCollide_X(int32_t d_x);
bool checkCollide_Y();