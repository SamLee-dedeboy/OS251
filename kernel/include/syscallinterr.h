#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef SYSTEMCALLANDINTERRUPT_H
#define SYSTEMCALLANDINTERRUPT_H

// --------------System call function identifiers--------------
#define SYSTIMER 0x00000001
#define CONTROLLER_STATUS 0x00000002
#define MODE_STATUS 0x00000003
#define SMALL_SPRITE_DROP 0x00000004
#define READ_MACHINE_TIME 0x00000005
#define READ_MACHINE_PERIOD 0x00000006
#define READ_INT_PENDING_REG 0x00000007
#define RAND 0x00000008
#define SYSVIDEO 0x00000009
#define ENABLE_CMD 0x0000000FE
#define DISABLE_CMD 0x0000000FF


// --------------Graphic function identifiers--------------
#define get_LARGE_SPRITE_DATA_ADDRESS 0x0000000A    
#define get_SMALL_SPRITE_DATA_ADDRESS 0x0000000B    
#define get_BACKGROUND_PALLETE_ADDRESS 0x0000000C   
#define get_SPRITE_PALLETE_ADDRESS 0x0000000D    
#define get_TEXT_DATA_ADDRESS 0x0000000E            
#define get_BACKGROUND_CONTROL_ADDRESS 0x0000000F   
#define get_LARGE_SPRITE_CONTROL_ADDRESS 0x00000010 
#define get_SMALL_SPRITE_CONTROL_ADDRESS 0x00000011 
#define get_MODE_CONTROL_REGISTER 0x00000012
#define get_BACKGROUND_DATA_ADDRESS 0x00000013      

// Video Controls
#define VideoMode 0x00000020
#define RefreshRate 0x00000021

//Background
#define Merge 0x00000022
#define setBackgroundPalette_id 0x00000023
#define backgroundDrawRec_id 0x00000024
#define setBackgroundControl_id 0x00000025
#define changeBackgroundPalette_id 0x00000026

// Sprites
#define setSpritePalette_id 0x00000027
#define createRecSprite_id 0x00000028
#define moveSprite_id 0x00000029
#define changeSpritePalette_id 0x0000002A
#define calcSmallSpriteControl_id 0x0000002B
#define calcLargeSpriteControl_id 0x0000002C

//Text
#define drawText_id 0x0000002D
#define clearTextScreen_id 0x0000002E

// --------------Thread function identifiers--------------
#define THREAD_INITIALLIZE 0x00000017
#define WRITE_TEXT 0x00000014


// Video memory addresses
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

// c interrupt
void timer_interrupt();
void cmd_interrupt();
void video_interrupt();
void cart_interrupt();
void illegal_inst_interrupt();

#endif