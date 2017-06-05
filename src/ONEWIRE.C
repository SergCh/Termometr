#include <reg51.h>
#include "onewire.h"
#include "delay41w.h"
#include "config.h"

#define ENABLE_INTERRUPTS  { EA = 1; }
#define DISABLE_INTERRUPTS { EA = 0; }

unsigned char onewire_addr[8];       //public
unsigned char onewire_crc;           //public

signed char tolastprev;

void onewire_init(){
  PIN_OW=1;
}
/*  not need
__bit onewire_statewire(){
  return PIN_OW;
}
*/

__bit onewire_reset(){
  unsigned char i;

  DISABLE_INTERRUPTS

  PIN_OW=0;

  delay480us;

  PIN_OW=1;

  for (i=80; --i; ){
      PIN_OW=1;
      if (!PIN_OW){
          ENABLE_INTERRUPTS
          for (i=120; --i; ){
              PIN_OW=1;
              if (PIN_OW) {
                  return 1;
              }
              delay2us;
          }
          return 0;
      }
  }
  ENABLE_INTERRUPTS
  return 0;
}


__bit crcbit(__bit b){
  onewire_crc = ((onewire_crc & 1) != b) ? (onewire_crc>>1) ^ 0b10001100 : onewire_crc>>1;
  return b;
}

/* not need
 unsigned char onewire_crc8(unsigned char *addr, unsigned char len){
  unsigned char i, j, b;

  for (onewire_crc=i=0; i<len; i++)
    for (j = 0, b = addr[i]; j<8; j++, b >>=1)
      crcbit(b&1);

  return onewire_crc;
}
*/

__bit onewire_readbit(){
  unsigned char i=20;
  __bit r=1;

  DISABLE_INTERRUPTS

  PIN_OW=0;
  delay2us;
  PIN_OW=1;
  delay8us;

  for (;i;i--){
    if (!PIN_OW) r = 0;
  }
  ENABLE_INTERRUPTS

  PIN_OW=1;
  delay60us;
  PIN_OW=1;
  return r;
}

unsigned char onewire_read(){
  unsigned char  r, ibit=1;

  for (r=0; ibit; ibit<<=1){
      if (crcbit(onewire_readbit())){
      r |= ibit;
      }
  }
  return r;
}

__bit onewire_read_array(unsigned char *array, unsigned char len){
  onewire_crc=0;

  for (; len; len--, array++){
      *array = onewire_read();
  }
  return !onewire_crc;
}


void onewire_writebit(__bit b){

  DISABLE_INTERRUPTS

  PIN_OW=0;
  delay2us;

  if (b) PIN_OW=1;

  delay70us;
  PIN_OW=1;

  ENABLE_INTERRUPTS
}

void onewire_write(unsigned char b){
  unsigned char i=8;

  for (; i; --i, b>>=1)
    onewire_writebit(b&1);
}

void onewire_write_array(unsigned char *array, unsigned char len){
  for (; len; len--, array++)
    onewire_write(*array);
}

void onewire_search_init(){
  unsigned char i=8;

  while (i){
    onewire_addr[--i]=0;
  }
  tolastprev=-1;
}

__bit onewire_search(){
  unsigned char *pd = (unsigned char *)onewire_addr;
  signed char lastdef=64;
  signed char i;
  unsigned char ibit;

  if (tolastprev==64) return 0;

  onewire_crc=0;

  if (!onewire_reset()) return 0;
  onewire_write(0xF0);

  for (i=0; i<64; i++, pd++) for (ibit=1; ibit; i++, ibit<<=1){
    unsigned char rd;
    __bit d;

    rd = onewire_readbit()?2:0;
    rd |= onewire_readbit()?1:0;

    switch (rd){
    case 0b00000000:
       if ((i==tolastprev) || (i<tolastprev && (*pd & ibit)==0)) d=0;
       else                                           lastdef=i, d=1;
       break;
    case 0b00000010:
                                                                 d=1;
       break;
    case 0b00000001:
                                                                 d=0;
       break;
    case 0b00000011:
       tolastprev=64;
       return 0; //no device
    }

    crcbit(d);
    onewire_writebit(d);
    if (d) *pd |= ibit;
    else   *pd &= ~ibit;
  }

  tolastprev=lastdef;
  return 1;
}


void onewire_matchROM(unsigned char * addr){  //select sensor MATCH ROM
   onewire_write(0x55);
   onewire_write_array(addr,8);
}

/* not need
__bit onewire_readROM(){
    onewire_write(0x33);
    return onewire_read_array(onewire_addr,8);
}
*/

















































































































































































































