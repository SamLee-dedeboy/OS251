#include <stdint.h>
#include "api.h"
#include "systemcall.h"
#include "graphic.h"

volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile uint32_t controller_status = 0;

int main()
{
    while (1)
    {
        setVideoMode(GRAPHICS_MODE);
        setSpritePalette(0, 0xFFFF0000);
        setSpritePalette(1, 0xFF0000FF);
        uint32_t sprite_1 = createSprite(200, 100, 64, 10, 0);
        uint32_t sprite_2 = createSprite(100, 200, 10, 64, 1);

        changeSpriteColor(sprite_2, 0);
    }
    return 0;
}
