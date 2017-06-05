#ifndef CONFIG_H
#define CONFIG_H

#define SHOW_HELLO      /*for show HELLO*/
////////#define WITH_BLINK      /*blink number senser and count of sensers*/
//#define DISPLAY_HEX

#define FCPU                22118400
#define PERIOD              (250)   /*timer0 mode=2*/
#define DISPLAYHZ           150

#define COUNT_CYCLES_1DS                (unsigned int)(FCPU/(120*PERIOD))
#define MAX_PERIOD_BLINK                COUNT_CYCLES_1DS
#define COUNT_CYCLES_UPDATE_DISPLAY     (unsigned char) COUNT_CYCLES_1DS*10/DISPLAYHZ


#define MAX_SENSORS    4

#if defined(__SDCC_mcs51)
#  define NOP    __asm__("nop")
#else
#  include <intrins.h>
#  define __bit  bit
#  define NOP   _nop_()
#  define __code  code
#  define __interrupt  interrupt
#  define __using  using
   sbit P3_0=P3 ^ 0;
   sbit P3_1=P3 ^ 1;
   sbit P3_2=P3 ^ 2;
   sbit P3_3=P3 ^ 3;
   sbit P3_4=P3 ^ 4;
   sbit P3_5=P3 ^ 5;
   sbit P3_6=P3 ^ 6;
   sbit P3_7=P3 ^ 7;
#endif


#define PIN_OW      P3_0    /*termosensor*/

#define CATODPORT   P3
#define ANODPORT    P1


#define PININDICATOR0   P3_2
#define PININDICATOR1   P3_3
#define PININDICATOR2   P3_4


#define SGM_A       (1<<6)          /* p1.6     -A-         */
#define SGM_B       (1<<7)          /* p1.7   |     |       */
#define SGM_C       (1<<1)          /* p1.1   F     B       */
#define SGM_D       (1<<2)          /* p1.2   |     |       */
#define SGM_E       (1<<3)          /* p1.3     -G-         */
#define SGM_F       (1<<5)          /* p1.5   |     |       */
#define SGM_G       (1<<4)          /* p1.4   E     C       */
#define SGM_DP      0               /* NC     |     |       */
                                    /*          -D-    [DP] */


#endif

