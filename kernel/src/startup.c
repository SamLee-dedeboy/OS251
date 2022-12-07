#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef __INCLUDE_STDIO__
#define __INCLUDE_STDIO__
#include <stdio.h>
#endif

#ifndef __INCLUDE_STDLIB__
#define __INCLUDE_STDLIB__
#include <stdlib.h>
#endif

#include "chip.h"
#include "syscallinterr.h"
#include "thread.h"

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

typedef void (*Tfunc)(void);

volatile uint32_t *smallspritecontrol = (volatile uint32_t *)(0x500FF214);
volatile uint32_t *BGpalette0 = (volatile uint32_t *)0x500FC000;
volatile uint32_t *IER = (volatile uint32_t *)(0x40000000);
volatile uint32_t *IPR = (volatile uint32_t *)(0x40000004);
// Thread variable
uint32_t ThreadStack[9][2048];
TStackRef ThreadPointers[10];
int current_thread_num = 1;
int running_thread_pointer = 0;

volatile int video = 0;

extern volatile int global;
extern volatile uint32_t controller_status;
volatile uint32_t *INT_PENDING_REG = (volatile uint32_t *)(0x40000004);
int color = 1;
int color_counter = 0;

static unsigned int seed = 1;

// Nuber of sprites: 0~127: small sprite; 128~191: large sprite
int small_sprite_count = 0; // max: 128 small sprites
int large_sprite_count = 0; // max: 64 large sprites

void init(void)
{
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while (Base < End)
    {
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while (Base < End)
    {
        *Base++ = 0;
    }

    csr_write_mie(0x888);    // Enable all interrupt soruces
    csr_enable_interrupts(); // Global interrupt enable
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;

    // *IER = *IER | COMMAND_BIT;
    // *IER = *IER | VIDEO_BIT;
    // *IER = *IER | CART_BIT;
}

void c_srand(uint32_t newseed)
{
    seed = (unsigned)newseed & 0x7fffffffU;
}

uint32_t c_rand(void)
{
    seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
    return (uint32_t)seed;
}

void c_interrupt_handler(void)
{
	// CMD control
    if (((*IER) >> 2) && (((*IPR) & 0x4) >> 2))
    {
        if (MODE_CONTROL == 0x1)
            MODE_CONTROL = 0x00000000;
        else if (MODE_CONTROL == 0x0) {
            MODE_CONTROL = 0x00000001;
        }
		(*INT_PENDING_REG) |= ~(1U << 2);
		return;
    }
    // CMD control end
    uint32_t mcause = csr_mcause_read();
    switch (mcause)
    {
    case TIMER_INTERRUPT:
        timer_interrupt();
        break;
    case ILLEGAL_INST_INTERRUPT:
        illegal_inst_interrupt();
        break;
    case EXTERNAL_INTERRUPT:
    {
        // if (*IPR & COMMAND_BIT)
        // {
        //     cmd_interrupt();
        //     return;
        // }
        // if (*IPR & VIDEO_BIT)
        // {
        //     video_interrupt();
        //     return;
        // }
        if (*IPR & CART_BIT)
        {
            cart_interrupt();
            break;
        }
    }
    default:
        break;
    }
    if (*IPR & VIDEO_BIT)
    {
        video_interrupt();
        return;
    }
    (*INT_PENDING_REG) |= ~(1U << 2);
}

void illegal_inst_interrupt()
{
    csr_write_mepc(RESTART_ADDRESS);
}

void timer_interrupt()
{
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH) << 32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare >> 32;
    MTIMECMP_LOW = NewCompare;
    global++;
    if (current_thread_num >= 2)
    {
        uint32_t mepc = csr_mepc_read();
        printf("\n");
        TInterruptState PrevState = CPUHALSuspendInterrupts();
        int t1, t2;
        t1 = running_thread_pointer;
        t2 = (running_thread_pointer + 1) % current_thread_num;
        running_thread_pointer = t2;
        ContextSwitch(&ThreadPointers[t1], ThreadPointers[t2]);
        csr_write_mepc(mepc);
        CPUHALResumeInterrupts(PrevState);
    }
    if (global == 9999)
    {
        global = 0;
    }
}

void cmd_interrupt()
{
    if (MODE_CONTROL & 1)
    {
        MODE_CONTROL = 0x00000000;
    }
    else
    {
        MODE_CONTROL = 0x00000001;
    }
    *IPR = *IPR & COMMAND_BIT;
}
void video_interrupt()
{
    video++;
    if (video >= 1000)
    {
        video = 0;
    }
    *IPR = *IPR & VIDEO_BIT;
}
void cart_interrupt()
{
    if (CARTIDGE_STATUS & CIS)
    {
        *IPR = *IPR & CART_BIT;
    }
    else
    {
        *IPR = *IPR & CART_BIT;
    }
}

int setVideoMode(uint32_t mode)
{
    if (!(mode == 0 || mode == 1))
        return -1;

    uint32_t *VIDEO_MODE = (volatile uint32_t *)(MODE_CONTROL_REGISTER);
    VIDEO_MODE[0] &= ~(0x1);
    VIDEO_MODE[0] |= mode;
    return 1;
}

void setRefreshRate(uint32_t rate)
{
    uint32_t *VIDEO_MODE = (volatile uint32_t *)(MODE_CONTROL_REGISTER);
    VIDEO_MODE[0] &= ~(0xFE);
    VIDEO_MODE[0] |= (rate << 1);
}

int setBackgroundPalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB)
{
    if ((palette_num > 3 || entry_num > 255))
        return -1;

    uint32_t *PALETTE = (volatile uint32_t *)(BACKGROUND_PALLETE_ADDRESS + (0x400) * palette_num + (0x4) * entry_num);
    PALETTE[0] = ARGB;
    return 1;
}

int backgroundDrawRec(uint32_t background_num,
                      uint32_t xy, uint32_t wh,
                      uint32_t colorEntry)
{
    if (background_num < 0 || background_num > 3)
        return -1;

    int32_t x = xy >> 16 & (0x0000FFFF);
    int32_t y = xy & (0x0000FFFF);
    uint32_t w = wh >> 16 & (0x0000FFFF);
    uint32_t h = wh & (0x0000FFFF);

    // set background data
    uint8_t *DATA = (volatile uint8_t *)(BACKGROUND_DATA_ADDRESS + (0x24000) * background_num);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            DATA[(0x200) * (y + i) + (x + j)] = colorEntry;
        }
    }

    return 1;
}

int setBackgroundControl(uint32_t background_num, uint32_t xy, uint32_t z, uint32_t palette_num)
{
    if (background_num < 0 || background_num > 3)
        return -1;
    if (palette_num < 0 || palette_num > 3)
        return -1;

    int32_t x = xy >> 16 & (0x0000FFFF);
    int32_t y = xy & (0x0000FFFF);

    // set background control
    uint32_t *CONTROL = (volatile uint32_t *)(BACKGROUND_CONTROL_ADDRESS + (0x4) * background_num);
    CONTROL[0] = ((z) << 22) | ((y + 288) << 12) | ((x + 512) << 2) | palette_num;

    return 1;
}

int changeBackgroundPalette(uint32_t background_num, uint32_t palette_num)
{
    if (background_num < 0 || background_num > 3)
        return -1;
    if (palette_num < 0 || palette_num > 3)
        return -1;

    uint32_t *CONTROL = (volatile uint32_t *)(BACKGROUND_CONTROL_ADDRESS + (0x4) * background_num);
    CONTROL[0] &= 0xFFFFFFFC;
    CONTROL[0] |= palette_num;

    return 1;
}

int setSpritePalette(uint32_t palette_num, uint32_t entry_num, uint32_t ARGB)
{
    if (palette_num > 3 || entry_num > 255)
        return -1;

    uint32_t *PALETTE = (volatile uint32_t *)(SPRITE_PALLETE_ADDRESS + (0x400) * palette_num + (0x4) * entry_num);
    PALETTE[0] = ARGB;
    return 1;
}

uint32_t calcSmallSpriteControl(uint32_t xy, uint32_t wh, uint32_t p)
{
    uint32_t z = 4;
    int32_t x = xy >> 16 & (0x0000FFFF);
    int32_t y = xy & (0x0000FFFF);
    uint32_t w = wh >> 16 & (0x0000FFFF);
    uint32_t h = wh & (0x0000FFFF);
    return ((z) << 29) | ((h - 1) << 25) | ((w - 1) << 21) | ((y + 16) << 12) | ((x + 16) << 2) | p;
}

uint32_t calcLargeSpriteControl(uint32_t xy, uint32_t wh, uint32_t p)
{
    int32_t x = xy >> 16 & (0x0000FFFF);
    int32_t y = xy & (0x0000FFFF);
    uint32_t w = wh >> 16 & (0x0000FFFF);
    uint32_t h = wh & (0x0000FFFF);
    return ((h - 33) << 26) | ((w - 33) << 21) | ((y + 64) << 12) | ((x + 64) << 2) | p;
}

uint16_t createRecSprite(uint32_t xy, uint32_t wh, uint32_t palette_num, uint32_t colorEntry)
{
    int16_t num;
    int32_t x = xy >> 16 & (0x0000FFFF);
    int32_t y = xy & (0x0000FFFF);
    uint32_t w = wh >> 16 & (0x0000FFFF);
    uint32_t h = wh & (0x0000FFFF);

    if (w < 16 && h < 16)
    { // create small sprite
        if (small_sprite_count >= 128)
            small_sprite_count = 0;
        num = small_sprite_count;

        // set sprite data
        uint8_t *DATA = (volatile uint8_t *)(SMALL_SPRITE_DATA_ADDRESS + (0x100) * small_sprite_count);
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                DATA[(i << 6) + j] = (i < h && j < w) ? colorEntry : 0;
            }
        }

        // set sprite control
        uint32_t *CONTROL = (volatile uint32_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x4) * small_sprite_count);
        CONTROL[0] = calcSmallSpriteControl(xy, wh, palette_num);

        small_sprite_count++;
    }
    else
    { // create large sprite
        if (large_sprite_count >= 64)
            large_sprite_count = 0;
        num = 128 + large_sprite_count;

        // set sprite data
        uint8_t *DATA = (volatile uint8_t *)(LARGE_SPRITE_DATA_ADDRESS + (0x1000) * large_sprite_count);
        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                DATA[(i << 6) + j] = (i < h && j < w) ? colorEntry : 0;
            }
        }

        // set sprite control
        uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4) * large_sprite_count);
        CONTROL[0] = calcLargeSpriteControl(xy, wh, palette_num);

        large_sprite_count++;
    }

    return num;
}

int moveSprite(uint32_t sprite_num, uint32_t d_x, uint32_t d_y)
{
    if (sprite_num < 0 || sprite_num > 191)
        return -1;

    uint32_t x, y;

    if (sprite_num < 128)
    { // small sprite
        uint32_t *CONTROL = (volatile uint32_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x4) * sprite_num);
        x = (CONTROL[0] & 0x7FE) >> 2;
        y = (CONTROL[0] & 0x1FF000) >> 12;

        x += d_x;
        y += d_y;

        CONTROL[0] &= ~(0X1FFFFC); // clear out original x, y bits
        CONTROL[0] |= (x << 2);
        CONTROL[0] |= (y << 12);
    }
    else
    { // large sprite
        uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4) * (sprite_num - 128));
        x = (CONTROL[0] & 0x7FE) >> 2;
        y = (CONTROL[0] & 0x1FF000) >> 12;

        x += d_x;
        y += d_y;

        CONTROL[0] &= ~(0X1FFFFC);
        CONTROL[0] |= (x << 2);
        CONTROL[0] |= (y << 12);
    }
    return 1;
}

int changeSpritePalette(uint32_t sprite_num, uint32_t palette_num)
{
    if (sprite_num < 0 || sprite_num > 191)
        return -1;
    if (palette_num < 0 || palette_num > 3)
        return -1;

    if (sprite_num < 128)
    { // small sprite
        uint32_t *CONTROL = (volatile uint32_t *)(SMALL_SPRITE_CONTROL_ADDRESS + (0x4) * sprite_num);
        CONTROL[0] &= 0xFFFFFFFC;
        CONTROL[0] |= palette_num;
    }
    else
    { // large sprite
        uint32_t num = sprite_num - 128;
        uint32_t *CONTROL = (volatile uint32_t *)(LARGE_SPRITE_CONTROL_ADDRESS + (0x4) * num);
        CONTROL[0] &= 0xFFFFFFFC;
        CONTROL[0] |= palette_num;
    }

    return 1;
}

int drawText(char *text, uint32_t length, uint32_t x, uint32_t y)
{
    // ranges: x = 0~63; y = 0~35
    if (x >= 64 || y >= 36)
        return -1; // position out of range

    char *TEXT_DATA = (volatile char *)(TEXT_DATA_ADDRESS);
    for (int i = 0; i < length; i++)
    {
        int index = y * (0x40) + x + i;

        if (index >= 64 * 36)
            continue;

        TEXT_DATA[index] = text[i];
    }

    return 1;
}

void clearTextScreen()
{
    char *TEXT_DATA = (volatile char *)(TEXT_DATA_ADDRESS);
    for (int i = 0; i < 0x900; i++)
    {
        TEXT_DATA[i] = 0;
    }

    return;
}

uint32_t c_system_call(uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5, char *param6)
{
    switch (param1)
    {
    case SYSTIMER:
        return global;
        break;

    case CONTROLLER_STATUS:
        return CONTROLLER;
        break;

    case ENABLE_CMD:
		// (*IER) |= ~(1U << 2);
        (*IER) |= 1UL << 2;
        return 1;
        break;
    case DISABLE_CMD:
		(*IER) &= ~(1UL << 2);
        // number &= ~(1UL << n);

        return 1;
        break;

    case MODE_STATUS:
        if (MODE_CONTROL & 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;

    case SMALL_SPRITE_DROP:
        smallspritecontrol[0] += 0x00001000;
        return 1;
        break;

    case READ_MACHINE_TIME:
        return MACHINE_TIME_REGISTER;

    case READ_MACHINE_PERIOD:
        return MACHINE_PERIOD_REGISTER;

    case READ_INT_PENDING_REG:
        return INT_PENDING_REG;

    case RAND:
        c_srand(MACHINE_TIME_REGISTER);
        return c_rand();

    case get_BACKGROUND_DATA_ADDRESS:
        return BACKGROUND_DATA_ADDRESS + (0x24000) * param2;

    case get_LARGE_SPRITE_DATA_ADDRESS:
        return LARGE_SPRITE_DATA_ADDRESS + (0x1000) * param2;

    case get_SMALL_SPRITE_DATA_ADDRESS:
        return SMALL_SPRITE_DATA_ADDRESS + (0x100) * param2;

    case get_BACKGROUND_PALLETE_ADDRESS:
        return BACKGROUND_PALLETE_ADDRESS + (0x400) * param2 + (0x4) * param3;

    case get_SPRITE_PALLETE_ADDRESS:
        return SPRITE_PALLETE_ADDRESS + (0x400) * param2 + (0x4) * param3;

    case get_TEXT_DATA_ADDRESS:
        return TEXT_DATA_ADDRESS;

    case get_BACKGROUND_CONTROL_ADDRESS:
        return BACKGROUND_CONTROL_ADDRESS + (0x4) * param2;

    case get_LARGE_SPRITE_CONTROL_ADDRESS:
        return LARGE_SPRITE_CONTROL_ADDRESS + (0x4) * param2;

    case get_SMALL_SPRITE_CONTROL_ADDRESS:
        return SMALL_SPRITE_CONTROL_ADDRESS + (0x4) * param2;

    case get_MODE_CONTROL_REGISTER:
        return MODE_CONTROL_REGISTER;

    case VideoMode:
        return setVideoMode(param2);

    case RefreshRate:
        setRefreshRate(param2);
        break;

    case Merge:
        return (param2 << 16) | (param3);

    case setBackgroundPalette_id:
        return setBackgroundPalette(param2, param3, param4);

    case backgroundDrawRec_id:
        return backgroundDrawRec(param2, param3, param4, param5);

    case setBackgroundControl_id:
        return setBackgroundControl(param2, param3, param4, param5);

    case changeBackgroundPalette_id:
        return changeBackgroundPalette(param2, param3);

    case setSpritePalette_id:
        return setSpritePalette(param2, param3, param4);

    case createRecSprite_id:
        return createRecSprite(param2, param3, param4, param5);

    case moveSprite_id:
        return moveSprite(param2, param3, param4);

    case changeSpritePalette_id:
        return changeSpritePalette(param2, param3);

    case calcSmallSpriteControl_id:
        return calcSmallSpriteControl(param2, param3, param4);

    case calcLargeSpriteControl_id:
        return calcLargeSpriteControl(param2, param3, param4);

    case drawText_id:
        return drawText(param6, param2, param3, param4);

    case clearTextScreen_id:
        clearTextScreen();
        break;

    case SYSVIDEO:
        return video;
        break;

    case WRITE_TEXT:
        printf((char *)param2, (int)param3);
        fflush(stdout);
        break;

    case THREAD_INITIALLIZE:
        if (current_thread_num <= 10)
        {
            ThreadPointers[current_thread_num] = ContextInitialize((TStackRef)(ThreadStack[current_thread_num - 1] + 2048), (TContextEntry)param2, (void *)param3);
            current_thread_num++;
        }
        break;

    default:
        break;
    }
}
