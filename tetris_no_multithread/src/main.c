#include <stdint.h>
#include <stdbool.h>
#include "api.h"
#include "tetris.h"

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile int videoInt = 0;
volatile int last_videoInt;
volatile uint32_t controller_status = 0;

int prev_game_unit = -1;
int current_game_unit = 0; // 0: UNIT = 16; 1: UNIT = 8

uint8_t Block[7][4][4] = {
    {{1, 2, 4, 5}, {1, 5, 6, 10}, {5, 6, 8, 9}, {0, 4, 5, 9}},     // S_type
    {{4, 5, 6, 7}, {2, 6, 10, 14}, {8, 9, 10, 11}, {1, 5, 9, 13}}, // I_type
    {{2, 4, 5, 6}, {1, 5, 9, 10}, {4, 5, 6, 8}, {0, 1, 5, 9}},     // J_type
    {{0, 4, 5, 6}, {1, 2, 5, 9}, {4, 5, 6, 10}, {1, 5, 8, 9}},     // L_type
    {{1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}, {1, 2, 5, 6}},      // O_type
    {{1, 4, 5, 6}, {1, 5, 6, 9}, {4, 5, 6, 9}, {1, 4, 5, 9}},      // T_type
    {{0, 1, 5, 6}, {2, 5, 6, 9}, {4, 5, 9, 10}, {1, 4, 5, 8}}      // Z_type
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

uint8_t Score_text[5][25] = {
    { 0, 1, 1, 1, 1, 
      1, 0, 0, 0, 0, 
      0, 1, 1, 1, 0, 
      0, 0, 0, 0, 1, 
      1, 1, 1, 1, 0 },      // S

    { 0, 1, 1, 1, 1, 
      1, 0, 0, 0, 0, 
      1, 0, 0, 0, 0, 
      1, 0, 0, 0, 0, 
      0, 1, 1, 1, 1 },      // C

    { 0, 1, 1, 1, 0, 
      1, 0, 0, 0, 1, 
      1, 0, 0, 0, 1, 
      1, 0, 0, 0, 1, 
      0, 1, 1, 1, 0 },      // O

    { 0, 1, 1, 1, 0, 
      1, 0, 0, 0, 1, 
      1, 1, 1, 1, 1, 
      1, 0, 0, 1, 0, 
      1, 0, 0, 0, 1 },      // R

    { 1, 1, 1, 1, 1, 
      1, 0, 0, 0, 0, 
      1, 1, 1, 1, 1, 
      1, 0, 0, 0, 0, 
      1, 1, 1, 1, 1 },      // E
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
int border_color = 0;

uint32_t Thread_video_interrupt(void *param)
{
    last_videoInt = videoInt;
    while (1)
    {
        videoInt = systemcall(SYSIDEO, 0, 0, 0, 0, 0);
        if (videoInt != last_videoInt)
        {
            border_color = (border_color == 0)? 2 : 0;
            setBackgroundPalette(2, 1, Rand_sys()); // random color
            setBackgroundControl(0, merge_arg(0, 0), 5, border_color); // change border color upon video interrupt
            last_videoInt = videoInt;
        }
    }
}

int main()
{
    // (*INT_ENABLE_REG) = 0x3; // disable cmd interrupt
    // void *param;
    // thread_init(THREAD_INITIALLIZE, Thread_video_interrupt, param);

    int last_global = global;
    int last_videoInt = videoInt;
    int mode;

    // -------Draw Welcome Page--------------
    char *greeting = "TETRISX";
    drawText(greeting, 7, 27, 16);
    init_x_pos = 16 * (0x40) + 27 + 7 - 1;
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

    setBackgroundPalette(2, 0, 0x00000000); // Transparent
    setBackgroundPalette(2, 1, 0xFFFF99CC); // Pink
    setBackgroundPalette(2, 2, 0xFF000000); // Black

    // -----------end set grid palette-------------

    // -----------Set gameboard palette(background num 1)-------------
    setBackgroundPalette(1, S_type, 0x80990000); // Dark Red
    setBackgroundPalette(1, I_type, 0x80994C00); // Dark Orange
    setBackgroundPalette(1, J_type, 0x80999900); // Dark Yellow
    setBackgroundPalette(1, L_type, 0x804C9900); // Dark Green
    setBackgroundPalette(1, O_type, 0x80009999); // Dark Turquoise
    setBackgroundPalette(1, T_type, 0x80000099); // Dark Blue
    setBackgroundPalette(1, Z_type, 0x804C0099); // Dark Purple
    setBackgroundPalette(1, 8, 0x00000000);      // Transparent

    backgroundDrawRec(1, merge_arg(0, 0), merge_arg(FULL_X, FULL_Y), 8); // fill with tranparent first
    setBackgroundControl(1, merge_arg(0, 0), 0, 1);
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
    setSpritePalette(0, 8, 0x00000000);      // Transparent

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

    // ---------set digit palettes---------------
    // color palatte_num = 2 for digits
    setSpritePalette(2, 0, 0xFFFFFFFF); // White
    setSpritePalette(2, 8, 0x00000000); // Transparent

    // transparent palette_num = 3 for digits
    setSpritePalette(3, 0, 0x00000000);
    setSpritePalette(3, 8, 0x00000000);

    // -----------end init digit palettes-------------

    game_state = WELCOME_PAGE_STATE;
    setVideoMode(TEXT_MODE);

    while (1)
    {
        global = getTimer();
        if (global != last_global)
        {
            mode = getMode();

            if (mode == TEXT_MODE)
            {
                if (game_state == WELCOME_PAGE_STATE)
                {
                    welcome_page_state();
                }
                else if (game_state == INIT_GAME_STATE)
                {
                    init_game_state(&rotation);
                }
            }
            else if (mode == GRAPHICS_MODE)
            {
                if (game_state == CREATE_BLOCK_STATE)
                {
                    // random generate next block
                    current_block_type = next_block_type;
                    next_block_type = global % 7;
                    if (next_block_type == current_block_type)
                        next_block_type++;
                    if (next_block_type == 7)
                        next_block_type = 0;

                    // visualize current block
                    setBlockControl(current_block_type, FULL_X / 2 - 2 * UNIT, 0, 0);

                    // visualize next blockgame_board
                    setBlockControl(next_block_type, (MARGIN/2)*UNIT, FULL_Y/3, 0);

                    // visualize 2-digit score
                    setDigitControl((score%100)/10+10, FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, 2);
                    setDigitControl(score%10, FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, 2);

                    // reset moving block info
                    block_current_x_idx = game_board_width / 2 - 2; // the middle of the game_board
                    block_current_y_idx = 0;
                    rotation = 0;
                    for (int k = 0; k < 4; k++)
                    {
                        block_nonempty_idx[k] = Block[current_block_type][rotation][k];
                    }

                    game_state = DROP_BLOCK_STATE;
                }
                else if (game_state == DROP_BLOCK_STATE)
                {
                    drop_block_state(current_block_type, &rotation);
                }
                else if (game_state == DRAW_TO_BG_STATE)
                {
                    // draw current block to background
                    int x_idx, y_idx;
                    for (int k = 0; k < 4; k++)
                    {
                        x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
                        y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

                        if (y_idx == 0 || y_idx == 1)
                        {
                            break;
                        }
                        game_board[y_idx][x_idx] = true;
                        backgroundDrawRec(1, merge_arg((x_idx + MARGIN) * UNIT, y_idx * UNIT), merge_arg(UNIT, UNIT), current_block_type);
                    }

                    // check full line
                    full_line_count = 0;
                    bool isFullLine = true;
                    for (int i = game_board_height - 1; i >= 0; i--)
                    {
                        isFullLine = true;
                        for (int j = 0; j < game_board_width; j++)
                        {
                            isFullLine &= game_board[i][j];
                        }
                        if (isFullLine)
                        {
                            fullLine[full_line_count] = i;
                            full_line_count++;
                        }
                    }

                    // set current block to zero-rotation and transparent
                    rotateBlock(current_block_type, 0);
                    setBlockControl(current_block_type, FULL_X / 2 - 2 * UNIT, 0, 1);

                    if (y_idx == 0 || y_idx == 1)
                    {
                        game_state = GAME_OVER_STATE;
                    }
                    else
                    {
                        game_state = DELETE_FULL_LINE_STATE;
                    }
                }
                else if (game_state == DELETE_FULL_LINE_STATE)
                {
                    if (full_line_count == 0)
                    {
                        // no deletion needed
                    }
                    else
                    {
                        total_full_line += full_line_count;

                        // clear 2-digit score
                        setDigitControl((score%100)/10+10, FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, 3);
                        setDigitControl(score%10, FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, 3);

                        // draw 2-digit score
                        setDigitControl((total_full_line%100)/10+10, FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, 2);
                        setDigitControl(total_full_line%10, FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, 2);

                        score = total_full_line;

                        delete_full_line_state();
                    }

                    game_state = CREATE_BLOCK_STATE;
                }
                else if (game_state == GAME_OVER_STATE)
                {
                    // clear game_board array
                    for (int i = 0; i < game_board_height; i++)
                    {
                        for (int j = 0; j < game_board_width; j++)
                        {
                            game_board[i][j] = false;
                        }
                    }

                    // clear gameboard background
                    setVideoMode(TEXT_MODE);
                    backgroundDrawRec(1, merge_arg(0, 0), merge_arg(FULL_X, FULL_Y), 8); // fill with tranparent
                    game_state = WELCOME_PAGE_STATE;
                }
            }
            last_global = global;
        }
        

        videoInt = systemcall(SYSIDEO, 0, 0, 0, 0, 0);
        if (videoInt != last_videoInt)
        {
            border_color = (border_color == 0)? 2 : 0;
            setBackgroundPalette(2, 1, Rand_sys()); // random color
            setBackgroundControl(0, merge_arg(0, 0), 5, border_color); // change border color upon video interrupt
            last_videoInt = videoInt;
        }
    }

    return 0;
}

void welcome_page_state()
{
    controller_status = getStatus();
    if (controller_status)
    {
        if ((x_pos / 0x40 == 30) &&
            ((VIDEO_MEMORY[x_pos] == 'S') ||
             (VIDEO_MEMORY[x_pos] == 'T') ||
             (VIDEO_MEMORY[x_pos] == 'A') ||
             (VIDEO_MEMORY[x_pos] == 'R') ||
             (VIDEO_MEMORY[x_pos] == 'T')))
        {

            // clear 'X'
            if (VIDEO_MEMORY[x_pos] == 'X')
                VIDEO_MEMORY[x_pos] = 0;
            current_game_unit = ((x_pos % 0x40) < 0x20) ? 0 : 1;

            // redraw title
            char *greeting = "TETRISX";
            drawText(greeting, 7, 27, 16);
            x_pos = init_x_pos;

            // switch to game
            game_state = INIT_GAME_STATE;

            // clear 2-digit score
            setDigitControl((score%100)/10+10, FULL_X-(MARGIN-2)*UNIT, FULL_Y/3, 3);
            setDigitControl(score%10, FULL_X-(MARGIN-2-4)*UNIT, FULL_Y/3, 3);

            total_full_line = 0;
            score = 0;
            return;
        }

        if (VIDEO_MEMORY[x_pos] == 'X')
            VIDEO_MEMORY[x_pos] = 0;
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
        if (VIDEO_MEMORY[x_pos] == 0)
            VIDEO_MEMORY[x_pos] = 'X';
    }
}

void init_game_state(int *rotation)
{
    if (current_game_unit == prev_game_unit)
    {
        // do nothing
    }
    else
    {
        // ---------Set units--------------
        if (current_game_unit == 0)
        {
            UNIT = 16;                 // 16 pixels
            BLOCK_SIZE = UNIT * 4;     // a block consists of 4UNIT * 4UNIT
            MARGIN = 10 * (16 / UNIT); // how many amount of UNIT
            game_board_width = (FULL_X / UNIT) - 2 * MARGIN;
            game_board_height = FULL_Y / UNIT;
        }
        else
        {
            UNIT = 8;                  // 8 pixels
            BLOCK_SIZE = UNIT * 4;     // a block consists of 4UNIT * 4UNIT
            MARGIN = 10 * (16 / UNIT); // how many amount of UNIT
            game_board_width = (FULL_X / UNIT) - 2 * MARGIN;
            game_board_height = FULL_Y / UNIT;
        }
        // --------end set units------------

        // -----------Draw grid-------------
        backgroundDrawRec(0, merge_arg(0, 0), merge_arg(FULL_X, FULL_Y), 0); // fill with transparent first

        for (int i = 0; i < FULL_X; i += UNIT)
        {
            backgroundDrawRec(0, merge_arg(i, 0), merge_arg(1, FULL_Y), 2); // vertical lines
        }
        for (int j = 0; j < FULL_Y; j += UNIT)
        {
            backgroundDrawRec(0, merge_arg(0, j), merge_arg(FULL_X, 1), 2); // horizontal lines
        }

        for (int i = (MARGIN + 1) * UNIT; i < FULL_X - (MARGIN * UNIT); i += UNIT)
        {
            for (int j = 0; j < FULL_Y; j += UNIT)
            {
                backgroundDrawRec(0, merge_arg(i, j), merge_arg(1, 1), 1); // grid points
            }
        }

        backgroundDrawRec(0, merge_arg(MARGIN * UNIT, 0), merge_arg(game_board_width * UNIT, UNIT + 1), 1); // top boarder
        backgroundDrawRec(0, merge_arg(MARGIN * UNIT - UNIT, 0), merge_arg(UNIT, FULL_Y), 1);               // left boarder
        backgroundDrawRec(0, merge_arg(FULL_X - (MARGIN * UNIT), 0), merge_arg(UNIT, FULL_Y), 1);           // right boarder

        setBackgroundControl(0, merge_arg(0, 0), 5, border_color); // put grid in front of blocks, blocks(large sprite) are rendered in z-plane 4
        // -----------end draw grid---------------

        // -----------draw blocks-------------
        initBlock(S_type, *rotation, FULL_X / 2 - 2 * UNIT);
        initBlock(I_type, *rotation, FULL_X / 2 - 2 * UNIT);
        initBlock(J_type, *rotation, FULL_X / 2 - 2 * UNIT);
        initBlock(L_type, *rotation, FULL_X / 2 - 2 * UNIT);
        initBlock(O_type, *rotation, FULL_X / 2 - 2 * UNIT);
        initBlock(T_type, *rotation, FULL_X / 2 - 2 * UNIT);
        initBlock(Z_type, *rotation, FULL_X / 2 - 2 * UNIT);
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
        // -----------end draw digits (for 2-digit score)-----------

        // -----------draw score text-----------
        // draw score text pixel. (5rect by 5rect) (1rect is 3pix by 3pix)
        int UNIT_text = 3;
        int32_t rect_start_x = FULL_X-(MARGIN-2)*UNIT + 1;
        int32_t rect_start_y = UNIT*3 + 1;
        for (int rect_id=0; rect_id<5; rect_id++) {
            for (int rect_tmp=0; rect_tmp<25; rect_tmp++) {
                if (Score_text[rect_id][rect_tmp] == 1) {
                    initTextRect(rect_id*25+rect_tmp, rect_start_x + UNIT_text*(rect_tmp%5)+16*rect_id, rect_start_y+UNIT_text*(rect_tmp/5));
                }

            }
        }
        // -----------end draw score text-----------

        prev_game_unit = current_game_unit;
    }

    game_state = CREATE_BLOCK_STATE;
    setVideoMode(GRAPHICS_MODE);
    return;
}

void drop_block_state(int32_t block_type, int *rotation)
{
    controller_status = getStatus();
    if (controller_status)
    {
        if (controller_status & 0x1) // left
        {
            if (checkCollide_X(-UNIT) == false)
            {
                moveSprite(block_type + 128, -UNIT, 0);
                block_current_x_idx--;
            }
        }
        if (controller_status & 0x2) // up
        {
            // TODO: rotation check
            *rotation = (*rotation == 3) ? 0 : *rotation + 1;
            rotateBlock(block_type, *rotation);
        }
        if (controller_status & 0x4) // down
        {
            if (checkCollide_Y() == false)
            {
                moveSprite(block_type + 128, 0, UNIT);
                block_current_y_idx++;
            }
            else
            {
                game_state = DRAW_TO_BG_STATE;
            }
        }
        if (controller_status & 0x8) // right
        {
            if (checkCollide_X(UNIT) == false)
            {
                moveSprite(block_type + 128, UNIT, 0);
                block_current_x_idx++;
            }
        }
    }

    if (global % 10 == 0)
    {
        if (checkCollide_Y() == false)
        {
            moveSprite(block_type + 128, 0, UNIT);
            block_current_y_idx++;
        }
        else
        {
            game_state = DRAW_TO_BG_STATE;
        }
    }

    return;
}

void delete_full_line_state()
{
    uint8_t *DATA = (volatile uint8_t *)(getBACKGROUND_DATA_ADDRESS(1)); // background_num = 1

    // game board
    int k = 0;
    int jump = 0;
    for (int i = game_board_height - 1; i > 0; i--)
    {
        while ((k != full_line_count) && (((jump == 0) && (i == fullLine[k])) || ((i - jump) == fullLine[k])))
        {
            jump++;
            k++;
        }

        // gameboard
        for (int j = 0; j < game_board_width; j++)
        {
            if ((i - jump) < 0)
            {
                game_board[i][j] = false;
            }
            else
            {
                game_board[i][j] = game_board[i - jump][j];
            }
        }

        // background data
        for (int l = 0; l < UNIT; l++)
        {
            for (int j = MARGIN * UNIT; j < (MARGIN + game_board_width + 1) * UNIT; j++)
            {
                if ((i - jump) < 0)
                {
                    DATA[(0x200) * ((i * UNIT) + l) + j] = 8;
                }
                else
                {
                    DATA[(0x200) * ((i * UNIT) + l) + j] = DATA[(0x200) * (((i - jump) * UNIT) + l) + j];
                }
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        fullLine[i] = -1;
    }
    full_line_count = 0;

    return;
}

uint8_t initBlock(uint8_t block_type, uint8_t rotation, int32_t x)
{
    uint32_t sprite_num = block_type + 128;
    uint8_t *DATA = (volatile uint8_t *)(getSPRITE_DATA_ADDRESS(sprite_num));

    // clear to transparent
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            DATA[(i << 6) + j] = 8;
        }
    }

    int sub_block;
    int start_x, start_y;
    for (int k = 0; k < 4; k++)
    {
        sub_block = Block[block_type][rotation][k];
        start_x = (sub_block % 4) * UNIT;
        start_y = (sub_block / 4) * UNIT;

        for (int i = 0; i < UNIT; i++)
        {
            for (int j = 0; j < UNIT; j++)
            {
                DATA[((start_y + i) << 6) + (start_x + j)] = block_type;
            }
        }
    }

    // set sprite control
    uint32_t *CONTROL = (volatile uint32_t *)(getSPRITE_CONTROL_ADDRESS(sprite_num));
    CONTROL[0] = calcLargeSpriteControl(merge_arg(x, 0), merge_arg(BLOCK_SIZE, BLOCK_SIZE), 1); // use transparent palette at initialization

    return block_type + 128;
}

int initDigit(int digit_type, int32_t x, int32_t y) {
    // set large sprite data
    uint32_t sprite_num = digit_type + 128 + 7;
    uint8_t *DATA = (volatile uint8_t *)(getSPRITE_DATA_ADDRESS(sprite_num));

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
    }

    // set large sprite control
    uint32_t *CONTROL = (volatile uint32_t *)(getSPRITE_CONTROL_ADDRESS(sprite_num));
    CONTROL[0] = calcLargeSpriteControl(merge_arg(x, y), merge_arg(BLOCK_SIZE, BLOCK_SIZE), 3); // use transparent palette at initialization

	return digit_type + 128 + 10;
}

int initTextRect(int rect_id, int32_t x, int32_t y) {
    // set small sprite data
    // uint8_t *DATA = (volatile uint8_t *)(getLARGE_SPRITE_DATA_ADDRESS() + (0x1000)*(block_type));
    // uint32_t digit_type_test = digit_type + 30;
    uint8_t *DATA = (volatile uint8_t *)(getSPRITE_DATA_ADDRESS(rect_id));

    int UNIT_tmp = 3;
    for(int i = 0; i < UNIT_tmp; i++) {
        for(int j = 0; j < UNIT_tmp; j++) {
            DATA[((y+i)<<6) + (x+j)] = 0;
        }
    }

    // set small sprite control
    // uint32_t *CONTROL = (volatile uint32_t *)(getLARGE_SPRITE_CONTROL_ADDRESS() + (0x4)*block_type);
    uint32_t *CONTROL = (volatile uint32_t *)(getSPRITE_CONTROL_ADDRESS(rect_id));
    CONTROL[0] = calcSmallSpriteControl(merge_arg(x, y), merge_arg(3, 3), 2); // use transparent palette at initialization

	return rect_id;
}

void rotateBlock(uint8_t block_type, uint8_t rotation)
{
    uint32_t sprite_num = block_type + 128;
    uint8_t *DATA = (volatile uint8_t *)(getSPRITE_DATA_ADDRESS(sprite_num));

    // clear to transparent
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            DATA[(i << 6) + j] = 8;
        }
    }

    int sub_block;
    int start_x, start_y;
    for (int k = 0; k < 4; k++)
    {
        sub_block = Block[block_type][rotation][k];
        start_x = (sub_block % 4) * UNIT;
        start_y = (sub_block / 4) * UNIT;

        for (int i = 0; i < UNIT; i++)
        {
            for (int j = 0; j < UNIT; j++)
            {
                DATA[((start_y + i) << 6) + (start_x + j)] = block_type;
            }
        }
        block_nonempty_idx[k] = sub_block;
    }
}

void setBlockControl(uint8_t block_type, int32_t x, int32_t y, uint8_t palette_num)
{
    // set large sprite control
    uint32_t sprite_num = block_type + 128;
    uint32_t *CONTROL = (volatile uint32_t *)(getSPRITE_CONTROL_ADDRESS(sprite_num));
    CONTROL[0] = calcLargeSpriteControl(merge_arg(x, y), merge_arg(BLOCK_SIZE, BLOCK_SIZE), palette_num); // use transparent palette at initialization

    return;
}

void setDigitControl(uint8_t digit_type, int32_t x, int32_t y, uint8_t palette_num) {
	// set large sprite control
    uint32_t sprite_num = digit_type + 128 + 7;
    uint32_t *CONTROL = (volatile uint32_t *)(getSPRITE_CONTROL_ADDRESS(sprite_num));
    CONTROL[0] = calcLargeSpriteControl(merge_arg(x, y), merge_arg(BLOCK_SIZE, BLOCK_SIZE), palette_num); 

	return;
}

bool checkCollide_X(int32_t d_x)
{
    int x_idx, y_idx;
    if (d_x < 0)
    {
        for (int k = 0; k < 4; k++)
        {
            x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
            y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

            if (x_idx - 1 == -1)
            {
                return true;
            }
            else if (game_board[y_idx][x_idx - 1] == true)
            {
                return true;
            }
        }
    }
    else if (d_x > 0)
    {
        for (int k = 0; k < 4; k++)
        {
            x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
            y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

            if (x_idx + 1 == game_board_width)
            {
                return true;
            }
            else if (game_board[y_idx][x_idx + 1] == true)
            {
                return true;
            }
        }
    }

    return false;
}

bool checkCollide_Y()
{
    int x_idx, y_idx;
    for (int k = 3; k >= 0; k--)
    {
        x_idx = block_current_x_idx + (block_nonempty_idx[k] % 4);
        y_idx = block_current_y_idx + (block_nonempty_idx[k] / 4);

        if (y_idx + 1 == game_board_height)
        {
            return true;
        }
        else if (game_board[y_idx + 1][x_idx] == true)
        {
            return true;
        }
    }

    return false;
}
