#include <stdint.h>
#include <stdbool.h>
#include "api.h"
#include "systemcall.h"

// define game states
#define CREATE_BLOCK_STATE 0
#define DROP_BLOCK_STATE 1
#define CHECK_FULL_LINE_STATE 2
#define GAME_OVER_STATE 3

volatile int global = 42;
volatile uint32_t controller_status = 0;
int game_state = CREATE_BLOCK_STATE;
extern int Block;
bool game_board[FULL_Y/UNIT][(FULL_X/UNIT)-2*MARGIN] = {false};
int block_nonempty_idx[4] = {0};
int block_current_x = 0;
int block_current_y = 0;

void drop_block_state(int32_t sprite_num, int *rotation);


int main()
{
    int last_global = global;
    int mode;

    // draw text
    char* greeting = "Hello!";
    drawText(greeting, 6, 0, 0);

    // -----------draw sprite-----------------
    // setSpritePalette(1, 0, 0x00000000); // Transparent
    // setSpritePalette(1, 1, 0xFFFF0000); // Red
    // setSpritePalette(1, 2, 0xFF0000FF); // Blue
    // int32_t sprite_1 = createRecSprite(0, 0, 64, 64, 1, 1);
    // int32_t sprite_2 = createRecSprite(0, 0, 16, 16, 1, 2);

    // -----------Draw grid-----------------
    setBackgroundPalette(0, 0, 0x00000000); // Transparent
    setBackgroundPalette(0, 1, 0xFFFFFFFF); // White
    setBackgroundPalette(0, 2, 0xFF000000); // Black

    backgroundDrawRec(0, 0, 0, FULL_X/2, FULL_Y, 0); // fill with tranparent first

    for(int i = 0; i < FULL_X; i += UNIT) {
        backgroundDrawRec(0, i, 0, 1, FULL_Y, 2); // vertical lines
    }
    for(int j = 0; j < FULL_Y; j += UNIT) {
        backgroundDrawRec(0, 0, j, FULL_X, 1, 2); // horizontal lines
    }

    for(int i = (MARGIN+1)*UNIT; i < FULL_X - (MARGIN*UNIT); i+=UNIT) {
        for(int j = 0; j < FULL_Y; j+=UNIT) {
            backgroundDrawRec(0, i, j, 1, 1, 1); // grid points
        }
    }
    
    backgroundDrawRec(0, MARGIN*UNIT - UNIT/2, 0, UNIT/2, FULL_Y, 1); // left boarder
    backgroundDrawRec(0, FULL_X - (MARGIN*UNIT), 0, UNIT/2, FULL_Y, 1); // right boarder

    setBackgroundControl(0, 0, 0, 5, 0); // put grid in front of blocks, blocks(large sprite) are rendered in z-plane 4
    // -----------end draw grid-------------


    // ---------Draw Blocks on large sprites---------------

    // color palatte_num = 0 for blocks
    setSpritePalette(0, S_type, 0xFFFF3333); // Red
    setSpritePalette(0, I_type, 0xFFFF9933); // Orange
    setSpritePalette(0, J_type, 0xFFFFFF33); // Yellow
    setSpritePalette(0, L_type, 0xFF99FF33); // Green
    setSpritePalette(0, O_type, 0xFF33FFFF); // Turquoise
    setSpritePalette(0, T_type, 0xFF3333FF); // Blue
    setSpritePalette(0, Z_type, 0xFF9933FF); // Green
    setSpritePalette(0, 8, 0x00000000); // Transparent

    // transparent palette_num = 1 for blocks
    setSpritePalette(1, S_type, 0x00000000);
    setSpritePalette(1, I_type, 0x00000000);
    setSpritePalette(1, J_type, 0x00000000);
    setSpritePalette(1, L_type, 0x00000000);
    setSpritePalette(1, O_type, 0x00000000);
    setSpritePalette(1, T_type, 0x00000000);
    setSpritePalette(1, Z_type, 0x00000000);
    setSpritePalette(1, 8, 0x00000000);

    int rotation = 0;
    initBlock(S_type, rotation, FULL_X/2-2*UNIT);
    initBlock(I_type, rotation, FULL_X/2-2*UNIT);
    initBlock(J_type, rotation, FULL_X/2-2*UNIT);
    initBlock(L_type, rotation, FULL_X/2-2*UNIT);
    initBlock(O_type, rotation, FULL_X/2-2*UNIT);
    initBlock(T_type, rotation, FULL_X/2-2*UNIT);
    initBlock(Z_type, rotation, FULL_X/2-2*UNIT);

    int current_block_type = 0;
    int next_block_type = 1;
    // -----------end init blocks-------------

    setVideoMode(GRAPHICS_MODE);

    while (1)
    {
        global =  getTimer();
        if(global != last_global) {

            mode = getMode();
            
            if(mode == TEXT_MODE) {
                // do something
            }
            else if (mode == GRAPHICS_MODE) {
                if(game_state == CREATE_BLOCK_STATE) {
                    // reset moving block info
                    block_current_x = (FULL_X/UNIT)/2 -2;
                    block_current_y = 0;
                    rotation = 0;

                    // random generate next block
                    current_block_type = next_block_type;
                    next_block_type = global % 7;
                    if(next_block_type == current_block_type) next_block_type++;

                    // visuallize current block
                    setBlockControl(current_block_type, FULL_X/2-2*UNIT, 0, 0);

                    // visualize next block
                    setBlockControl(next_block_type, (MARGIN/2)*UNIT, FULL_Y/3, 0);

                    // for(int k = 0; k < 4; k++) {
                    //     block_nonempty_idx[k] = Block[current_block_type][rotation][k];
                    // }

                    game_state = DROP_BLOCK_STATE;
                }
                else if(game_state == DROP_BLOCK_STATE) {
                    drop_block_state(current_block_type, &rotation);
                }
                else if(game_state == CHECK_FULL_LINE_STATE) {
                    //draw current block to background

                    // set current block to transparent
                    setBlockControl(current_block_type, FULL_X/2-2*UNIT, 0, 1);

                    //check full line
                    game_state = CREATE_BLOCK_STATE;
                }
                else if(game_state == GAME_OVER_STATE) {
                }
            }
            last_global = global;
        }
    }
    return 0;
}

void drop_block_state(int32_t block_type, int *rotation) {
    controller_status = getStatus();
    if(controller_status) {
        if (controller_status & 0x1) // left
        {
            if(checkCollide_X(block_type, *rotation, -UNIT) == 0) {
                moveSprite(block_type+128, -UNIT, 0);
                block_current_x++;
            }
        }
        if (controller_status & 0x2) // up
        {
            *rotation = (*rotation == 3)? 0 : *rotation + 1;
            rotateBlock(block_type, *rotation);
        }
        if (controller_status & 0x4) // down
        {
            if(checkCollide_Y(block_type, *rotation, UNIT) == 0) {
                moveSprite(block_type+128, 0, UNIT);
                block_current_y++;
            }
            else {
                game_state = CHECK_FULL_LINE_STATE;
            }
        }
        if (controller_status & 0x8) // right
        {
            if(checkCollide_X(block_type, *rotation, UNIT) == 0) {
                moveSprite(block_type+128, UNIT, 0);
                block_current_x++;
            }
        }
    }

    if(global % 10 == 0) {
        if(checkCollide_Y(block_type, *rotation, UNIT) == 0) {
            moveSprite(block_type+128, 0, UNIT);
            block_current_y++;
        }
        else {
            game_state = CHECK_FULL_LINE_STATE;
        }
    }

    return;
}