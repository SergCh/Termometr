#ifndef DELAY41W_H
#define DELAY41W_H

#include "config.h"

// cristall 22.1184 MGz
// nop=0.5us


#define delay2us    \
    {NOP; NOP; NOP; NOP;    \
    }

#define delay8us    \
    {NOP;   NOP;    \
     NOP;   NOP;    \
     NOP;   NOP;    \
     NOP;   NOP;    \
     NOP;   NOP;    \
     NOP;   NOP;    \
     NOP;   NOP;    \
     NOP;   NOP;}

// for SDCC_mcs51 need anothe delays
// this approximate delays for Franklin software

#define delay480us   delayNOP(222)
#define delay15us    delayNOP(16)
#define delay60us    delayNOP(20)
#define delay70us    delayNOP(20)


void delayNOP(unsigned char /* int*/);  //delay NOP

#endif

