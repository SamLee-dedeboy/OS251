#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef GRAPHIC_H
#define GRAPHIC_H
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
#define TEXT_DATA_ADDRESS 0x500FE800            // data size: char
#define MODE_CONTROL_REGISTER 0x500FF414
#define MODE_CONTROL (*((volatile uint32_t *)0x500FF414))

// ---------Functions for tetris game-----------
void welcome_page_state();
void init_game_state(int *rotation);
void drop_block_state(int32_t sprite_num, int *rotation);
void delete_full_line_state();
uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x);
int initDigit(int digit_type, int32_t x, int32_t y);
int initTextRect(int rect_id, int32_t x, int32_t y);
void rotateBlock(uint8_t block_type, uint8_t rotation);
bool checkCollide_X(int32_t d_x);
bool checkCollide_Y();

// ---------Functions complement group_2's missing APIs-----------
uint32_t getMode();

// Video Controls
void setRefreshRate(uint32_t rate);

// Backgrounds
uint32_t merge_xy(uint32_t x, uint32_t y);
int backgroundDrawRec(uint32_t background_num, uint32_t xy, uint32_t wh, uint32_t colorEntry);

// Sprites
int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y);

//Text 
int drawText(char* text, uint32_t length, uint32_t x, uint32_t y);

// replaced by group_2 APIs.
// group_2: setLargeSpriteControl(block_type, calcLargeSpriteControl(x, y, BLOCK_SIZE, BLOCK_SIZE, palette_num)) (group_2 API)
// ours:    setDigitControl, void setBlockControl(uint8_t block_type, int32_t x, int32_t y, uint8_t palette_num);
// group_2: setTextMode(), setGraphicsMode()
// ours:    int setVideoMode(uint32_t mode)
// group_2: setBackgroundColor
// ours:    setBackgroundPalette
// group_2: setBackgroundSpriteControl(background_num, calcBackgroundControl(x,y,z,p))
// ours:    int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num)
// group_2: setColor
// ours:    setSpritePalette
// group_2: calcSmallSpriteControl
// ours:    calcSmallSpriteControl
// group_2: calcLargeSpriteControl
// ours:    calcLargeSpriteControl


