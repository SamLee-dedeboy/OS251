#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "api.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;
int test_num = 0;

uint32_t Thread(void *param)
{
    int test_num = 0;
    while (1)
    {
        test_num++;
        printtext(WRITE_TEXT, "thread test_num      %d\n", test_num);
    }
}

int main()
{
    void *param;
    thread_init(THREAD_INITIALLIZE, Thread, param);

    int last_global = 42;

    while (1)
    {
        global = getTimer();
        controller_status = getStatus();
        printtext(WRITE_TEXT, "test_num      %d\n", test_num);
    }

    return 0;
}