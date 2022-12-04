#include "graphic.h"


// Nuber of sprites: 0~127: small sprite; 128~191: large sprite
int small_sprite_count = 0; // max: 128 small sprites
int large_sprite_count = 0; // max: 64 large sprites
int down_sprite_count = 0; 


int setVideoMode(int mode) {
	if(!(mode == 0 || mode == 1)) return -1;

	uint32_t *VIDEO_MODE = (volatile uint32_t *)(MODE_CONTROL_REGISTER);
	VIDEO_MODE[0] &= ~(0x1);
	VIDEO_MODE[0] |= mode;
	return 1;
}

void setRefreshRate(uint8_t rate) {
	uint32_t *VIDEO_MODE = (volatile uint32_t *)(MODE_CONTROL_REGISTER);
	VIDEO_MODE[0] &= ~(0xFE);
	VIDEO_MODE[0] |= (rate<<1);
}


int setBackgroundPalette(uint8_t palette_num, uint32_t entry_num, uint32_t ARGB) {
	if((palette_num > 3 || entry_num > 255)) return -1;

	uint32_t *PALETTE = (volatile uint32_t *)(BACKGROUND_PALLETE_ADDRESS + (0x400)*palette_num + (0x4)*entry_num);
	PALETTE[0] = ARGB;
	return 1;
}


int backgroundDrawRec(uint8_t background_num, 
						int32_t x, int32_t y, 
						uint32_t w, uint32_t h,
						uint8_t colorEntry) {
	if(background_num < 0 || background_num > 3) return -1;

	// set background data
	uint8_t *DATA = (volatile uint8_t *)(BACKGROUND_DATA_ADDRESS + (0x24000)*background_num);
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			DATA[(0x200)*(y+i) + (x+j)] = colorEntry;
		}
	}

	return 1;
}


int setBackgroundControl(uint8_t background_num, int32_t x, int32_t y, int32_t z, uint8_t palette_num) {
	if(background_num < 0 || background_num > 3) return -1;
	if(palette_num < 0 || palette_num > 3) return -1;
	
	// set background control
	uint32_t *CONTROL = (volatile uint32_t *)(BACKGROUND_CONTROL_ADDRESS + (0x4)*background_num);
	CONTROL[0] = ((z)<<22) | ((y+288)<<12) | ((x+512)<<2) | palette_num;
	
	return 1;
}


int changeBackgroundPalette(uint8_t background_num, uint8_t palette_num) {
	if(background_num < 0 || background_num > 3) return -1;
	if(palette_num < 0 || palette_num > 3) return -1;

	uint32_t *CONTROL = (volatile uint32_t *)(BACKGROUND_CONTROL_ADDRESS + (0x4)*background_num);
	CONTROL[0] &= 0xFFFFFFFC;
	CONTROL[0] |= palette_num;

	return 1;
}


int setSpritePalette(uint8_t palette_num, uint32_t entry_num, uint32_t ARGB) {
	if(palette_num > 3 || entry_num > 255) return -1;

	uint32_t *PALETTE = (volatile uint32_t *)(SPRITE_PALLETE_ADDRESS + (0x400)*palette_num + (0x4)*entry_num);
	PALETTE[0] = ARGB;
	return 1;
}


uint16_t createRecSprite(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t palette_num, uint8_t colorEntry) {
	int16_t num;
	if(w < 16 && h < 16 ) { // create small sprite
		if(small_sprite_count >= 128) small_sprite_count = 0;
		num = small_sprite_count;

		// set sprite data
		uint8_t *DATA = (volatile uint8_t *)(SMALL_SPRITE_DATA_ADDRESS + (0x100)*small_sprite_count);
		for(int i = 0; i < 16; i++){
			for(int j = 0; j < 16; j++){
				DATA[(i<<6) + j] = (i<h && j<w) ? colorEntry : 0;
        	}
    	}

		// set sprite control
		uint32_t *CONTROL = (volatile uint32_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x4)*small_sprite_count);
		CONTROL[0] = calcSmallSpriteControl(x, y, 0, w, h, palette_num);

		small_sprite_count++;
	}
	else { // create large sprite
		if(large_sprite_count >= 64) large_sprite_count = 0;
		num = 128 + large_sprite_count;

		// set sprite data
		uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000)*large_sprite_count);
		for(int i = 0; i < 64; i++){
			for(int j = 0; j < 64; j++){
				DATA[(i<<6) + j] = (i<h && j<w) ? colorEntry : 0;
        	}
		}

		// set sprite control
		uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*large_sprite_count);
		CONTROL[0] = calcLargeSpriteControl(x, y, w, h, palette_num);

		large_sprite_count++;
		down_sprite_count++;
	}

	return num;
}


int moveSprite(uint16_t sprite_num, uint32_t d_x, uint32_t d_y) {
	if(sprite_num < 0 || sprite_num > 191) return -1;

	uint32_t x, y;

	if (sprite_num < 128 ) { // small sprite
		uint32_t *CONTROL = (volatile uint32_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x4)*sprite_num);
		x = (CONTROL[0] & 0x7FE) >> 2;
		y = (CONTROL[0] & 0x1FF000) >> 12;

		x += d_x;
		y += d_y;

		CONTROL[0] &= ~(0X1FFFFC); // clear out original x, y bits
		CONTROL[0] |= (x<<2);
		CONTROL[0] |= (y<<12);
	}
	else { // large sprite
		uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*(sprite_num - 128));
		x = (CONTROL[0] & 0x7FE) >> 2;
		y = (CONTROL[0] & 0x1FF000) >> 12;

		x += d_x;
		y += d_y;

		CONTROL[0] &= ~(0X1FFFFC);
		CONTROL[0] |= (x<<2);
		CONTROL[0] |= (y<<12);
	}
	return 1;
}


int changeSpritePalette(uint16_t sprite_num, uint8_t palette_num) {
	if(sprite_num < 0 || sprite_num > 191) return -1;
	if(palette_num < 0 || palette_num > 3) return -1;

	if (sprite_num < 128 ) { // small sprite
		uint32_t *CONTROL = (volatile uint32_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x4)*sprite_num);
		CONTROL[0] &= 0xFFFFFFFC;
		CONTROL[0] |= palette_num;
	}
	else { // large sprite
		uint32_t num = sprite_num - 128;
		uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*num);
		CONTROL[0] &= 0xFFFFFFFC;
		CONTROL[0] |= palette_num;
	}

	return 1;
}


uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint8_t p){
    return ((z)<<29) | ((h-1)<<25) | ((w-1)<<21) | ((y+16)<<12) | ((x+16)<<2) | p;
}


uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t p){
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | p;
}


int drawText(char* text, uint32_t length, int32_t x, int32_t y) {
	// ranges: x = 0~63; y = 0~35
	if(x >= 64 || y >= 36) return -1; // position out of range

	char *TEXT_DATA = (volatile char *)(TEXT_DATA_ADDRESS);
	for(int i = 0; i < length; i++) {
		int index = y*(0x40) + x + i;

		if(index >= 64*36) continue;

		TEXT_DATA[index] = text[i];
	}

	return 1;
}


void clearTextScreen() {
	char *TEXT_DATA = (volatile char *)(TEXT_DATA_ADDRESS);
	for(int i = 0; i < 0x900; i++) {
		TEXT_DATA[i] = 0;
	}

	return;
}
