#ifndef GRAPHIC_H
#define GRAPHIC_H 

#include <stdint.h>

#define TEXT_BUFF_SIZE               2304
#define IMAGE_BUFF_SIZE              147456 
#define FULL_X                       512
#define FULL_Y                       288
#define TOP                          4
#define BOTTOM                       0


uint32_t draw_rec(uint32_t x, uint32_t y, uint32_t e_x, uint32_t e_y, uint32_t* RGB);
uint32_t moveImage(uint32_t image, uint32_t x, uint32_t y, uint32_t z);
uint32_t setImagePalette(uint32_t image, uint32_t palette);
uint32_t setPalette(uint32_t background, uint32_t* buff);
uint32_t setText(char * buff, uint32_t size);
uint32_t setRefreshRate(uint32_t rate);
uint32_t imageFill(uint32_t background, uint8_t* buff, uint32_t size);
uint32_t clearTextScreen();


#endif