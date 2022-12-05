#ifndef SYSTEMCALLANDINTERRUPT_H
#define SYSTEMCALLANDINTERRUPT_H

#include <stdint.h>
// system call number
#define SYSTIMER 0x00000001
#define CONTROLLER_STATUS 0x00000002
#define MODE_STATUS 0x00000003
#define SMALL_SPRITE_DROP 0x00000004
#define READ_MACHINE_TIME 0x00000005
#define READ_MACHINE_PERIOD 0x00000006
#define READ_INT_PENDING_REG 0x00000007
#define RAND 0x00000008

#define THREAD_INITIALLIZE 0x00000017
#define WRITE_TEXT 0x00000014

// c interrupt
void timer_interrupt();
void cmd_interrupt();
void video_interrupt();
void cart_interrupt();
void illegal_inst_interrupt();

#endif