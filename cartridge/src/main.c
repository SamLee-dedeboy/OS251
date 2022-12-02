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
int down_block[9][7] = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0}
};
int pressed = 0;

void line_elimination() {
    int line_sum = 0;
    int line_target[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int need_redraw = 0;

    for (int i=0; i<9; i++){
        line_sum = 0;
        for (int j=0; j<7; j++){
            if (down_block[i][j]==1) line_sum += 1;
        }
        if (line_sum==7) {
            line_target[i] = 1;
            b_pos_y += 1;
            need_redraw = 1;
        }
    }
    for (int i=8; i>=0; i--){
        if (line_target[i]==1) {
            for (int j=i; j>0; j--) {
                for (int k=0; k<7; k++)
                    down_block[j][k] = down_block[j-1][k];
            }
        }
    }
    if (need_redraw==1) redraw_dropped(down_block, block_length, b_offset_x, b_offset_y, 2);
}

void reset_block() {
    if (rand==0) {
        if (rotate_counter%4==0 || rotate_counter%4==2) {
            if (rotate_counter%4==0) {
                if (down_block[b_pos_y+2][b_pos_x]==1 || down_block[b_pos_y+2][b_pos_x+1]==1 || down_block[b_pos_y+2][b_pos_x+2]==1){
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x] = 1;
                    down_block[b_pos_y+1][b_pos_x] = 1;
                    down_block[b_pos_y+1][b_pos_x+1] = 1;
                    down_block[b_pos_y+1][b_pos_x+2] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
                }
                else if (b_pos_y+2 >= 9) {
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x] = 1;
                    down_block[b_pos_y+1][b_pos_x] = 1;
                    down_block[b_pos_y+1][b_pos_x+1] = 1;
                    down_block[b_pos_y+1][b_pos_x+2] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
                }
            }
            else {
                if (down_block[b_pos_y+2][b_pos_x+2]==1 || down_block[b_pos_y+1][b_pos_x]==1 || down_block[b_pos_y+1][b_pos_x+1]==1){
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x] = 1;
                    down_block[b_pos_y][b_pos_x+1] = 1;
                    down_block[b_pos_y][b_pos_x+2] = 1;
                    down_block[b_pos_y+1][b_pos_x+2] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
                }
                else if (b_pos_y+2 >= 9) {
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x] = 1;
                    down_block[b_pos_y][b_pos_x+1] = 1;
                    down_block[b_pos_y][b_pos_x+2] = 1;
                    down_block[b_pos_y+1][b_pos_x+2] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
                }
            }
        }
        else {
            if (rotate_counter%4==1) {
                if (down_block[b_pos_y+3][b_pos_x]==1 || down_block[b_pos_y+1][b_pos_x+1]==1){
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x] = 1;
                    down_block[b_pos_y][b_pos_x+1] = 1;
                    down_block[b_pos_y+1][b_pos_x] = 1;
                    down_block[b_pos_y+2][b_pos_x] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
                }
                else if (b_pos_y+3 >= 9) {
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x] = 1;
                    down_block[b_pos_y][b_pos_x+1] = 1;
                    down_block[b_pos_y+1][b_pos_x] = 1;
                    down_block[b_pos_y+2][b_pos_x] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
                }
            }
            else {
                if (down_block[b_pos_y+3][b_pos_x]==1 || down_block[b_pos_y+3][b_pos_x+1]==1){
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x+1] = 1;
                    down_block[b_pos_y+1][b_pos_x+1] = 1;
                    down_block[b_pos_y+2][b_pos_x+1] = 1;
                    down_block[b_pos_y+2][b_pos_x] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
                }
                else if (b_pos_y+3 >= 9) {
                    clearBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 2, rand, rotate_counter);
                    down_block[b_pos_y][b_pos_x+1] = 1;
                    down_block[b_pos_y+1][b_pos_x+1] = 1;
                    down_block[b_pos_y+2][b_pos_x+1] = 1;
                    down_block[b_pos_y+2][b_pos_x] = 1;
                    line_elimination();
                    if (b_pos_y<2) {
                        setVideoMode(0);
                        block_created = -1; // game over
                    }
                    
                    block_created = 0;
                    b_pos_x = 0;
                    b_pos_y = 0;
                    rotate_counter = 0;
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
                setSpritePalette(3, 0xFF000000); // reset to black.
                // uint32_t block_1     = createBlock(100, 200, 10, 10, 0, 0);
                // uint32_t sprite_5 = createSprite(100, 200, 10, 60, 1);

                if (block_created==0) {
                    // TODO: rand = random();
                    rand = 0;
                    current_blocks = createBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand);
                    block_created = 1;
                    last_update = global;
                }
                else if (block_created==1) {
                    if ((global-last_update)>=15) {
                        // TODO: add hit bottom detection.
                        reset_block();
                        b_pos_y += 1;
                        dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, rotate_counter);
                        last_update = global;
                    }
                    if ((controller_status & 0x1) && pressed == 0) {
                        pressed = 1;
                        // left
                        if (rand==0) {
                            if (rotate_counter%4==3 && down_block[b_pos_y+2][b_pos_x-1]==1);
                            else if ((rotate_counter%4==0 || rotate_counter%4==1 ||rotate_counter%4==2) && down_block[b_pos_y][b_pos_x-1]==1);
                            else if ((rotate_counter%4==0 || rotate_counter%4==1) && down_block[b_pos_y+1][b_pos_x-1]==1);
                            else if (rotate_counter%4==1 && down_block[b_pos_y+2][b_pos_x-1]==1);
                            else if (b_pos_x >= 1)   {
                                b_pos_x -= 1;
                                dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, rotate_counter);
                            }
                        }
                    }
                    else if ((controller_status & 0x8) && pressed == 0) {
                        pressed = 1;
                        // right
                        if (rand==0) {
                            if (rotate_counter%4==0 || rotate_counter%4==2) {
                                if (down_block[b_pos_y+1][b_pos_x+3]==1);
                                else if (rotate_counter%4==2 && down_block[b_pos_y][b_pos_x+3]==1);
                                else if (b_pos_x+3 < 7)   {
                                    b_pos_x += 1;
                                    dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, rotate_counter);
                                }
                            }
                            else {
                                if (down_block[b_pos_y][b_pos_x+2]==1);
                                else if (rotate_counter%4==3 && (down_block[b_pos_y+1][b_pos_x+2]==1 || down_block[b_pos_y+2][b_pos_x+2]==1));
                                else if (b_pos_x+2 < 7)   {
                                    b_pos_x += 1;
                                    dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, rotate_counter);
                                }
                            }
                        }
                        // if (rand==0)
                        //     if (b_pos_x < 510-3*block_length) b_pos_x += 1;
                        // dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, rotate_counter);
                    }
                    else if ((controller_status & 0x2) && pressed == 0) {
                        pressed = 1;
                        // TODO: up (rotate)
                        if (rand==0){
                            if (down_block[b_pos_y+2][b_pos_x]==1 && rotate_counter%4==0);
                            else if ((down_block[b_pos_y+2][b_pos_x]==1 || down_block[b_pos_y+2][b_pos_x+1]==1) && rotate_counter%4==2);
                            else {
                                rotate_counter += 1;
                                dropBlock(b_pos_x*block_length+b_offset_x, b_pos_y*block_length+b_offset_y, block_length, block_length, 0, rand, rotate_counter);
                                if (rotate_counter == 4)    rotate_counter = 0;
                            }
                        }
                    }
                    else if (controller_status & 0x4) {
                        // TODO: down (hard drop)
                        // put down block to background
                        // reset block_created, b_pos_x, b_pos_y
                        // check if a line has been filled.
                        
                    } else {
                        pressed = 0;
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