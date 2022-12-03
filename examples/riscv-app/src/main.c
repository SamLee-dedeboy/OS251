#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define SYSTIMER               0x00000001
#define GET_CONTROLLER_STATUS  0x00000002
#define MOVE_LARGE_SPRITE      0x00000003
#define WRITE_TEXT             0x00000004
#define SYSVIDEO               0x00000005
#define MOVE_SMALL_SPRITE      0x00000006
#define THREAD_INITIALLIZE     0x00000007
#define SET_BG_COLOR           0x00000008


typedef uint32_t (*TCPUContextEntry)(void *param);


uint32_t timer(uint32_t funName);
uint32_t get_controller_status(uint32_t funName);
uint32_t moveLargeSprite(uint32_t funName, int SpriteNum, int x, int y);
uint32_t moveSmallSprite(uint32_t funName, int SpriteNum, int x, int y);
uint32_t my_printf(uint32_t funName, char* text,int variable);
uint32_t video(uint32_t funName);
uint32_t thread_initialize(uint32_t funName,TCPUContextEntry entry, void *param);
uint32_t setBG_color(uint32_t funName, uint32_t color);


//interrput handler variable
uint32_t CurrentGlobal;
uint32_t LastGlobal = 0;
uint32_t CurrentVideo;
uint32_t LastVideo = 0;
uint32_t controller_status;

uint32_t Thread1(void *param){
    while (1) {
        CurrentGlobal = timer(SYSTIMER);
        my_printf(WRITE_TEXT, "Thread: %d\r",CurrentGlobal);
        fflush(stdout);
    }
}



int main() {
    // thread initialize
    void *param;
    thread_initialize(THREAD_INITIALLIZE,Thread1,param);
    // thread initialize
    // demonstrate  malloc 
    int *p = malloc(100); 
    int q=0;
    int *x=&q;
    my_printf(WRITE_TEXT,"address_malloc：    %p\n", p);
    my_printf(WRITE_TEXT,"value_malloc：      %d\n", *p);
    my_printf(WRITE_TEXT,"address_stack：    %p\n", x);
    my_printf(WRITE_TEXT,"value_stack：      %d\n", q);
    *p = 200;
    q =12345678;
    my_printf(WRITE_TEXT,"address_malloc：    %p\n", p);
    my_printf(WRITE_TEXT,"value_malloc：      %d\n", *p);
    my_printf(WRITE_TEXT,"address_stack：    %p\n", x);
    my_printf(WRITE_TEXT,"value_stack：      %d\n", q);
    free(p);
    // demonstrate  malloc end

    while (1) {
        CurrentGlobal = timer(SYSTIMER);
        if(CurrentGlobal != LastGlobal){
            controller_status = get_controller_status(GET_CONTROLLER_STATUS);
            if(controller_status & 0x1){
                moveLargeSprite(MOVE_LARGE_SPRITE,0,-10,0);
                moveSmallSprite(MOVE_SMALL_SPRITE,0,10,0);
            }
            if(controller_status & 0x2){
                moveLargeSprite(MOVE_LARGE_SPRITE,0,0,-10);
                moveSmallSprite(MOVE_SMALL_SPRITE,0,0,10);
            }
            if(controller_status & 0x4){
                moveLargeSprite(MOVE_LARGE_SPRITE,0,0,10);
                moveSmallSprite(MOVE_SMALL_SPRITE,0,0,-10);
            }
            if(controller_status & 0x8){
                moveLargeSprite(MOVE_LARGE_SPRITE,0,10,0);
                moveSmallSprite(MOVE_SMALL_SPRITE,0,-10,0);
            }
            if(controller_status & 0x10){
                setBG_color(SET_BG_COLOR, 0xff00ff00);
            }
            if(controller_status & 0x20){
                setBG_color(SET_BG_COLOR, 0xff00ff00);
            }   
            if(controller_status & 0x40){
                setBG_color(SET_BG_COLOR, 0xffabcdef);
            }   
            if(controller_status & 0x80){
                setBG_color(SET_BG_COLOR, 0xff000000);
            }             
            LastGlobal = CurrentGlobal;
        }
        CurrentVideo = video(SYSVIDEO);
        if(CurrentVideo != LastVideo){
            fflush(stdout);
            moveLargeSprite(MOVE_LARGE_SPRITE,1,10,0);                
            LastVideo = CurrentVideo;
        }
        my_printf(WRITE_TEXT,"Cartridge: %d   ",CurrentGlobal);
        my_printf(WRITE_TEXT,"Timer interrupt: %d   ",CurrentGlobal);
        my_printf(WRITE_TEXT,"Video interrupt: %d",CurrentVideo);
        my_printf(WRITE_TEXT,"\r",CurrentGlobal);
        fflush(stdout);
    }
    return 0;
}


void* _sbrk(int incr) {
  extern char _heapbase;		/* Defined by the linker */
  static char *heap_end;
  char *prev_heap_end;
 
  if (heap_end == 0) {
    heap_end = &_heapbase;
  }
  prev_heap_end = heap_end;
  /*
  if (heap_end + incr > stack_ptr) {
    write (1, "Heap and stack collision\n", 25);
    abort ();
  }
  */
  heap_end += incr;
  return (void*) prev_heap_end;
}