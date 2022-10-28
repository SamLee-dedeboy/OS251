#include <stdint.h>

volatile int global = 42;
volatile uint32_t controller_status = 0;

uint32_t getTicks(void);
uint32_t getStatus(void);
uint32_t getMode(void);
uint32_t getColor(void);

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);
volatile char *GRAPHIC_CONTROL = (volatile char *)(0x50000000 + 0xFF100);
volatile char *GRAPHIC_DATA = (volatile char *)(0x50000000);
volatile char *PALETTE_DATA = (volatile char *)(0x50000000 + 0xFC000);
volatile char *MODE_CONTROL = (volatile char *)(0x50000000 + 0xFF414);
volatile uint32_t *INTERRUPT_ENABLE = (volatile uint32_t *)(0x40000000);
volatile uint32_t *INTERRUPT_PENDING = (volatile uint32_t *)(0x40000004);

int main() {
    int a = 4;
    int b = 12;
    int last_global = 42;
    int x_pos = 12; // for text
    int init_pos = 0; // for graphics
    int mode = 0;
    int counter = 0;
    int width = 10;
    int height = 10;
    int color = 1;


    MODE_CONTROL[0] = 0x01; //LSB
    MODE_CONTROL[1] = 0x00;
    MODE_CONTROL[2] = 0x00;
    MODE_CONTROL[3] = 0x00;

    PALETTE_DATA[0] = 0x00; 
    PALETTE_DATA[1] = 0x00;  
    PALETTE_DATA[2] = 0x00; 
    PALETTE_DATA[3] = 0xFF; //black 

    PALETTE_DATA[4] = 0x00; // B
    PALETTE_DATA[5] = 0x00; // G
    PALETTE_DATA[6] = 0xFF; // R
    PALETTE_DATA[7] = 0xFF; // A //red

    PALETTE_DATA[8] = 0xFF; // B
    PALETTE_DATA[9] = 0x00; // G
    PALETTE_DATA[10] = 0x00; // R
    PALETTE_DATA[11] = 0xFF; // A //blue

    // We can set a palette all zero as a clear function by switching palette number. (or creating animation, pre-draw frames.)
    GRAPHIC_CONTROL[0] = 0x00;
    GRAPHIC_CONTROL[1] = 0x08;
    GRAPHIC_CONTROL[2] = 0x12;
    GRAPHIC_CONTROL[3] = 0x00; //0000 0000 00 01 0010 0000 1000 0000 00 00

    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            GRAPHIC_DATA[init_pos+j + 512*i] = color;
        }
    }

    VIDEO_MEMORY[0] = 'H';
    VIDEO_MEMORY[1] = 'e';
    VIDEO_MEMORY[2] = 'l';
    VIDEO_MEMORY[3] = 'l';
    VIDEO_MEMORY[4] = 'o';
    VIDEO_MEMORY[5] = ' ';
    VIDEO_MEMORY[6] = 'W';
    VIDEO_MEMORY[7] = 'o';
    VIDEO_MEMORY[8] = 'r';
    VIDEO_MEMORY[9] = 'l';
    VIDEO_MEMORY[10] = 'd';
    VIDEO_MEMORY[11] = '!';
    VIDEO_MEMORY[12] = 'X';

    INTERRUPT_ENABLE[0] = 0x07;
    INTERRUPT_PENDING[0] = 0xFF;

    while (1) {
        global = getTicks();
        mode = getMode();
        color = getColor();
        if(global != last_global){
            controller_status = getStatus();
            if (mode==0){
                if(controller_status){
                    VIDEO_MEMORY[x_pos] = ' ';
                    if(controller_status & 0x1){
                        if(x_pos & 0x3F){
                            x_pos--;
                            // VIDEO_MEMORY[x_pos] = 'A';
                            // VIDEO_MEMORY[x_pos] = '0' + global%10;
                        }
                    }
                    else if(controller_status & 0x2){
                        if(x_pos >= 0x40){
                            x_pos -= 0x40;
                            // VIDEO_MEMORY[x_pos] = 'W';
                            // VIDEO_MEMORY[x_pos] = '0' + global%10;
                        }
                    }
                    else if(controller_status & 0x4){
                        if(x_pos < 0x8C0){
                            x_pos += 0x40;
                            // VIDEO_MEMORY[x_pos] = 'X';
                            // VIDEO_MEMORY[x_pos] = '0' + global%10;
                        }
                    }
                    else if(controller_status & 0x8){
                        if((x_pos & 0x3F) != 0x3F){
                            x_pos++;
                            // VIDEO_MEMORY[x_pos] = 'D';
                            // VIDEO_MEMORY[x_pos] = '0' + global%10;
                        }
                    }
                    VIDEO_MEMORY[x_pos] = 'X';
                }
            }
            else if (mode==1){
                if (('0' + global%10)=='9'){
                    counter += 1;
                    if (counter % 1 == 0){
                        if (mode==1){
                            for (int i=0; i<height; i++){
                                for (int j=0; j<width; j++){
                                    GRAPHIC_DATA[init_pos+j + 512*i] = 0;
                                }
                            }
                            if(init_pos/512 <288-10-10){
                                init_pos += 512*10;
                                // VIDEO_MEMORY[x_pos] = 'X';
                            }
                            else {
                                init_pos = init_pos%512;
                            }
                            for (int i=0; i<height; i++){
                                for (int j=0; j<width; j++){
                                    GRAPHIC_DATA[init_pos+j + 512*i] = color;
                                }
                            }
                        }
                    }
                }
                if(controller_status){
                    for (int i=0; i<height; i++){
                        for (int j=0; j<width; j++){
                            GRAPHIC_DATA[init_pos+j + 512*i] = 0;
                        }
                    }
                    if(controller_status & 0x1){
                        if(init_pos%512>10){
                            init_pos-=10;
                            // VIDEO_MEMORY[x_pos] = 'A';
                        }
                    }
                    else if(controller_status & 0x2){
                        if(init_pos/512 >10){
                            init_pos -= 512*10;
                            // VIDEO_MEMORY[x_pos] = 'W';
                        }
                    }
                    else if(controller_status & 0x4){
                        if(init_pos/512 <288-10-10){
                            init_pos += 512*10;
                            // VIDEO_MEMORY[x_pos] = 'X';
                        }
                    }
                    else if(controller_status & 0x8){
                        if(init_pos%512 <512-10-10){
                            init_pos+=10;
                            // VIDEO_MEMORY[x_pos] = 'D';
                        }
                    }
                    for (int i=0; i<height; i++){
                        for (int j=0; j<width; j++){
                            GRAPHIC_DATA[init_pos+j + 512*i] = color;
                        }
                    }
                }
            }
            last_global = global;
        }
    }
    return 0;
}

