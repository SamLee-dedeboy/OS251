#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "video_api.h"


volatile int global = 42;
volatile int dup_global = 42;
volatile int last_char = 0;
volatile uint32_t controller_status = 0;

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);

int main() {
    int a = 4;
    int b = 12;
    int last_global = 42;
    int x_pos = 25;
    int last_pos;

    VIDEO_MEMORY[0] = 'C';
    VIDEO_MEMORY[1] = 'u';
    VIDEO_MEMORY[2] = 'r';
    VIDEO_MEMORY[3] = 'r';
    VIDEO_MEMORY[4] = 'e';
    VIDEO_MEMORY[5] = 'n';
    VIDEO_MEMORY[6] = 't';
    VIDEO_MEMORY[7] = ' ';
    VIDEO_MEMORY[8] = 'T';
    VIDEO_MEMORY[9] = 'i';
    VIDEO_MEMORY[10] = 'c';
    VIDEO_MEMORY[11] = 'k';
    VIDEO_MEMORY[12] = ':';
    VIDEO_MEMORY[13] = ' ';
    
    VIDEO_MEMORY[14] = 'X';


    while (1) {
        global = getTicks();
        dup_global = global;
        last_pos = 50;
        
        while(dup_global > 0)
        {
        	last_char = dup_global%10;
        	last_char += 48;
        	VIDEO_MEMORY[last_pos--] = last_char;
        	dup_global = dup_global/10;
        }
        
        if(global == 500){
            char helloWorldStr [] = "Hello World!";
            uint32_t str_len = 12;
            PrintString(str_len, (uint32_t *) helloWorldStr);
        }
        
        if(global == 100){
            uint32_t len = 25;
            char str [] = "Press Any Key to Continue";
            PrintString(len, (uint32_t *)str);

            int key_val = keyEvent();

            len = 25;
            char str1 []  = "                          ";
            PrintString(len, (uint32_t *) str1); 

            len = 17;
            char output [17] = "Button pressed: ";
            switch(key_val){
                case(97):
                    output[16] = 'a';
                    break;
                case(119):
                    output[16] = 'w';
                    break;
                case(120):
                    output[16] = 'x';
                    break;
                case(100):
                    output[16] = 'd';
                    break;
                case(117):
                    output[16] = 'u';
                    break;
                case(105):
                    output[16] = 'i';
                    break;
                case(106):
                    output[16] = 'j';
                    break;
                case(107):
                    output[16] = 'k';
            }
            PrintString(len, (uint32_t *) output);
            
        }
        
        if(global != last_global){
            controller_status = getStatus();
            if(controller_status){
                VIDEO_MEMORY[x_pos] = ' ';
                if(controller_status & 0x1){
                    if(x_pos & 0x3F){
                        x_pos--;
                    }
                }
                if(controller_status & 0x2){
                    if(x_pos >= 0x40){
                        x_pos -= 0x40;
                    }
                }
                if(controller_status & 0x4){
                    if(x_pos < 0x8C0){
                        x_pos += 0x40;
                    }
                }
                if(controller_status & 0x8){
                    if((x_pos & 0x3F) != 0x3F){
                        x_pos++;
                    }
                }
                VIDEO_MEMORY[x_pos] = 'X';
            }
            last_global = global;
        }
    }
    
    return 0;
}