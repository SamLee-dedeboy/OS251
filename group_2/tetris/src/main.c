#include <stdint.h>
#include <stdbool.h>
#include "api.h"
#include "tetris.h"

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

int score = 0;
int total_full_line = 0;

uint8_t Digit[10][64] = {
    { 1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 0, 0, 1, 1, 0, 0, 
      1, 1, 0, 0, 1, 1, 0, 0, 
      1, 1, 0, 0, 1, 1, 0, 0, 
      1, 1, 0, 0, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0 },      // 0

    { 0, 0, 1, 1, 0, 0, 0, 0, 
      0, 0, 1, 1, 0, 0, 0, 0,
      0, 0, 1, 1, 0, 0, 0, 0,
      0, 0, 1, 1, 0, 0, 0, 0,
      0, 0, 1, 1, 0, 0, 0, 0, 
      0, 0, 1, 1, 0, 0, 0, 0,
      0, 0, 1, 1, 0, 0, 0, 0, 
      0, 0, 1, 1, 0, 0, 0, 0 },      // 1
    
    { 1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0,
      0, 0, 1, 1, 1, 1, 0, 0,
      1, 1, 1, 1, 0, 0, 0, 0, 
      1, 1, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0 },      // 2

    { 1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0 },      // 3

    { 1, 1, 0, 0, 1, 1, 0, 0, 
      1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0 },      // 4

    { 1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0 },      // 5

    { 1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0 },      // 6

    { 1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0 },      // 7

    { 1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 0, 0, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0 },      // 8

    { 1, 1, 1, 1, 1, 1, 0, 0, 
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0,
      0, 0, 0, 0, 1, 1, 0, 0, 
      0, 0, 0, 0, 1, 1, 0, 0 }      // 9
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

// Nuber of sprites: 0~127: small sprite; 128~191: large sprite
int small_sprite_count = 0; // max: 128 small sprites
int large_sprite_count = 0; // max: 64 large sprites

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
    setBackgroundColor(0, 0, 0x00000000); // Transparent
    setBackgroundColor(0, 1, 0xFFFFFFFF); // White
    setBackgroundColor(0, 2, 0xFF000000); // Black

    // -----------end set grid palette-------------


    // -----------Set gameboard palette(background num 1)-------------
    setBackgroundColor(1, S_type, 0x80990000); // Dark Red
    setBackgroundColor(1, I_type, 0x80994C00); // Dark Orange
    setBackgroundColor(1, J_type, 0x80999900); // Dark Yellow
    setBackgroundColor(1, L_type, 0x804C9900); // Dark Green
    setBackgroundColor(1, O_type, 0x80009999); // Dark Turquoise
    setBackgroundColor(1, T_type, 0x80000099); // Dark Blue
    setBackgroundColor(1, Z_type, 0x804C0099); // Dark Purple
    setBackgroundColor(1, 8, 0x00000000); // Transparent

    backgroundDrawRec(1, merge_xy(0, 0), merge_xy(FULL_X, FULL_Y), 8); // fill with tranparent first
    setBackgroundSpriteControl(1, calcBackgroundControl(0,0,0,1));
    // -----------end set gameboard-------------


    // ---------set block palettes---------------
    // color palatte_num = 0 for blocks
    setColor(0, S_type, 0xFFFF3333); // Red
    setColor(0, I_type, 0xFFFF9933); // Orange
    setColor(0, J_type, 0xFFFFFF33); // Yellow
    setColor(0, L_type, 0xFF99FF33); // Green
    setColor(0, O_type, 0xFF33FFFF); // Turquoise
    setColor(0, T_type, 0xFF3333FF); // Blue
    setColor(0, Z_type, 0xFF9933FF); // Purple
    setColor(0, 8, 0x00000000); // Transparent

    // transparent palette_num = 1 for blocks
    setColor(1, S_type, 0x00000000);
    setColor(1, I_type, 0x00000000);
    setColor(1, J_type, 0x00000000);
    setColor(1, L_type, 0x00000000);
    setColor(1, O_type, 0x00000000);
    setColor(1, T_type, 0x00000000);
    setColor(1, Z_type, 0x00000000);
    setColor(1, 8, 0x00000000);

    int rotation = 0;
    int current_block_type = 0;
    int next_block_type = 1;
    // -----------end init block palettes-------------

    // ---------set digit palettes---------------
    // color palatte_num = 2 for digits
    setColor(2, 0, 0xFFFFFFFF); // White
    setColor(2, 8, 0x00000000); // Transparent

    // transparent palette_num = 3 for digits
    setColor(3, 0, 0x00000000);
    setColor(3, 8, 0x00000000);

    // -----------end init digit palettes-------------

    game_state = WELCOME_PAGE_STATE;
    setTextMode();

    while (1)
    {
        // global =  getTimer();
        global =  getTicks();
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
                    // next_block_type = global % 7;
                    next_block_type = genRandom(7);
                    if(next_block_type == current_block_type) next_block_type++;
                    if(next_block_type == 7) next_block_type = 0;

                    // visualize current block
                    setLargeSpriteControl(current_block_type, calcLargeSpriteControl(FULL_X/2-2*UNIT, 0, BLOCK_SIZE, BLOCK_SIZE, 0));

                    // visualize next blockgame_board
                    setLargeSpriteControl(next_block_type, calcLargeSpriteControl((MARGIN/2)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 0));

                    // visualize 2-digit score
                    setLargeSpriteControl((score%100)/10+20, calcLargeSpriteControl(FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 2));
                    setLargeSpriteControl(score%10+10, calcLargeSpriteControl(FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 2));
    
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
                    setLargeSpriteControl(current_block_type, calcLargeSpriteControl(FULL_X/2-2*UNIT, 0, BLOCK_SIZE, BLOCK_SIZE, 1));


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
                        total_full_line += full_line_count;

                        // clear 2-digit score
                        // setDigitControl((score%100)/10+10, FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, 3);
                        // setDigitControl(score%10, FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, 3);
                        setLargeSpriteControl((score%100)/10+20, calcLargeSpriteControl(FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 3));
                        setLargeSpriteControl(score%10+10, calcLargeSpriteControl(FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 3));
    
                        // draw 2-digit score
                        // setDigitControl((total_full_line%100)/10+10, FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, 2);
                        // setDigitControl(total_full_line%10, FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, 2);
                        setLargeSpriteControl((total_full_line%100)/10+20, calcLargeSpriteControl(FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 2));
                        setLargeSpriteControl(total_full_line%10+10, calcLargeSpriteControl(FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 2));
    
                        score = total_full_line;

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
                    setTextMode();
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
            // clear 2-digit score
            // setDigitControl((score%100)/10+10, FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, 3);
            // setDigitControl(score%10, FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, 3);
            setLargeSpriteControl((score%100)/10+20, calcLargeSpriteControl(FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 3));
            setLargeSpriteControl(score%10+10, calcLargeSpriteControl(FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, BLOCK_SIZE, BLOCK_SIZE, 3));
    
            total_full_line = 0;
            score = 0;
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

       setBackgroundSpriteControl(0, calcBackgroundControl(0,0,5,0)); // put grid in front of blocks, blocks(large sprite) are rendered in z-plane 4
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

        // -----------draw digits (for 2-digit score)-------------
        initDigit(0, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(1, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(2, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(3, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(4, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(5, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(6, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(7, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(8, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(9, FULL_X-MARGIN*UNIT, FULL_Y/3);

        initDigit(10, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(11, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(12, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(13, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(14, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(15, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(16, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(17, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(18, FULL_X-MARGIN*UNIT, FULL_Y/3);
        initDigit(19, FULL_X-MARGIN*UNIT, FULL_Y/3);
        // -----------end draw digits (for 2-digit score-----------

        prev_game_unit = current_game_unit;
    }

    game_state = CREATE_BLOCK_STATE;
    setGraphicsMode();
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
    //group_2 no changing background data APIs.
    uint8_t *DATA = (volatile uint8_t *)(BACKGROUND_DATA_ADDRESS + (0x24000)*1); // background_num = 1
    
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

    // group_2 no changing large sprite data APIs.
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
    setLargeSpriteControl(block_type, calcLargeSpriteControl(x, 0, BLOCK_SIZE, BLOCK_SIZE, 1));

	return block_type + 128;
}

int initDigit(int digit_type, int32_t x, int32_t y) {
    // set small sprite data
    // uint8_t *DATA = (volatile uint8_t *)(getLARGE_SPRITE_DATA_ADDRESS() + (0x1000)*(block_type));
    uint32_t digit_type_test = digit_type + 10;
    uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000)*(digit_type_test));

    // clear to transparent
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            DATA[(i<<6) + j] = 8;
        }
    }

    int sub_block;
    int start_x, start_y;
    int UNIT_tmp = UNIT/2;
    for(int k = 0; k < 64; k++) {
        if (Digit[digit_type%10][k] == 1) {
            sub_block = k;
            start_x = (sub_block % 8)*UNIT_tmp;
            start_y = (sub_block / 8)*UNIT_tmp;

            for(int i = 0; i < UNIT_tmp; i++) {
                for(int j = 0; j < UNIT_tmp; j++) {
                    DATA[((start_y+i)<<6) + (start_x+j)] = 0;
                }
            }
        }
        // sub_block = Block[block_type][rotation][k];
        // start_x = (sub_block % 4)*UNIT;
        // start_y = (sub_block / 4)*UNIT;

        // for(int i = 0; i < UNIT; i++) {
        //     for(int j = 0; j < UNIT; j++) {
        //         DATA[((start_y+i)<<6) + (start_x+j)] = block_type;
        //     }
        // }
    }

    // set small sprite control
    // uint32_t *CONTROL = (volatile uint32_t *)(getLARGE_SPRITE_CONTROL_ADDRESS() + (0x4)*block_type);
    uint32_t large_sprite_count_test = digit_type + 10;
    setLargeSpriteControl(large_sprite_count_test, calcLargeSpriteControl(x, y, BLOCK_SIZE, BLOCK_SIZE, 3));

	return digit_type + 128 + 10;
}

void rotateBlock(uint8_t block_type, uint8_t rotation) {
	// uint8_t block_type = sprite_num - 128;

    // group_2 no changing large sprite data API. 
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

// group_2 no getting mode API. function preserved.
uint32_t getMode() {
    if (MODE_CONTROL & 1)   return 1;
    else    return 0;
}

// Pure tetris function. function preserved.
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

// Pure tetris function. function preserved.
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

// group_2 no text drawing on different positions APIs. function preserved.
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


// group_2 no refresh rate setting API. function preserved.
void setRefreshRate(uint32_t rate) {
	uint32_t *VIDEO_MODE = (volatile uint32_t *)(MODE_CONTROL_REGISTER);
	VIDEO_MODE[0] &= ~(0xFE);
	VIDEO_MODE[0] |= (rate<<1);
}

uint32_t merge_xy (uint32_t x, uint32_t y) {
    return (x<<16) | (y);
}

//group_2 no changing background data APIs.
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

// contructed by group_2 APIs, fundtion preserved.
int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y) {
	if(sprite_num < 0 || sprite_num > 191) return -1;

	uint32_t x, y;

	if (sprite_num < 128 ) { // small sprite
        x = (getSmallSpriteControl(sprite_num) & 0x7FE) >> 2;
		y = (getSmallSpriteControl(sprite_num) & 0x1FF000) >> 12;
		x += d_x;
		y += d_y;

        setSmallSpriteControl(sprite_num, ((getSmallSpriteControl(sprite_num) & ~(0X1FFFFC)) | (x<<2)) | (y<<12));
	}
	else { // large sprite
        x = (getLargeSpriteControl(sprite_num - 128) & 0x7FE) >> 2;
		y = (getLargeSpriteControl(sprite_num - 128) & 0x1FF000) >> 12;
		x += d_x;
		y += d_y;

        setLargeSpriteControl(sprite_num - 128, ((getLargeSpriteControl(sprite_num - 128) & ~(0X1FFFFC)) | (x<<2)) | (y<<12));
	}
	return 1;
}