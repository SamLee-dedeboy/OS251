#include "tmp.h"

// ------- replaced apis start --------
int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num) {
	uint32_t x = unmerge(xy, 0);
	uint32_t y = unmerge(xy, 1);
	controlBackground(background_num, x, y, z, palette_num);
	return 1;
}

int setVideoMode(uint32_t mode) {
	setDisplayMode(mode);
	return 1;
}

uint32_t getTimer() {
	return (uint32_t)getTicks();
}


// ------- replaced apis end --------

int drawText(char* text, uint32_t length, uint32_t x, uint32_t y) {
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

int setBackgroundPalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB) {
	if((palette_num > 3 || entry_num > 255)) return -1;

	uint32_t *PALETTE = (volatile uint32_t *)(BACKGROUND_PALLETE_ADDRESS + (0x400)*palette_num + (0x4)*entry_num);
	PALETTE[0] = ARGB;
	return 1;
}

int backgroundDrawRec(uint32_t background_num, 
						uint32_t xy, uint32_t wh,
						uint32_t colorEntry) {
	if(background_num < 0 || background_num > 3) return -1;

	int32_t x = xy>>16 & (0x0000FFFF);
	int32_t y = xy & (0x0000FFFF);
	uint32_t w = wh>>16 & (0x0000FFFF);
	uint32_t h = wh & (0x0000FFFF);

	// set background data
	uint8_t *DATA = (volatile uint8_t *)(BACKGROUND_DATA_ADDRESS + (0x24000)*background_num);
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			DATA[(0x200)*(y+i) + (x+j)] = colorEntry;
		}
	}

	return 1;
}

int setSpritePalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB) {
	if(palette_num > 3 || entry_num > 255) return -1;

	uint32_t *PALETTE = (volatile uint32_t *)(SPRITE_PALLETE_ADDRESS + (0x400)*palette_num + (0x4)*entry_num);
	PALETTE[0] = ARGB;
	return 1;
}

int getMode() {
	if (MODE_CONTROL & 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint32_t getLARGE_SPRITE_DATA_ADDRESS(uint32_t large_sprite_count) {
	return LARGE_SPRITE_DATA_ADDRESS + (0x1000)*large_sprite_count;
}

uint32_t getLARGE_SPRITE_CONTROL_ADDRESS(uint32_t large_sprite_count) {
	return LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*large_sprite_count;
}

uint32_t calcLargeSpriteControl(uint32_t xy, uint32_t wh, uint32_t p) {
	int32_t x = xy>>16 & (0x0000FFFF);
	int32_t y = xy & (0x0000FFFF);
	uint32_t w = wh>>16 & (0x0000FFFF);
	uint32_t h = wh & (0x0000FFFF);
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | p;
}

int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y) {
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


uint32_t getBACKGROUND_DATA_ADDRESS(uint32_t background_num) {
	return BACKGROUND_DATA_ADDRESS + (0x24000)*background_num;
}

uint32_t merge_xy(uint32_t x, uint32_t y) {
	return (x<<16) | y;
}

uint32_t unmerge(uint32_t xy, int flag) {
	if(flag == 0) { // return x
		return xy >> 16;
	} else { // return y
		return  (xy << 16) >> 16;
	}
}