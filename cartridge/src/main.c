#include <stdint.h>

typedef uint32_t *TContext;
typedef void (*TEntry)(void *);

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile uint32_t controller_status = 0;

#define SYSTIMER 0x00000001
#define CONTROLLER_STATUS 0x00000002
#define MODE_STATUS 0x00000003
#define SMALL_SPRITE_DROP 0x00000004

uint32_t getTicks(void);
uint32_t getStatus(void);
uint32_t getMode(void);
uint32_t initThread(void);
uint32_t spritedown(void);
uint32_t systemcall(uint32_t funName);

TContext newThread;
int mode = 0; // 0 = text mode, 1 = graphics mode

int main()
{
    newThread = initThread();

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
        global = systemcall(SYSTIMER);
        // global = getTicks();
        if (global != last_global)
        {
            mode = systemcall(MODE_STATUS);
            // mode = getMode();
            if (mode == 1)
            {
                // TODO: add response to controller status & bound checks
                // ctr_bits = 000 1111 1111 000010000 00 0001 0000 00
                // dx = 000 0000 0000 00000 0001 0000 0000 0000
                systemcall(SMALL_SPRITE_DROP);
                // continue;
                // (*((volatile uint32_t *)0x500FF214 + 0)) += 0x00001000;
            }

            controller_status = systemcall(CONTROLLER_STATUS); // getStatus();
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
