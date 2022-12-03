#include "graphic.h"



// num of sprites: 0~127: small sprite; 128~191: large sprite
int small_sprite_count = 0; // max: 128 small sprites
int large_sprite_count = 0; // max: 64 large sprites
int down_sprite_count = 0; 


void setVideoMode(int mode) {
	*VIDEO_MODE = mode;
	return;
}


void setSpritePalette(uint32_t num, uint32_t ARGB) {
	uint32_t *PALETTE = (volatile uint32_t *)(SPRITE_PALLETE_ADDRESS + (0x400)*num);
	PALETTE[0] = ARGB;
	return;
}


uint32_t createSprite(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num) {
	uint32_t num;
	if(w < 16 && h < 16 ) { // create small sprite
		if(small_sprite_count >= 128) small_sprite_count = 0;
		num = small_sprite_count;

		// set sprite data
		uint8_t *DATA = (volatile uint8_t *)(SMALL_SPRITE_DATA_ADDRESS + (0x100)*small_sprite_count);
		for(int i = 0; i < 16; i++){
			for(int j = 0; j < 16; j++){
				DATA[(i<<6) + j] = (i<h && j<w) ? 0 : 1;
        	}
    	}

		// set sprite control
		uint32_t *CONTROL = (volatile uint32_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x4)*small_sprite_count);
		CONTROL[0] = calcSmallSpriteControl(x, y, 0, w, h, color_num);

		small_sprite_count++;
	}
	else { // create large sprite
		if(large_sprite_count >= 64) large_sprite_count = 0;
		num = 128 + large_sprite_count;

		// set sprite data
		uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000)*large_sprite_count);
		for(int i = 0; i < 64; i++){
			for(int j = 0; j < 64; j++){
				DATA[(i<<6) + j] = (i<h && j<w) ? 0 : 1;
        	}
		}

		// set sprite control
		uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*large_sprite_count);
		CONTROL[0] = calcLargeSpriteControl(x, y, w, h, color_num);

		large_sprite_count++;
		down_sprite_count++;
	}

	return num;
}


void changeSpriteColor(uint32_t sprite_num, uint32_t color_num) {
	if (sprite_num < 128 ) { // small sprite
		uint32_t *CONTROL = (volatile uint8_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x100)*sprite_num);
		CONTROL[0] &= 0xFFFFFFFC;
		CONTROL[0] |= color_num;
	}
	else { // large sprite
		uint32_t num = sprite_num - 128;
		uint32_t *CONTROL = (volatile uint8_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x100)*num);
		CONTROL[0] &= 0xFFFFFFFC;
		CONTROL[0] |= color_num;
	}
}


uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p){
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | p;
}


uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint32_t p){
    return ((z)<<29) | ((h-1)<<25) | ((w-1)<<21) | ((y+16)<<12) | ((x+16)<<2) | p;
}


void drawText(char* text, uint32_t length, int32_t x, int32_t y) {
	// ranges: x = 0~63; y = 0~35
	if(x >= 64 || y >= 36) return; // position out of range

	char *TEXT_DATA = (volatile char *)(TEXT_DATA_ADDRESS);
	for(int i = 0; i < length; i++) {
		int index = y*(0x40) + x + i;

		if(index >= 64*36) continue;

		TEXT_DATA[index] = text[i];
	}
	return;
}


void clearTextScreen() {
	char *TEXT_DATA = (volatile char *)(TEXT_DATA_ADDRESS);
	for(int i = 0; i < 0x900; i++) {
		TEXT_DATA[i] = 0;
	}

	return;
}
