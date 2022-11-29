#include <stdint.h>

volatile int global = 42;
volatile uint32_t controller_status = 0;

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);

volatile uint32_t *SPRITE_PALLETE = (volatile uint32_t *)(0x500FD000);
volatile uint32_t *LARGE_SPRITE_CONTROL = (volatile uint32_t *)(0x500FF114);
volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500FF414);
volatile uint8_t *LARGE_SPRITE_DATA = (volatile uint8_t *)(0x500B4000);
uint32_t CalcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p);

int main() {
    int a = 4;
    int b = 12;
    int last_global = 42;
    int x_pos = 12;
    int hold_count = 0;

    VIDEO_MEMORY[0] = 'H';
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
    for(int y = 0; y < 64; y++){
        for(int x = 0; x < 64; x++){
            LARGE_SPRITE_DATA[(y<<6) + x] = (32 - y)*(32 - y) + (32 - x)*(32 - x) <= 900  ? 0 : 1;
        }
    }

    SPRITE_PALLETE[0] = 0xFFFFA500;
    LARGE_SPRITE_CONTROL[0] = CalcLargeSpriteControl(0,0,64,64,0);
    *VIDEO_MODE = 1;

    while (1) {
        int c = a + b + global;
        if(global != last_global){
            if(controller_status){
                VIDEO_MEMORY[x_pos] = ' ';
                if(controller_status & 0x1){
                    if(x_pos & 0x3F){
                        x_pos--;
                    }
                }
                if(controller_status & 0x2){
                    if(x_pos >= 0x40){
                        x_pos -= 0x40;
                    }
                }
                if(controller_status & 0x4){
                    if(x_pos < 0x8C0){
                        x_pos += 0x40;
                    }
                }
                if(controller_status & 0x8){
                    if((x_pos & 0x3F) != 0x3F){
                        x_pos++;
                    }
                }
                if(controller_status & 0x10){
                    SPRITE_PALLETE[0] += 0x10;
                }
                if(controller_status & 0x20){
                    SPRITE_PALLETE[0] += 0x1000;
                }
                if(controller_status & 0x40){
                    SPRITE_PALLETE[0] += 0x100000;
                }
                if(controller_status & 0x80){
                    hold_count++;
                    if(hold_count > 10){
                        *VIDEO_MODE ^= 1;
                        hold_count = 0;
                    }
                }
                else{
                    hold_count = 0;
                }
                SPRITE_PALLETE[0] |= 0xFF000000;
                VIDEO_MEMORY[x_pos] = 'X';
                LARGE_SPRITE_CONTROL[0] = CalcLargeSpriteControl((x_pos & 0x3F)<<3,(x_pos>>3) & 0x1F8,64,64,0);
            }
            last_global = global;
        }
    }
    return 0;
}

uint32_t CalcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p){
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | p;
}