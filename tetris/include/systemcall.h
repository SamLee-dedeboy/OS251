#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H


#define SYSTIMER 0x00000001
#define CONTROLLER_STATUS 0x00000002
#define MODE_STATUS 0x00000003
#define SMALL_SPRITE_DROP 0x00000004
#define READ_MACHINE_TIME 0x00000005
#define READ_MACHINE_PERIOD 0x00000006
#define READ_INT_PENDING 0x00000007
#define RAND 0x00000008
#define SYSIDEO 0x00000009

#define THREAD_INITIALLIZE 0x00000017
#define WRITE_TEXT 0x00000014

// get addresses.
#define get_BACKGROUND_DATA_ADDRESS 0x00000013      
#define get_LARGE_SPRITE_DATA_ADDRESS 0x0000000A    
#define get_SMALL_SPRITE_DATA_ADDRESS 0x0000000B    
#define get_BACKGROUND_PALLETE_ADDRESS 0x0000000C   
#define get_SPRITE_PALLETE_ADDRESS 0x0000000D    
#define get_TEXT_DATA_ADDRESS 0x0000000E            
#define get_BACKGROUND_CONTROL_ADDRESS 0x0000000F   
#define get_LARGE_SPRITE_CONTROL_ADDRESS 0x00000010 
#define get_SMALL_SPRITE_CONTROL_ADDRESS 0x00000011 
#define get_MODE_CONTROL_REGISTER 0x00000012

// Video Controls
#define VideoMode 0x00000020
#define RefreshRate 0x00000021

// Backgrounds
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

#endif