#include "Chip.h"
#include "Thread.h"
#include "Systemcall.h"
#include <stdint.h>
#include <stdio.h>

extern volatile int global;

int main()
{
  while (1)
  {
    if (CARTIDGE_STATUS & CIS)
    {
      volatile uint32_t start_address = (*((volatile uint32_t *)0x4000001C));
      start_address = start_address & 0xfffffffc; // get address
      Tfunc cstart = (Tfunc)start_address;
      cstart();
    }
  }
  return 0;
}

void outbyte(char ch)
{
  volatile char *Textdata = (volatile char *)0x500FE800;
  const uint32_t CharsPerRow = 64, RowsPerScreen = 36;
  static uint32_t Xpos = 0, Ypos = 0;

  switch (ch)
  {
  case '\n':
    Xpos = 0;
    Ypos = (Ypos + 1) % RowsPerScreen;
    break;
  case '\r':
    Xpos = 0;
    break;
  case '\b':
    if (Xpos)
    {
      Xpos--;
    }
    break;
  default:
    Textdata[Ypos * CharsPerRow + Xpos] = ch;
    Xpos++;
    if (CharsPerRow <= Xpos)
    {
      Xpos = 0;
      Ypos = (Ypos + 1) % RowsPerScreen;
    }
    break;
  }
}

int _write(int file, char *ptr, int len)
{
  int todo;

  for (todo = 0; todo < len; todo++)
  {
    outbyte(*ptr++);
  }
  return len;
}

#include <sys/stat.h>
int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

void *_sbrk(int incr)
{
  extern char _heapbase; /* Defined by the linker */
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0)
  {
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
  return (void *)prev_heap_end;
}