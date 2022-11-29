#include <stdint.h>
#include "api.h"
#include "Systemcall.h"

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile uint32_t controller_status = 0;
TContext newThread;
int mode = 0; // 0 = text mode, 1 = graphics mode

int main()
{
    // newThread = initThread();

    // Sprite Memory Data
    for (int sp_index = 0; sp_index < 128; sp_index++)
    {
        volatile char *SM_SPRITE_MEMORY_i = (volatile uint8_t *)(0x500F4000 + sp_index * 0x100);
        for (int j = 0; j < 256; j++)
        {
            SM_SPRITE_MEMORY_i[j] = 255;
        }
    }

    int a = 4;
    int b = 12;
    int last_global = 42;
    int x_pos = 12; // for text
    int init_pos = 0; // for graphics
    int mode = 0;
    int counter = 0;
    int width = 10;
    int height = 10;
    int color = 1;


    MODE_CONTROL[0] = 0x01; //LSB
    MODE_CONTROL[1] = 0x00;
    MODE_CONTROL[2] = 0x00;
    MODE_CONTROL[3] = 0x00;

    PALETTE_DATA[0] = 0x00; 
    PALETTE_DATA[1] = 0x00;  
    PALETTE_DATA[2] = 0x00; 
    PALETTE_DATA[3] = 0xFF; //black 

    PALETTE_DATA[4] = 0x00; // B
    PALETTE_DATA[5] = 0x00; // G
    PALETTE_DATA[6] = 0xFF; // R
    PALETTE_DATA[7] = 0xFF; // A //red

    PALETTE_DATA[8] = 0xFF; // B
    PALETTE_DATA[9] = 0x00; // G
    PALETTE_DATA[10] = 0x00; // R
    PALETTE_DATA[11] = 0xFF; // A //blue

    // We can set a palette all zero as a clear function by switching palette number. (or creating animation, pre-draw frames.)
    GRAPHIC_CONTROL[0] = 0x00;
    GRAPHIC_CONTROL[1] = 0x08;
    GRAPHIC_CONTROL[2] = 0x12;
    GRAPHIC_CONTROL[3] = 0x00; //0000 0000 00 01 0010 0000 1000 0000 00 00

    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            GRAPHIC_DATA[init_pos+j + 512*i] = color;
        }
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
        if (global != last_global)
        {
            mode = getMode();
            if (mode == 1)
            {
                spriteDrop();
            }

            controller_status = getStatus();
            if (controller_status)
            {
                if (mode == 0)
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
            }
            last_global = global;
        }
    }
    return 0;
}

