#include <stdint.h>
#include "api.h"
#include "Systemcall.h"

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile uint32_t controller_status = 0;
TContext MainThread;
TContext OtherThread;

int mode = 0; // 0 = text mode, 1 = graphics mode

uint32_t Thread(void *param)
{
    int last_global = 0;

    while (1)
    {
        if (last_global != global)
        {
            SwitchContext(Thread_SWITCH, &OtherThread, MainThread);
            last_global = global;
        }
    }
}

int main()
{
    OtherThread = thread_init(Thread_INIT, Thread, (void *)0);

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
        global = systemcall(SYSTIMER); // Todo: Can not use getTimer()
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
