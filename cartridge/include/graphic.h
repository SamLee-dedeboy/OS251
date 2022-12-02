#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef GRAPHIC_H
#define GRAPHIC_H 

#define TEXT_BUFF_SIZE               2304
#define IMAGE_BUFF_SIZE              147456 
#define FULL_X                       512
#define FULL_Y                       288
#define TOP                          4
#define BOTTOM                       0
#define TEXT_MODE                    0
#define GRAPHICS_MODE                1

void setVideoMode(int mode);
void setSpritePalette(uint32_t num, uint32_t ARGB);
uint32_t createSprite(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num);
void changeSpriteColor(uint32_t sprite_num, uint32_t color_num);
void moveSprite(int sprite_num, int d_x, int d_y);
uint32_t CalcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t CalcSmallSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t * createBlock(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num, int32_t block_id);
void dropBlock(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num, int32_t block_id, uint32_t *current_blocks, int rotate_id);
void clearBlock(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num, int32_t block_id, uint32_t *current_blocks, int rotate_id);

// uint32_t draw_rec(uint32_t x, uint32_t y, uint32_t e_x, uint32_t e_y, uint32_t* RGB);
// uint32_t moveImage(uint32_t image, uint32_t x, uint32_t y, uint32_t z);
// uint32_t setImagePalette(uint32_t image, uint32_t palette);
// uint32_t setPalette(uint32_t background, uint32_t* buff);
// uint32_t setText(char * buff, uint32_t size);
// uint32_t setRefreshRate(uint32_t rate);
// uint32_t imageFill(uint32_t background, uint8_t* buff, uint32_t size);
// uint32_t clearTextScreen();


#endif