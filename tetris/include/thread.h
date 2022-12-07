#ifndef __INCLUDE_STDINT__
#define __INCLUDE_STDINT__
#include <stdint.h>
#endif

#ifndef THREAD_H
#define THREAD_H

typedef uint32_t (*TContextEntry)(void *param);
uint32_t printtext(uint32_t funName, char *text, int variable);
uint32_t thread_init(uint32_t funName, TContextEntry entry, void *param);


#endif