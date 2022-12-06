#include "video_api.h"

extern uint8_t runner_palette[4*8];
extern uint8_t desert_background_palette[4*16];
extern uint8_t runner_0[64*64];
extern uint8_t runner_1[64*64];
extern uint8_t runner_2[64*64];
extern uint8_t bird_background_img[288*512];
extern uint8_t back_desert[288*512];
extern uint8_t sky_img[288*512];



void initVideoSetting()
{
    setDisplayMode(DISPLAY_MODE_GRAPHICS);
    backgroundPaletteMemory(0, &desert_background_palette[0], 4*100);
    spritePaletteMemory(1, &runner_palette[0], 4*254);
    spritePaletteMemory(2, &runner_palette[0], 4*254);
    transparentSpritePalette(0);
    setBackgroundDataImage(0, &back_desert[0]);

    controlBackground(0, 0, 0, 0, 0);
    controlLargeSprite(0, 64, 64, 30, 30);
    controlLargeSprite(1, 64, 64, 0, 0);
    controlLargeSprite(2, 64, 64, 0, 0);
    
    setLargeSpriteDataImage(0, &runner_0[0]);
    setLargeSpriteDataImage(1, &runner_1[0]);
    setLargeSpriteDataImage(2, &runner_2[0]);

}