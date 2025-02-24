#include <stdint.h>
#include "api.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;


int main()
{
    int last_global = global;
    int mode;
    // draw text
    char* greeting = "Hello!";
    drawText(greeting, 6, 0, 0);

    // draw sprite
    setSpritePalette(0, 0, 0x00000000); // Transparent
    setSpritePalette(0, 1, 0xFFFF0000); // Red
    setSpritePalette(0, 2, 0xFF0000FF); // Blue
    int32_t sprite_1 = createRecSprite(merge_arg(0, 0), merge_arg(40, 20), 0, 1);
    int32_t sprite_2 = createRecSprite(merge_arg(0, 20), merge_arg(20, 40), 0, 2);

    setBackgroundPalette(0, 0, 0x00000000); // Transparent
    setBackgroundPalette(0, 1, 0xFFFFFF00); // Yellow
    backgroundDrawRec(0, merge_arg(FULL_X/2, FULL_Y/2), merge_arg(50, 50), 1);
    setBackgroundControl(0, merge_arg(0, 0), 0, 0);
    setVideoMode(GRAPHICS_MODE);

    while (1)
    {
        global =  getTimer();
        controller_status = getStatus();
        if(global != last_global) {

            mode = getMode();
            
            if(mode == TEXT_MODE) {
                // do something
            }
            else if (mode == GRAPHICS_MODE) {
                // do something
                if(controller_status) {
                    if (controller_status & 0x1) // left
                    {
                        moveSprite(sprite_1, -1, 0);
                    }
                    if (controller_status & 0x2) // up
                    {
                        moveSprite(sprite_2, 0, -1);
                    }
                    if (controller_status & 0x4) // down
                    {
                        moveSprite(sprite_2, 0, 1);
                    }
                    if (controller_status & 0x8) // right
                    {
                        moveSprite(sprite_1, 1, 0);
                    }
                }
            }
            last_global = global;
        }

        if(global % 15 == 0) {
            if (mode == TEXT_MODE) {
                setVideoMode(GRAPHICS_MODE);
            }
            else {
                setVideoMode(TEXT_MODE);
            }
        }
        
    }
    return 0;
}