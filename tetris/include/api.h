#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef API_H_
#define API_H_
#include "graphic.h"
#include "systemcall.h"

// define sys call func parameters
#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))
#define CONTROLLER (*((volatile uint32_t *)0x40000018))

uint32_t printtext(uint32_t funName, char *text, int variable);
typedef uint32_t (*TContextEntry)(void *param);
uint32_t thread_init(uint32_t funName, TContextEntry entry, void *param);
uint32_t systemcall(uint32_t funName, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5, uint32_t param6);

uint32_t getTimer();
uint32_t getStatus();
uint32_t getMode();
uint32_t spriteDrop();
uint32_t Thread(void *param);

// get memory addresses.
uint32_t getBACKGROUND_DATA_ADDRESS(uint32_t background_num); 
uint32_t getLARGE_SPRITE_DATA_ADDRESS(uint32_t large_sprite_count); 
uint32_t getSMALL_SPRITE_DATA_ADDRESS(uint32_t small_sprite_count); 
uint32_t getBACKGROUND_PALLETE_ADDRESS(uint32_t palette_num_test, uint32_t entry_num); 
uint32_t getSPRITE_PALLETE_ADDRESS(uint32_t palette_num_test, uint32_t entry_num); 
uint32_t getTEXT_DATA_ADDRESS(); 
uint32_t getBACKGROUND_CONTROL_ADDRESS(uint32_t background_num); 
uint32_t getLARGE_SPRITE_CONTROL_ADDRESS(uint32_t large_sprite_count);
uint32_t getSMALL_SPRITE_CONTROL_ADDRESS(uint32_t small_sprite_count);
uint32_t getMODE_CONTROL_REGISTER(); 

#endif