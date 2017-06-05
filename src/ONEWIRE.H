#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_


#include "config.h"

void onewire_init();                            //init port
__bit onewire_reset();                          //reset devices and check "exist?"

void onewire_search_init();                     //need init before search
__bit onewire_search();                         //check crc in onewire_crc, must be "0"

void onewire_write(unsigned char);
void onewire_write_array(unsigned char *, unsigned char);

unsigned char onewire_read();
__bit onewire_read_array(unsigned char *, unsigned char);

//unsigned char onewire_crc8(unsigned char *, unsigned char );     do not use

//__bit onewire_statewire();                                       do not use

void onewire_matchROM(unsigned char * addr);  //select sensor  MATCH ROM
//bit onewire_readROM();                                           do not use

extern unsigned char onewire_addr[8];
extern unsigned char onewire_crc;

#endif




