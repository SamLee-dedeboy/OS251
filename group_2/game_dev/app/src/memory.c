#include <string.h>

char *_sbrk(int incr) {
    extern char _heap[];
    extern char _stack[];
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_heap;
    }
    prev_heap_end = heap_end;
    if (heap_end + incr > &_stack) {
        write (1, "Heap and stack collision\n", 25);
        abort ();    
    }
    heap_end += incr;
    return (char *) prev_heap_end;
}

int _write(int fd, void *buf, size_t len) {
    return 0;
}