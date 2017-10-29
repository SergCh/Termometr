#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_


#include "config.h"

void onewire_init();                                        // init port
__bit onewire_reset();                                      // reset devices and check "exist?"

void onewire_search_init();                                 // need init before search
__bit onewire_search();                                     // check crc in onewire_crc, must be "0"

void onewire_write(unsigned char);                          // write byte
void onewire_write_array(unsigned char *, unsigned char);   // write bytes

unsigned char onewire_read();                               // read byte
__bit onewire_read_array(unsigned char *, unsigned char);   // read bytes, return true if crc==0

void onewire_matchROM(unsigned char * addr);                // select sensor  MATCH ROM

extern unsigned char onewire_addr[8];
extern unsigned char onewire_crc;

#endif




