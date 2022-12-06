#include <stdint.h>
#include <stdbool.h>
// #include "api.h"
// #include "systemcall.h"
// #include "tetris.h"
#include "tmp.h"

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile uint32_t controller_status = 0;

int prev_game_unit = -1;
int current_game_unit = 0; // 0: UNIT = 16; 1: UNIT = 8

uint8_t Block[7][4][4] = {
    {{1, 2, 4, 5}, {1, 5, 6, 10}, {5, 6, 8, 9}, {0, 4, 5, 9}}, 		// S_type
    {{4, 5, 6, 7}, {2, 6, 10, 14}, {8, 9, 10, 11}, {1, 5, 9, 13}}, 	// I_type
    {{2, 4, 5, 6}, {1, 5, 9, 10}, {4, 5, 6, 8}, {0, 1, 5, 9}}, 		// J_type
    {{0, 4, 5, 6}, {1, 2, 5, 9}, {4, 5, 6, 10}, {1, 5, 8, 9}}, 		// L_type
    {{1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}}, 		// O_type
    {{1, 4, 5, 6}, {1, 5, 6, 9}, {4, 5, 6, 9}, {1, 4, 5, 9}}, 		// T_type
    {{0, 1, 5, 6}, {2, 5, 6, 9}, {4, 5, 9, 10}, {1, 4, 5, 8}} 		// Z_type
};

int UNIT;       // 16 pixels or 8 pixels
int BLOCK_SIZE; // a block consists of 4UNIT * 4UNIT
int MARGIN;     // how many amount of UNIT
int game_state = WELCOME_PAGE_STATE;

int game_board_width;  // max: (FULL_X/8 - 2*MARGIN) = 24; min: (FULL_X/16 - 2*MARGIN) = 12
int game_board_height; // max: (FULL_Y/8) = 36; min: (FULL_X/16) = 18
bool game_board[36][24] = {false};
int fullLine[4] = {-1};
int full_line_count = 0;

int block_nonempty_idx[4] = {0};
int block_current_x_idx = 0; // index relative to game_board
int block_current_y_idx = 0; // index relative to game_board

int init_x_pos;
volatile int x_pos;

int main()
{
    // (*INT_ENABLE_REG) = 0x3; // disable cmd interrupt
    int last_global = global;
    int mode;

    // -------Draw Welcome Page--------------
    char *greeting = "TETRISX";
    drawText(greeting, 7, 27, 16);
    init_x_pos = 16*(0x40) + 27 + 7 - 1;
    x_pos = init_x_pos;

    char *instruction = "move X to START";
    drawText(instruction, 15, 23, 20);

    // char *instruction2 = "or just press CMD, whatever";
    // drawText(instruction2, 27, 17, 22);

    char *start_button = "START";
    drawText(start_button, 5, 20, 30);
    drawText(start_button, 5, 38, 30);

    char *difficulty = "(casual)     (not too casual)";
    drawText(difficulty, 29, 19, 32);
    // -------end draw Welcome Page--------------

    // -----------Set grid palette(background num 0)-----------------
    setBackgroundPalette(0, 0, 0x00000000); // Transparent
    setBackgroundPalette(0, 1, 0xFFFFFFFF); // White
    setBackgroundPalette(0, 2, 0xFF000000); // Black

    // -----------end set grid palette-------------


    // -----------Set gameboard palette(background num 1)-------------
    setBackgroundPalette(1, S_type, 0x80990000); // Dark Red
    setBackgroundPalette(1, I_type, 0x80994C00); // Dark Orange
    setBackgroundPalette(1, J_type, 0x80999900); // Dark Yellow
    setBackgroundPalette(1, L_type, 0x804C9900); // Dark Green
    setBackgroundPalette(1, O_type, 0x80009999); // Dark Turquoise
    setBackgroundPalette(1, T_type, 0x80000099); // Dark Blue
    setBackgroundPalette(1, Z_type, 0x804C0099); // Dark Purple
    setBackgroundPalette(1, 8, 0x00000000); // Transparent

    backgroundDrawRec(1, merge_xy(0, 0), merge_xy(FULL_X, FULL_Y), 8); // fill with tranparent first
    setBackgroundControl(1, merge_xy(0, 0), 0, 1);
    // -----------end set gameboard-------------


    // ---------set block palettes---------------
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
    int current_block_type = 0;
    int next_block_type = 1;
    // -----------end init block palettes-------------

    game_state = WELCOME_PAGE_STATE;
    setVideoMode(TEXT_MODE);

    while (1)
    {
        global =  getTimer();
        if(global != last_global) {

            mode = getMode();
            
            if(mode == TEXT_MODE) {
                if(game_state == WELCOME_PAGE_STATE) {
                    welcome_page_state();
                }
                else if(game_state == INIT_GAME_STATE) {
                    init_game_state(&rotation);
                }
            }
            else if (mode == GRAPHICS_MODE) {
                if(game_state == INIT_GAME_STATE) {
                }
                else if(game_state == CREATE_BLOCK_STATE) {
                    // random generate next block
                    current_block_type = next_block_type;
                    next_block_type = global % 7;
                    if(next_block_type == current_block_type) next_block_type++;
                    if(next_block_type == 7) next_block_type = 0;

                    // visualize current block
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
                else if(game_state == DRAW_TO_BG_STATE) {
                    // draw current block to background
                    int x_idx, y_idx;
                    for(int k = 0; k < 4; k++) {
                        x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
                        y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

                        if(y_idx == 0 || y_idx == 1) {
                            break;
                        }
                        game_board[y_idx][x_idx] = true;
                        backgroundDrawRec(1, merge_xy((x_idx+MARGIN)*UNIT, y_idx*UNIT), merge_xy(UNIT, UNIT), current_block_type);
                    }

                    // check full line
                    full_line_count = 0;
                    bool isFullLine = true;
                    for(int i = game_board_height-1; i >= 0; i--) {
                        isFullLine = true;
                        for(int j = 0; j < game_board_width; j++) {
                            isFullLine &= game_board[i][j];
                        }
                        if(isFullLine){ 
                            fullLine[full_line_count] = i;
                            full_line_count++;
                        }
                    }
                    
                    // set current block to zero-rotation and transparent
                    rotateBlock(current_block_type, 0);
                    setBlockControl(current_block_type, FULL_X/2-2*UNIT, 0, 1);


                    if(y_idx == 0 || y_idx == 1) {
                        game_state = GAME_OVER_STATE;
                    }
                    else {
                        game_state = DELETE_FULL_LINE_STATE;
                    }
                }
                else if(game_state == DELETE_FULL_LINE_STATE) {
                    // bool stop = false;
                    if(full_line_count == 0) {
                        //no deletion needed
                    }
                    else {
                        delete_full_line_state();
                    }

                    // game_state = (stop)? GAME_OVER_STATE : CREATE_BLOCK_STATE;
                    game_state = CREATE_BLOCK_STATE;
                }
                else if(game_state == GAME_OVER_STATE) {
                    //clear game_board array
                    for(int i = 0; i < game_board_height; i++) {
                        for(int j = 0; j < game_board_width; j++) {
                            game_board[i][j] = false;
                        }
                    }

                    //clear gameboard background
                    setVideoMode(TEXT_MODE);
                    backgroundDrawRec(1, merge_xy(0, 0), merge_xy(FULL_X, FULL_Y), 8); // fill with tranparent
                    game_state = WELCOME_PAGE_STATE;
                }
            }
            last_global = global;
        }
    }
    return 0;
}


void welcome_page_state() {
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
            current_game_unit = ((x_pos % 0x40) < 0x20)? 0 : 1;

            // redraw title
            char *greeting = "TETRISX";
            drawText(greeting, 7, 27, 16);
            x_pos = init_x_pos;

            // switch to game
            game_state = INIT_GAME_STATE;
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


void init_game_state(int *rotation) {
    if(current_game_unit == prev_game_unit) {
        //do nothing
    }
    else {
        // ---------Set units--------------
        if(current_game_unit == 0) {
            UNIT = 16; // 16 pixels
            BLOCK_SIZE = UNIT*4; // a block consists of 4UNIT * 4UNIT
            MARGIN = 10*(16/UNIT); // how many amount of UNIT
            game_board_width = (FULL_X/UNIT)-2*MARGIN;
            game_board_height = FULL_Y/UNIT;
        }
        else {
            UNIT = 8; // 8 pixels
            BLOCK_SIZE = UNIT*4; // a block consists of 4UNIT * 4UNIT
            MARGIN = 10*(16/UNIT); // how many amount of UNIT
            game_board_width = (FULL_X/UNIT)-2*MARGIN;
            game_board_height = FULL_Y/UNIT;
        }
        // --------end set units------------

        // -----------Draw grid-------------
        backgroundDrawRec(0, merge_xy(0, 0), merge_xy(FULL_X, FULL_Y), 0); // fill with transparent first

        for(int i = 0; i < FULL_X; i += UNIT) {
            backgroundDrawRec(0, merge_xy(i, 0), merge_xy(1, FULL_Y), 2); // vertical lines
        }
        for(int j = 0; j < FULL_Y; j += UNIT) {
            backgroundDrawRec(0, merge_xy(0, j), merge_xy(FULL_X, 1), 2); // horizontal lines
        }

        for(int i = (MARGIN+1)*UNIT; i < FULL_X - (MARGIN*UNIT); i+=UNIT) {
            for(int j = 0; j < FULL_Y; j+=UNIT) {
                backgroundDrawRec(0, merge_xy(i, j), merge_xy(1, 1), 1); // grid points
            }
        }

        backgroundDrawRec(0, merge_xy(MARGIN*UNIT, 0), merge_xy(game_board_width*UNIT, UNIT+1), 1); // top boarder
        backgroundDrawRec(0, merge_xy(MARGIN*UNIT - UNIT, 0), merge_xy(UNIT, FULL_Y), 1); // left boarder
        backgroundDrawRec(0, merge_xy(FULL_X - (MARGIN*UNIT), 0), merge_xy(UNIT, FULL_Y), 1); // right boarder

        setBackgroundControl(0, merge_xy(0, 0), 5, 0); // put grid in front of blocks, blocks(large sprite) are rendered in z-plane 4
        // -----------end draw grid---------------


        // -----------draw blocks-------------
        initBlock(S_type, *rotation, FULL_X/2-2*UNIT);
        initBlock(I_type, *rotation, FULL_X/2-2*UNIT);
        initBlock(J_type, *rotation, FULL_X/2-2*UNIT);
        initBlock(L_type, *rotation, FULL_X/2-2*UNIT);
        initBlock(O_type, *rotation, FULL_X/2-2*UNIT);
        initBlock(T_type, *rotation, FULL_X/2-2*UNIT);
        initBlock(Z_type, *rotation, FULL_X/2-2*UNIT);
        // -----------end draw blocks-----------

        prev_game_unit = current_game_unit;
    }

    game_state = CREATE_BLOCK_STATE;
    setVideoMode(GRAPHICS_MODE);
    return;
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
            // TODO: rotation check
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
                game_state = DRAW_TO_BG_STATE;
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
            game_state = DRAW_TO_BG_STATE;
        }
    }

    return;
}


void delete_full_line_state() {
    // uint8_t *DATA = (volatile uint8_t *)(getBACKGROUND_DATA_ADDRESS() + (0x24000)*1); // background_num = 1
	uint8_t *DATA = (volatile uint8_t *)(getBACKGROUND_DATA_ADDRESS(1));
    
    // game board
    int k = 0;
    int jump = 0;
    for(int i = game_board_height-1; i > 0; i--) {
        while((k != full_line_count) && (((jump == 0) && (i == fullLine[k])) || ((i-jump) == fullLine[k]))) {
            jump++;
            k++;
        }

        // gameboard
        for(int j = 0; j < game_board_width; j++) {
            if((i-jump) < 0) {
                game_board[i][j] = false;
            }
            else {
                game_board[i][j] = game_board[i-jump][j];
            }
        }

        // background data
        for(int l = 0; l < UNIT; l++) {
            for(int j = MARGIN*UNIT; j < (MARGIN+game_board_width+1)*UNIT; j++) {
                if((i-jump) < 0) {
                    DATA[(0x200)*((i*UNIT)+l) + j] = 8;
                }
                else {
                    DATA[(0x200)*((i*UNIT)+l) + j] = DATA[(0x200)*(((i-jump)*UNIT)+l) + j];
                }
            }
        }
    }

    for(int i = 0; i < 4; i++) {
        fullLine[i] = -1;
    }
    full_line_count = 0;

    return;
}


uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x) {

    // set sprite data
    // uint8_t *DATA = (volatile uint8_t *)(getLARGE_SPRITE_DATA_ADDRESS() + (0x1000)*(block_type));
    uint32_t block_type_test = block_type;
    uint8_t *DATA = (volatile uint8_t *)(getLARGE_SPRITE_DATA_ADDRESS(block_type_test));

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
    // uint32_t *CONTROL = (volatile uint32_t *)(getLARGE_SPRITE_CONTROL_ADDRESS() + (0x4)*block_type);
    uint32_t large_sprite_count_test = block_type;
    uint32_t *CONTROL = (volatile uint32_t *)(getLARGE_SPRITE_CONTROL_ADDRESS(large_sprite_count_test));
    CONTROL[0] = calcLargeSpriteControl(merge_xy(x, 0), merge_xy(BLOCK_SIZE, BLOCK_SIZE), 1); // use transparent palette at initialization

	return block_type + 128;
}


void rotateBlock(uint8_t block_type, uint8_t rotation) {
	// uint8_t block_type = sprite_num - 128;
	// set sprite data
    // uint8_t *DATA = (volatile uint8_t *)(getLARGE_SPRITE_DATA_ADDRESS() + (0x1000)*(block_type));
    uint32_t block_type_test = block_type;
    uint8_t *DATA = (volatile uint8_t *)(getLARGE_SPRITE_DATA_ADDRESS(block_type_test));

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
    // uint32_t *CONTROL = (volatile uint32_t *)(getLARGE_SPRITE_CONTROL_ADDRESS() + (0x4)*block_type);
    uint32_t large_sprite_count_test = block_type;
    uint32_t *CONTROL = (volatile uint32_t *)(getLARGE_SPRITE_CONTROL_ADDRESS(large_sprite_count_test));
    CONTROL[0] = calcLargeSpriteControl(merge_xy(x, y), merge_xy(BLOCK_SIZE, BLOCK_SIZE), palette_num); // use transparent palette at initialization

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
