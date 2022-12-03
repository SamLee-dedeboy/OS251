#include <stdint.h>
#include "api.h"
#include "systemcall.h"

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
    setSpritePalette(0, 0xFFFF0000); // Red
    setSpritePalette(1, 0xFF0000FF); // Blue
    uint32_t sprite_1 = createSprite(0, 0, 40, 20, 0);
    uint32_t sprite_2 = createSprite(0, 20, 20, 40, 1);

    while (1)
    {
        global =  getTimer();

        if(global != last_global) {

            mode = getMode();
            
            if(mode == TEXT_MODE) {
                // do something
            }
            else if (mode == GRAPHICS_MODE) {
                // do something
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