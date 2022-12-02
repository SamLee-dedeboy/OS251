#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include <stdint.h>
// system call number
#define SYSTIMER 0x00000001
#define CONTROLLER_STATUS 0x00000002
#define MODE_STATUS 0x00000003
#define SMALL_SPRITE_DROP 0x00000004
#define Thread_INIT 0x00000005
#define Thread_SWITCH 0x00000006
#define WRITE_TEXT 0x00000007
// #define MOVE_LARGE_SPRITE 0x00000003
// #define SYSVIDEO 0x00000005

// c interrupt
void thread_timer_scheduel();
void cmd_interrupt();
void video_interrupt();

#endif