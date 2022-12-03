#ifndef SYSTEMCALLANDINTERRUPT_H
#define SYSTEMCALLANDINTERRUPT_H

#include <stdint.h>
// system call number
#define SYSTIMER 0x00000001
#define GET_CONTROLLER_STATUS 0x00000002
#define MOVE_LARGE_SPRITE 0x00000003
#define WRITE_TEXT 0x00000004
#define SYSVIDEO 0x00000005
#define MOVE_SMALL_SPRITE 0x00000006
#define THREAD_INITIALLIZE 0x00000007
#define SET_BG_COLOR 0x00000008

// c interrupt
void timer_interrupt();
void cmd_interrupt();
void video_interrupt();
void cart_interrupt();
void illegal_inst_interrupt();
void setBGcolor(uint32_t color);

#endif