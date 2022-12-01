#include <stdint.h>
#include "api.h"
#include "Systemcall.h"

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile uint32_t controller_status = 0;
TContext newThread;
int mode = 0; // 0 = text mode, 1 = graphics mode
int block_created = 0; // 0: not created yet, 1: already created.
uint32_t rand;
int last_update = 42;
int rotate_counter = 0;
uint32_t *current_blocks;
int block_length = 30;
int b_pos_x = 0;
int b_pos_y = 0;
int b_offset_x = 151;
int b_offset_y = 9;
int down_block[9][7];

void reset_block() {
    if (rand==0) {
        if (rotate_counter%4==0 || rotate_counter%4==2) {
            if (b_pos_y+2 >= 9) {
                // clearBlock();
                clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, current_blocks, rotate_counter);
                block_created = 0;
                b_pos_x = 0;
                b_pos_y = 0;
                rotate_counter = 0;
                if (rotate_counter%4==0){
                    down_block[b_pos_x][b_pos_y] = 1;
                    down_block[b_pos_x+1][b_pos_y] = 1;
                    down_block[b_pos_x+1][b_pos_y+1] = 1;
                    down_block[b_pos_x+1][b_pos_y+2] = 1;
                }
                else {
                    down_block[b_pos_x][b_pos_y] = 1;
                    down_block[b_pos_x+1][b_pos_y] = 1;
                    down_block[b_pos_x+2][b_pos_y] = 1;
                    down_block[b_pos_x+2][b_pos_y+1] = 1;
                }
            }
        }
        else if (rotate_counter%4==1 || rotate_counter%4==3) {
            if (b_pos_y+3 >= 9) {
                // clearBlock();
                clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, current_blocks, rotate_counter);
                block_created = 0;
                b_pos_x = 0;
                b_pos_y = 0;
                rotate_counter = 0;
                if (rotate_counter%4==1){
                    down_block[b_pos_x][b_pos_y] = 1;
                    down_block[b_pos_x+1][b_pos_y] = 1;
                    down_block[b_pos_x][b_pos_y+1] = 1;
                    down_block[b_pos_x][b_pos_y+2] = 1;
                }
                else {
                    down_block[b_pos_x+1][b_pos_y] = 1;
                    down_block[b_pos_x+1][b_pos_y+1] = 1;
                    down_block[b_pos_x+1][b_pos_y+2] = 1;
                    down_block[b_pos_x][b_pos_y+2] = 1;
                }
            }
        }
    }
}

int main()
{
    // newThread = initThread();

    // Sprite Memory Data
    // for (int sp_index = 0; sp_index < 128; sp_index++)
    // {
    //     volatile char *SM_SPRITE_MEMORY_i = (volatile uint8_t *)(0x500F4000 + sp_index * 0x100);
    //     for (int j = 0; j < 256; j++)
    //     {
    //         SM_SPRITE_MEMORY_i[j] = 255;
    //     }
    // }

    int a = 4;
    int b = 12;
    int last_global = 42;
    int x_pos = 12;

    for (int i=0; i<9; i++) {
        for (int j=0; j<9; j++)
            down_block[i][j] = 0;
    }

    VIDEO_MEMORY[0] = 'h';
    VIDEO_MEMORY[1] = 'e';
    VIDEO_MEMORY[2] = 'l';
    VIDEO_MEMORY[3] = 'l';
    VIDEO_MEMORY[4] = 'o';
    VIDEO_MEMORY[5] = ' ';
    VIDEO_MEMORY[6] = 'W';
    VIDEO_MEMORY[7] = 'o';
    VIDEO_MEMORY[8] = 'r';
    VIDEO_MEMORY[9] = 'l';
    VIDEO_MEMORY[10] = 'd';
    VIDEO_MEMORY[11] = '!';
    VIDEO_MEMORY[12] = 'X';
    (*INT_ENABLE_REG) = 0x7;

    while (1)
    {
        global =  systemcall(SYSTIMER); // Todo: Can not use getTimer()
        controller_status = getStatus();
        if (global != last_global)
        {
            mode = getMode();
            if (mode == 1)
            {
                // spriteDrop();

                setSpritePalette(0, 0xFFFF0000); // Red
                setSpritePalette(1, 0xFF0000FF); // Blue
                setSpritePalette(2, 0xFF555555); // gray for dropped down blocks.
                // uint32_t block_1     = createBlock(100, 200, 10, 10, 0, 0);
                // uint32_t sprite_5 = createSprite(100, 200, 10, 60, 1);

                if (block_created==0) {
                    // TODO: rand = random();
                    rand = 0;
                    current_blocks = createBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand);
                    block_created = 1;
                    last_update = global;
                }
                else {
                    if ((global-last_update)>=100) {
                        // TODO: add hit bottom detection.
                        b_pos_y += 1;
                        dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, current_blocks, rotate_counter);
                        last_update = global;
                        reset_block();
                    }
                    if (controller_status & 0x1) {
                        // left
                        if (b_pos_x >= 1)   b_pos_x -= 1;
                        dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, current_blocks, rotate_counter);
                    }
                    else if (controller_status & 0x8) {
                        // right
                        if (rand==0) {
                            if (rotate_counter%4==0 || rotate_counter%4==2) {
                                if (b_pos_x+3 < 7)   b_pos_x += 1;
                            }
                            else {
                                if (b_pos_x+2 < 7)   b_pos_x += 1;
                            }
                        }
                        // if (rand==0)
                        //     if (b_pos_x < 510-3*block_length) b_pos_x += 1;
                        dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, current_blocks, rotate_counter);
                    }
                    else if (controller_status & 0x2) {
                        // TODO: up (rotate)
                        rotate_counter += 1;
                        dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, current_blocks, rotate_counter);
                        if (rotate_counter == 4)    rotate_counter = 0;
                    }
                    else if (controller_status & 0x4) {
                        // TODO: down (hard drop)
                        // put down block to background
                        // reset block_created, b_pos_x, b_pos_y
                        // check if a line has been filled.
                        
                    }
                    
                }
            }

            else if (controller_status)
            {
                VIDEO_MEMORY[x_pos] = ' ';
                if (controller_status & 0x1)
                {
                    if (x_pos & 0x3F)
                    {
                        x_pos--;
                    }
                }
                if (controller_status & 0x2)
                {
                    if (x_pos >= 0x40)
                    {
                        x_pos -= 0x40;
                    }
                }
                if (controller_status & 0x4)
                {
                    if (x_pos < 0x8C0)
                    {
                        x_pos += 0x40;
                    }
                }
                if (controller_status & 0x8)
                {
                    if ((x_pos & 0x3F) != 0x3F)
                    {
                        x_pos++;
                    }
                }
                VIDEO_MEMORY[x_pos] = 'X';
            }

            last_global = global;
        }
    }
    return 0;
}