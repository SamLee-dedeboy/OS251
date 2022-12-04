#include "api.h"

uint32_t Thread(void *param)
{
    int test_num = 0;
    while (1)
    {
        test_num++;

        printtext(WRITE_TEXT, "thread test_num      %d\n", test_num);
    }
}