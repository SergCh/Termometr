
#include <reg51.h>

#include "delay41w.h"
#include "config.h"

void delayNOP(unsigned char /* int*/ us){
    while (--us)
        NOP;
}

