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

#define BACKGROUND_DATA_ADDRESS 0x50000000      // data size: uint8_t
#define LARGE_SPRITE_DATA_ADDRESS 0x500B4000    // data size: uint8_t
#define SMALL_SPRITE_DATA_ADDRESS 0x500F4000    // data size: uint8_t
#define BACKGROUND_PALLETE_ADDRESS 0x500FC000   // data size: uint32_t
#define SPRITE_PALLETE_ADDRESS 0x500FD000       // data size: uint32_t
#define TEXT_DATA_ADDRESS 0x500FE800            // data size: char
#define BACKGROUND_CONTROL_ADDRESS 0x500FF100   // data size: uint32_t
#define LARGE_SPRITE_CONTROL_ADDRESS 0x500FF114 // data size: uint32_t
#define SMALL_SPRITE_CONTROL_ADDRESS 0x500FF214 // data size: uint32_t
#define MODE_CONTROL_REGISTER 0x500FF414

#endif