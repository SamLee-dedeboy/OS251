#include <stdint.h>
#include <stdbool.h>
#include "api.h"
#include "systemcall.h"
#include "tetris.h"

// define game states
#define CREATE_BLOCK_STATE 0
#define DROP_BLOCK_STATE 1
#define CHECK_FULL_LINE_STATE 2
#define GAME_OVER_STATE 3

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile int global = 42;
volatile uint32_t controller_status = 0;
int init_x_pos;
volatile int x_pos;

uint8_t Block[7][4][4] = {
    {{1, 2, 4, 5}, {1, 5, 6, 10}, {5, 6, 8, 9}, {0, 4, 5, 9}}, 		// S_type
    {{4, 5, 6, 7}, {2, 6, 10, 14}, {8, 9, 10, 11}, {1, 5, 9, 13}}, 	// I_type
    {{2, 4, 5, 6}, {1, 5, 9, 10}, {4, 5, 6, 8}, {0, 1, 5, 9}}, 		// J_type
    {{0, 4, 5, 6}, {1, 2, 5, 9}, {4, 5, 6, 10}, {1, 5, 8, 9}}, 		// L_type
    {{1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}}, 		// O_type
    {{1, 4, 5, 6}, {1, 5, 6, 9}, {4, 5, 6, 9}, {1, 4, 5, 9}}, 		// T_type
    {{0, 1, 5, 6}, {2, 5, 6, 9}, {4, 5, 9, 10}, {1, 4, 5, 8}} 		// Z_type
};

int game_state = CREATE_BLOCK_STATE;

int game_board_width = (FULL_X/UNIT)-2*MARGIN;
int game_board_height = FULL_Y/UNIT;
bool game_board[FULL_Y/UNIT][(FULL_X/UNIT)-2*MARGIN] = {false};

int block_nonempty_idx[4] = {0};
int block_current_x_idx = 0; // index relative to game_board
int block_current_y_idx = 0; // index relative to game_board

void drop_block_state(int32_t sprite_num, int *rotation);


int main()
{
    int last_global = global;
    int mode;

    // draw text
    char *greeting = "TETRISX";
    drawText(greeting, 7, 27, 16);
    init_x_pos = 16*(0x40) + 27 + 7 - 1;
    x_pos = init_x_pos;

    char *instruction = "move X to START";
    drawText(instruction, 15, 23, 20);

    char *instruction2 = "or just press CMD, whatever";
    drawText(instruction2, 27, 17, 22);

    char *start_button = "START";
    drawText(start_button, 5, 28, 30);


    // -----------draw sprite-----------------
    // setSpritePalette(1, 0, 0x00000000); // Transparent
    // setSpritePalette(1, 1, 0xFFFF0000); // Red
    // setSpritePalette(1, 2, 0xFF0000FF); // Blue
    // int32_t sprite_1 = createRecSprite(0, 0, 64, 64, 1, 1);
    // int32_t sprite_2 = createRecSprite(0, 0, 16, 16, 1, 2);

    // -----------Draw grid (background num 0)-----------------
    setBackgroundPalette(0, 0, 0x00000000); // Transparent
    setBackgroundPalette(0, 1, 0xFFFFFFFF); // White
    setBackgroundPalette(0, 2, 0xFF000000); // Black

    backgroundDrawRec(0, 0, 0, FULL_X, FULL_Y, 0); // fill with transparent first

    for(int i = 0; i < FULL_X; i += UNIT) {
        backgroundDrawRec(0, i, 0, 1, FULL_Y, 2); // vertical lines
    }
    for(int j = 0; j < FULL_Y; j += UNIT) {
        backgroundDrawRec(0, 0, j, FULL_X, 1, 2); // horizontal lines
    }

    for(int i = (MARGIN+1)*UNIT; i < FULL_X - (MARGIN*UNIT); i+=UNIT) {
        for(int j = 0; j < FULL_Y; j+=UNIT) {
            backgroundDrawRec(0, i, j, 1, 1, 1); // grid points
        }
    }
    
    backgroundDrawRec(0, MARGIN*UNIT, 0, game_board_width*UNIT, UNIT+1, 1); // top boarder
    backgroundDrawRec(0, MARGIN*UNIT - UNIT/2, 0, UNIT/2, FULL_Y, 1); // left boarder
    backgroundDrawRec(0, FULL_X - (MARGIN*UNIT), 0, UNIT/2, FULL_Y, 1); // right boarder

    setBackgroundControl(0, 0, 0, 5, 0); // put grid in front of blocks, blocks(large sprite) are rendered in z-plane 4
    // -----------end draw grid-------------

    // -----------Setup gameboard (background num 1)-------------
    setBackgroundPalette(1, S_type, 0x80990000); // Dark Red
    setBackgroundPalette(1, I_type, 0x80994C00); // Dark Orange
    setBackgroundPalette(1, J_type, 0x80999900); // Dark Yellow
    setBackgroundPalette(1, L_type, 0x804C9900); // Dark Green
    setBackgroundPalette(1, O_type, 0x80009999); // Dark Turquoise
    setBackgroundPalette(1, T_type, 0x80000099); // Dark Blue
    setBackgroundPalette(1, Z_type, 0x804C0099); // Dark Purple
    setBackgroundPalette(1, 8, 0x00000000); // Transparent

    backgroundDrawRec(1, 0, 0, FULL_X, FULL_Y, 8); // fill with tranparent first
    setBackgroundControl(1, 0, 0, 0, 1);
    // -----------end setup gameboard-------------

    // ---------Draw Blocks on large sprites---------------

    // color palatte_num = 0 for blocks
    setSpritePalette(0, S_type, 0xFFFF3333); // Red
    setSpritePalette(0, I_type, 0xFFFF9933); // Orange
    setSpritePalette(0, J_type, 0xFFFFFF33); // Yellow
    setSpritePalette(0, L_type, 0xFF99FF33); // Green
    setSpritePalette(0, O_type, 0xFF33FFFF); // Turquoise
    setSpritePalette(0, T_type, 0xFF3333FF); // Blue
    setSpritePalette(0, Z_type, 0xFF9933FF); // Purple
    setSpritePalette(0, 8, 0x00000000); // Transparent

    // transparent palette_num = 1 for blocks
    setSpritePalette(1, S_type, 0x00000000);
    setSpritePalette(1, I_type, 0x00000000);
    setSpritePalette(1, J_type, 0x00000000);
    setSpritePalette(1, L_type, 0x00000000);
    setSpritePalette(1, O_type, 0x00000000);
    setSpritePalette(1, T_type, 0x00000000);
    setSpritePalette(1, Z_type, 0x00000000);
    setSpritePalette(1, 8, 0x00000000);

    int rotation = 0;
    initBlock(S_type, rotation, FULL_X/2-2*UNIT);
    initBlock(I_type, rotation, FULL_X/2-2*UNIT);
    initBlock(J_type, rotation, FULL_X/2-2*UNIT);
    initBlock(L_type, rotation, FULL_X/2-2*UNIT);
    initBlock(O_type, rotation, FULL_X/2-2*UNIT);
    initBlock(T_type, rotation, FULL_X/2-2*UNIT);
    initBlock(Z_type, rotation, FULL_X/2-2*UNIT);

    int current_block_type = 0;
    int next_block_type = 1;
    // -----------end init blocks-------------

    setVideoMode(TEXT_MODE);

    while (1)
    {
        global =  getTimer();
        if(global != last_global) {

            mode = getMode();
            
            if(mode == TEXT_MODE) {
                welcome_page();
            }
            else if (mode == GRAPHICS_MODE) {
                if(game_state == CREATE_BLOCK_STATE) {
                    // random generate next block
                    current_block_type = next_block_type;
                    next_block_type = global % 7;
                    if(next_block_type == current_block_type) next_block_type++;
                    if(next_block_type == 7) next_block_type = 0;

                    // visuallize current block
                    setBlockControl(current_block_type, FULL_X/2-2*UNIT, 0, 0);

                    // visualize next blockgame_board
                    setBlockControl(next_block_type, (MARGIN/2)*UNIT, FULL_Y/3, 0);

                    // reset moving block info
                    block_current_x_idx = game_board_width/2 - 2; // the middle of the game_board
                    block_current_y_idx = 0;
                    rotation = 0;
                    for(int k = 0; k < 4; k++) {
                        block_nonempty_idx[k] = Block[current_block_type][rotation][k];
                    }

                    game_state = DROP_BLOCK_STATE;
                }
                else if(game_state == DROP_BLOCK_STATE) {
                    drop_block_state(current_block_type, &rotation);
                }
                else if(game_state == CHECK_FULL_LINE_STATE) {
                    // draw current block to background
                    int x_idx, y_idx;
                    for(int k = 0; k < 4; k++) {
                        x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
                        y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

                        if(y_idx == 0 || y_idx == 1) {
                            break;
                        }
                        game_board[y_idx][x_idx] = true;
                        backgroundDrawRec(1, (x_idx+MARGIN)*UNIT, y_idx*UNIT, UNIT, UNIT, current_block_type);
                    }
                    

                    // set current block to zero-rotation and transparent
                    rotateBlock(current_block_type, 0);
                    setBlockControl(current_block_type, FULL_X/2-2*UNIT, 0, 1);

                    // check full line

                    if(y_idx == 0 || y_idx == 1) {
                        game_state = GAME_OVER_STATE;
                    }
                    else {
                        game_state = CREATE_BLOCK_STATE;
                    }
                }
                else if(game_state == GAME_OVER_STATE) {
                    //clear gameboard array
                    for(int i = 0; i < game_board_height; i++) {
                        for(int j = 0; j < game_board_width; j++) {
                            game_board[i][j] = false;
                        }
                    }

                    //clear gameboard background
                    backgroundDrawRec(1, 0, 0, FULL_X, FULL_Y, 8); // fill with tranparent
                    setVideoMode(TEXT_MODE);
                }
            }
            last_global = global;
        }
    }
    return 0;
}


void welcome_page() {
    controller_status = getStatus();
    if (controller_status)
    {
        if((x_pos / 0x40 == 30) &&
            ((VIDEO_MEMORY[x_pos] == 'S') || 
             (VIDEO_MEMORY[x_pos] == 'T') || 
             (VIDEO_MEMORY[x_pos] == 'A') ||
             (VIDEO_MEMORY[x_pos] == 'R') || 
             (VIDEO_MEMORY[x_pos] == 'T'))) {
            
            // clear 'X'
            if (VIDEO_MEMORY[x_pos] == 'X') VIDEO_MEMORY[x_pos] = 0;

            // redraw title
            char *greeting = "TETRISX";
            drawText(greeting, 7, 27, 16);
            x_pos = init_x_pos;

            // switch to game
            game_state = CREATE_BLOCK_STATE;
            setVideoMode(GRAPHICS_MODE);
            return;
        }

        if (VIDEO_MEMORY[x_pos] == 'X') VIDEO_MEMORY[x_pos] = 0;
        if (controller_status & 0x1)
        {
            if (x_pos & 0x3F)
            {
                x_pos--;
            }
        }
        if (controller_status & 0x2)
        {
            if (x_pos >= 0x40)
            {
                x_pos -= 0x40;
            }
        }
        if (controller_status & 0x4)
        {
            if (x_pos < 0x8C0)
            {
                x_pos += 0x40;

            }
        }
        if (controller_status & 0x8)
        {
            if ((x_pos & 0x3F) != 0x3F)
            {
                x_pos++;
            }
        }
        if (VIDEO_MEMORY[x_pos] == 0) VIDEO_MEMORY[x_pos] = 'X';
    }
}


void drop_block_state(int32_t block_type, int *rotation) {
    controller_status = getStatus();
    if(controller_status) {
        if (controller_status & 0x1) // left
        {
            if(checkCollide_X(-UNIT) == false) {
                moveSprite(block_type+128, -UNIT, 0);
                block_current_x_idx--;
            }
        }
        if (controller_status & 0x2) // up
        {
            *rotation = (*rotation == 3)? 0 : *rotation + 1;
            rotateBlock(block_type, *rotation);
        }
        if (controller_status & 0x4) // down
        {
            if(checkCollide_Y() == false) {
                moveSprite(block_type+128, 0, UNIT);
                block_current_y_idx++;
            }
            else {
                game_state = CHECK_FULL_LINE_STATE;
            }
        }
        if (controller_status & 0x8) // right
        {
            if(checkCollide_X(UNIT) == false) {
                moveSprite(block_type+128, UNIT, 0);
                block_current_x_idx++;
            }
        }
    }

    if(global % 10 == 0) {
        if(checkCollide_Y() == false) {
            moveSprite(block_type+128, 0, UNIT);
            block_current_y_idx++;
        }
        else {
            game_state = CHECK_FULL_LINE_STATE;
        }
    }

    return;
}


uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x) {

    // set sprite data
    uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000)*(block_type));

    // clear to transparent
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            DATA[(i<<6) + j] = 8;
        }
    }

    int sub_block;
    int start_x, start_y;
    for(int k = 0; k < 4; k++) {
        sub_block = Block[block_type][rotation][k];
        start_x = (sub_block % 4)*UNIT;
        start_y = (sub_block / 4)*UNIT;

        for(int i = 0; i < UNIT; i++) {
            for(int j = 0; j < UNIT; j++) {
                DATA[((start_y+i)<<6) + (start_x+j)] = block_type;
            }
        }
    }

    // set sprite control
    uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*block_type);
    CONTROL[0] = calcLargeSpriteControl(x, 0, BLOCK_SIZE, BLOCK_SIZE, 1); // use transparent palette at initialization

	return block_type + 128;
}


void rotateBlock(uint8_t block_type, uint8_t rotation) {
	// uint8_t block_type = sprite_num - 128;
	// set sprite data
    uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000)*(block_type));

    // clear to transparent
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            DATA[(i<<6) + j] = 8;
        }
    }

    int sub_block;
    int start_x, start_y;
    for(int k = 0; k < 4; k++) {
        sub_block = Block[block_type][rotation][k];
        start_x = (sub_block % 4)*UNIT;
        start_y = (sub_block / 4)*UNIT;

        for(int i = 0; i < UNIT; i++) {
            for(int j = 0; j < UNIT; j++) {
                DATA[((start_y+i)<<6) + (start_x+j)] = block_type;
            }
        }
        block_nonempty_idx[k] = sub_block;
    }
}


void setBlockControl(uint8_t block_type, int32_t x, int32_t y, uint8_t palette_num) {
	// set large sprite control
    uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*block_type);
    CONTROL[0] = calcLargeSpriteControl(x, y, BLOCK_SIZE, BLOCK_SIZE, palette_num); // use transparent palette at initialization

	return;
}


bool checkCollide_X(int32_t d_x) {
	int x_idx, y_idx;
    if(d_x < 0) {
        for(int k = 0; k < 4; k++) {
            x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
            y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

            if(x_idx - 1 == -1) {
                return true;
            }
            else if(game_board[y_idx][x_idx-1] == true) {
                return true;
            }
        }
    }
    else if(d_x > 0) {
        for(int k = 0; k < 4; k++) {
            x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
            y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

            if(x_idx + 1 == game_board_width) {
                return true;
            }
            else if(game_board[y_idx][x_idx+1] == true) {
                return true;
            }
        }
    }

	return false;
}


bool checkCollide_Y() {
    int x_idx, y_idx;
    for(int k = 3; k >= 0; k--) {
        x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
        y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

        if(y_idx + 1 == game_board_height) {
            return true;
        }
        else if(game_board[y_idx+1][x_idx] == true) {
            return true;
        }
    }

	return false;
}


// int checkCollide_X(uint8_t block_type, uint8_t rotation, int32_t d_x) {
// 	// uint8_t block_type = sprite_num - 128;
// 	uint32_t x;

// 	// large sprite
// 	uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*(block_type));
// 	x = (CONTROL[0] & 0x7FE) >> 2;

// 	// x and y are shifted +64 inside registers
// 	int sub_block, start_x, end_x;
// 	if (d_x < 0) {
// 		for(int k = 0; k < 4; k++) {
// 			sub_block = Block[block_type][rotation][k];
// 			start_x = sub_block % 4;

// 			if((x-64)+(start_x*UNIT) <= MARGIN*UNIT) {
// 				return 1;
// 			}
// 		}
// 	}
// 	else if (d_x > 0) {
// 		for(int k = 0; k < 4; k++) {
// 			sub_block = Block[block_type][rotation][k];
// 			end_x = (sub_block % 4) + 1;
			
// 			if(((x-64)+(end_x*UNIT)) >= (FULL_X - (MARGIN*UNIT))) {
// 				return 1;
// 			}
// 		}
// 	}
// 	else {
// 		return 0;
// 	}

// 	return 0;
// }


// int checkCollide_Y(uint8_t block_type, uint8_t rotation, int32_t d_y) {
// 	// uint8_t block_type = sprite_num - 128;
// 	uint32_t y;

// 	// large sprite
// 	uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*(block_type));
// 	y = (CONTROL[0] & 0x1FF000) >> 12;


// 	// x and y are shifted +64 inside registers
// 	int sub_block, end_y;
// 	if (d_y > 0) {
// 		for(int k = 0; k < 4; k++) {
// 			sub_block = Block[block_type][rotation][k];
// 			end_y = (sub_block / 4) + 1;
			
// 			if(((y-64)+(end_y*UNIT)) >= FULL_Y) {
// 				return 1;
// 			}
// 		}
// 	}
// 	else {
// 		return 0;
// 	}

// 	return 0;
// }


// int moveBlock(uint16_t sprite_num, int32_t d_x, int32_t d_y) {
// 	if(sprite_num < 0 || sprite_num > 191) return -1;

// 	uint32_t x, y;

// 	// large sprite
// 	uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4)*(sprite_num - 128));
// 	x = (CONTROL[0] & 0x7FE) >> 2;
// 	y = (CONTROL[0] & 0x1FF000) >> 12;

// 	// x and y are shifted +64 inside registers
// 	if ((d_x < 0) && ((x-64) <= MARGIN*UNIT)) {
// 		x += 0;
// 	}
// 	else if ((d_x > 0) && (((x-64) + BLOCK_SIZE) >= FULL_X - (MARGIN*UNIT))) {
// 		x += 0;
// 	}
// 	else {
// 		x += d_x;
// 	}

// 	y += d_y;

// 	CONTROL[0] &= ~(0X1FFFFC);
// 	CONTROL[0] |= (x<<2);
// 	CONTROL[0] |= (y<<12);

// 	return 1;
// }