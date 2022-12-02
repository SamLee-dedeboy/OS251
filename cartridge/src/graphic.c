#include "graphic.h"

volatile uint8_t *BACKGROUND_DATA = (volatile uint8_t *)(0x50000000);
#define LARGE_SPRITE_DATA_ADDRESS 0x500B4000
#define SMALL_SPRITE_DATA_ADDRESS 0x500F4000

volatile uint32_t *BACKGROUND_PALLETE = (volatile uint32_t *)(0x500FC000);
uint32_t SPRITE_PALLETE_ADDRESS = 0x500FD000;

volatile char *TEXT_DATA = (volatile char *)(0x500FE800);
volatile uint32_t *BACKGROUND_CONTROL = (volatile uint32_t *)(0x500FF100);
#define LARGE_SPRITE_CONTROL_ADDRESS 0x500FF114
#define SMALL_SPRITE_CONTROL_ADDRESS 0x500FF214

volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500FF414);

uint32_t SystemCall(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

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
		CONTROL[0] = CalcSmallSpriteControl(x, y, w, h, color_num);

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
		CONTROL[0] = CalcLargeSpriteControl(x, y, w, h, color_num);

		large_sprite_count++;
		down_sprite_count++;
	}

	return num;
}

uint32_t * createBlock(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num, int32_t block_id) {
	uint32_t sprites[4];

	if (block_id == 0) {
		// A
		// AAA
		sprites[0] = createSprite(x, y, w, h, color_num) - 1;
		sprites[1] = createSprite(x, y+h, w, h, color_num) - 1;
		sprites[2] = createSprite(x+w, y+h, w, h, color_num) - 1;
		sprites[3] = createSprite(x+w*2, y+h, w, h, color_num) - 1;
	}

	return sprites;
}

void dropBlock(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num, int32_t block_id, uint32_t *current_blocks, int rotate_id) {
	for (int tmp=0; tmp<4; tmp++) {
		if (block_id==0) {
			if (rotate_id%4==0) {
				//B
				//BBB
				if (tmp==1)	y += h;
				else if (tmp==2) x += w;
				else if (tmp==3) x += w;
			}
			else if (rotate_id%4==1) {
				//BB
				//B
				//B
				if (tmp==1) y += h;
				else if (tmp==2) y += h;
				else if (tmp==3) {
					x += w;
					y -= 2*h;
				}
			}
			else if (rotate_id%4==2) {
				//BBB
				//  B
				if (tmp==1) x += w;
				else if (tmp==2) x += w;
				else if (tmp==3) y += h;
			}
			else if (rotate_id%4==3) {
				// B
				// B
				//BB
				if (tmp==0) x += w;
				else if (tmp==1) y += h;
				else if (tmp==2) y += h;
				else if (tmp==3) x -= w;
			}
		}
		// set sprite data
		// uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000)*tmp);
		// for(int i = 0; i < 64; i++){
		// 	for(int j = 0; j < 64; j++){
		// 		DATA[(i<<6) + j] = (i<h && j<w) ? 0 : 1;
		// 	}
		// }

		// set sprite control
		uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*tmp);
		CONTROL[0] = CalcLargeSpriteControl(x, y, w, h, color_num);
	}
}

void clearBlock(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color_num, int32_t block_id, uint32_t *current_blocks, int rotate_id){
	large_sprite_count = 0;
	for (int tmp=0; tmp<4; tmp++) {
		if (block_id==0) {
			if (rotate_id%4==0) {
				//B
				//BBB
				if (tmp==1)	y += h;
				else if (tmp==2) x += w;
				else if (tmp==3) x += w;
			}
			else if (rotate_id%4==1) {
				//BB
				//B
				//B
				if (tmp==1) y += h;
				else if (tmp==2) y += h;
				else if (tmp==3) {
					x += w;
					y -= 2*h;
				}
			}
			else if (rotate_id%4==2) {
				//BBB
				//  B
				if (tmp==1) x += w;
				else if (tmp==2) x += w;
				else if (tmp==3) y += h;
			}
			else if (rotate_id%4==3) {
				// B
				// B
				//BB
				if (tmp==0) x += w;
				else if (tmp==1) y += h;
				else if (tmp==2) y += h;
				else if (tmp==3) x -= w;
			}
		}
		// set dropped sprite data to the back
		uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000)*(64-down_sprite_count+tmp));
		for(int i = 0; i < 64; i++){
			for(int j = 0; j < 64; j++){
				DATA[(i<<6) + j] = (i<h && j<w) ? 0 : 1;
			}
		}

		// set dropped sprite control to the back
		uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*(64-down_sprite_count+tmp));
		CONTROL[0] = CalcLargeSpriteControl(x, y, w, h, color_num);
	}
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
// void moveSprite(int sprite_num, int d_x, int d_y);

uint32_t CalcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p){
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | p;
}

uint32_t CalcSmallSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p){
    return ((h-1)<<26) | ((w-1)<<21) | ((y+16)<<12) | ((x+16)<<2) | p;
}
