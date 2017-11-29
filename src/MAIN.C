#include <reg51.h>

#include "config.h"
#include "onewire.h"
#include "symbols.h"

#define   DELAY_DISPLAY_OF_TEMP    5               // delay of display temperature in ds
#define   DELAY_DISPLAY_OF_NUMB    10              // delay of display number of sensor in ds
#define   DELAY_DISPLAY_OF_SENSOR  50              // delay of display sensor, must biger then DELAY_DISPLAY_OF_TEMP
#define   TIMES_DISPLAY_OF_SENSOR  ((int)DELAY_DISPLAY_OF_SENSOR/DELAY_DISPLAY_OF_TEMP)

#ifdef DISPLAY_HEX
#  define SYMBOLS_HEX       ,SYMBOL_A,SYMBOL_B,SYMBOL_C,SYMBOL_D,SYMBOL_E,SYMBOL_F
#else
#  define SYMBOLS_HEX
#endif

__code const unsigned char numbers[]=   {SYMBOL_0,SYMBOL_1,SYMBOL_2,SYMBOL_3,SYMBOL_4,
                                         SYMBOL_5,SYMBOL_6,SYMBOL_7,SYMBOL_8,SYMBOL_9
                                         SYMBOLS_HEX};

unsigned char symbols[]={SYMBOL__, SYMBOL_EMPTY, SYMBOL_EMPTY};

#define   SETdisplay(S0,S1,S2) {symbols[0]=S0;           symbols[1]=S1;           symbols[2]=S2;}
#define   SETdisplay3MINUS     {symbols[0]=SYMBOL_MINUS; symbols[1]=SYMBOL_MINUS; symbols[2]=SYMBOL_MINUS;}
#define   SETdisplay_N_(N)     {symbols[0]=SYMBOL__;     symbols[1]=numbers[N];   symbols[2]=SYMBOL__;}
#define   SETdisplay_non       {symbols[0]=SYMBOL_n;     symbols[1]=SYMBOL_o;     symbols[2]=SYMBOL_n;}
#define   SETdisplay_Err       {symbols[0]=SYMBOL_E;     symbols[1]=SYMBOL_r;     symbols[2]=SYMBOL_r;}
#ifdef DISPLAY_HEX             // for debug
#  define SETdisplayHEX(N,I)   {symbols[0]=numbers[N>>4]; symbols[1]=numbers[N & 0x0F]; symbols[2]=numbers[I];}
#endif

__bit DISPLAYED=1;

#ifdef WITH_BLINK
__bit BLINKED=0;        //  switch in period DISPLAYED
unsigned int period_blink;
#  define BLINKON           {period_blink=0;    BLINKED=1;}
#  define BLINKOFF          {                   BLINKED=0;}
#else
#  define BLINKON
#  define BLINKOFF
#endif

#define ON_DISPLAY_START_BLINK      {BLINKON    DISPLAYED=1;}
#define ON_DISPLAY_NO_BLINK         {BLINKOFF   DISPLAYED=1;}
#define OFF_DISPLAY                 {BLINKOFF   DISPLAYED=0;}

unsigned int timer_;        // for delayds NEED ADD "volatile"!!!
//=========================================================================
void timer0(void) __interrupt 1  __using 2 {             // interrupt timer0
static unsigned char currsymbol=0;
static unsigned char timer150=0;

    ++timer_;                                            // increment any interrupt

#   ifdef WITH_BLINK
    if (BLINKED){
        if (!period_blink--){
           period_blink=MAX_PERIOD_BLINK;
           DISPLAYED= !DISPLAYED;
        }
    }
#   endif

    if (timer150--){
        return;
    }

    timer150=COUNT_CYCLES_UPDATE_DISPLAY;               // 150Hz

    if (DISPLAYED) {
        unsigned char S=symbols[currsymbol];            // on segments fo currsymbol NEED ADD "register"!!!

        PININDICATOR0 = 0;                              // hide
        PININDICATOR1 = 0;
        PININDICATOR2 = 0;

        ANODPORT = S;                                   // symbol to port

        switch (currsymbol){                            // show symbol
        case 0: PININDICATOR0 = 1; break;
        case 1: PININDICATOR1 = 1; break;
        case 2: PININDICATOR2 = 1; break;
        }

        if (!currsymbol--) currsymbol=2;

    } else {
        PININDICATOR0 = 0;
        PININDICATOR1 = 0;
        PININDICATOR2 = 0;
    }
}

//==========================================================================
void delayds(unsigned char s){                          // delay in decisenods
    while(s--){                                         // wait
        for(timer_=0; timer_<COUNT_CYCLES_1DS; ) ;      // empty cycle MAY BE ADD IDLE MODE? !!!
    }
}

unsigned char sensors_addr[MAX_SENSORS][8];             // address of sensors

#define CONFIG4B  0B00011111                            // 9bit
#define TH4B      0x7F
#define TL4B      0x80

unsigned char sensor_config[9];                         // config of sensor

//==========================================================================
__bit read_sensor_config_loop(unsigned char *addr){     // read config in loop
    unsigned char i=3;                                  // count of try
    do {

        if (!onewire_reset()){                          // reset
                break;
         }
        onewire_matchROM(addr);                         // select sensor

        onewire_write(0xBE);                            // read config
        if(onewire_read_array(sensor_config,9))         // check crc
            return 1;                                   // good :)

    }while (i--);

    return 0;                                           // bad :(
}

#ifdef SHOW_HELLO
__code const unsigned char HELLO[]= {SYMBOL_H, SYMBOL_E, SYMBOL_L, SYMBOL_L, SYMBOL_0, SYMBOL_EMPTY, SYMBOL_EMPTY, SYMBOL_EMPTY};
#endif

//==========================================================================
void main(){

    typedef union {
        signed int    s16;
        unsigned char u8[2];
    } S16_U8;

    unsigned char i;    //temporaly variables
    unsigned char n=0;  //number of sensors
    unsigned char times;//count of display temperature of one sensor
    __bit state=1;      //true-search sensors, false-read configs

    P3=0;               //init ports
    P1=0;

    TL0= -PERIOD;       //init timer0  set init value
    TH0= -PERIOD;       //             set reload value
    TMOD &= 0xF0;
    TMOD |= 0x02;       //             select mode 2
    ET0   = 1;          //             enable timer 0
    TR0   = 1;          //timer 0 run
    EA    = 1;          //global interrupt enable

    onewire_init();

#   ifdef SHOW_HELLO
    for (i=0; i<sizeof(HELLO);i++){       //show HELLO for fun :)
        symbols[0]=symbols[1];
        symbols[1]=symbols[2];
        symbols[2]=HELLO[i];
        delayds(3);
    }
#   endif

    while (1){                                      //main loop!!!!!
        if (state){                                 //search sensors

            i=0;
            onewire_search_init();                  //init 1w
            do {
                if(!onewire_search())               //search sensor
                    break;                          //no more sensors

                if (onewire_crc){                   //may be bad channel
                    i=0;                            //restart search
                    break;
                }

                if (((*onewire_addr)!=0x10)     &&  //ds18s20
                    ((*onewire_addr)!=0x28)     &&  //ds18b20
                    ((*onewire_addr)!=0x22))        //ds1822
                    continue;                       //ignore sensor but it is not thermosensor

                onewire_reset();                    //restart 1w and select new sensor
                onewire_matchROM(onewire_addr);

                onewire_write(0x4E);                //write config
                onewire_write(TH4B);
                onewire_write(TL4B);

                if (*onewire_addr != 0x10) {        //for new sensors
                    onewire_write(CONFIG4B);
                }

                {
                register unsigned char iii;
                    for (iii=8; iii--; )            //save address new sensor
                        sensors_addr[i][iii]=onewire_addr[iii];
                }
                i++;                                //next sensor
            }while (i<MAX_SENSORS);

            n=i;                                    //in "n" count of connected sensors

            SETdisplay_N_(n);                       //display count of sensors
            ON_DISPLAY_START_BLINK;

            delayds(10);

            if (n>0) {
                state=0;                            //go to read config

                SETdisplay3MINUS;
                ON_DISPLAY_NO_BLINK;
                i=0;
            }
        }else{                                      //read configs

            if (n>1){                               //display number of sensor if more then 1
                SETdisplay_N_(i+1);
                delayds(DELAY_DISPLAY_OF_NUMB);
            }

            for (times=TIMES_DISPLAY_OF_SENSOR;--times;){

                if (!onewire_reset()){                  //reset 1w
                    SETdisplay_Err;                     //show error 2seconds end restart search
                    delayds(20);
                    state=1;
                    break;
                }
                onewire_matchROM(sensors_addr[i]);      //select sensor[i]

                onewire_write(0x44);                    //start "Convert"
                delayds(1);                             //wait convert

                if (read_sensor_config_loop(sensors_addr[i])){ //read config
                    unsigned char signdes=0;                   //0b000000ds s-sign d-tens or 0b00000100 to hunded
                    unsigned char units;                       //units
                    unsigned char tens;                        //tens
                    S16_U8 t;                                  //temperature

#if defined(__SDCC_mcs51)                                      //direct or reverse order need to remove to config
                    t.s16= *(int*)sensor_config;               //DON'T TESTED
#else
                    t.u8[0]=sensor_config[1];                  //in this c51 not right order bytes in integer
                    t.u8[1]=sensor_config[0];                  //first Hi, second Lo
#endif

                    t.s16 >>= (sensors_addr[i][0] == 0x10)?1:4;

                    if (t.s16<0){
                        t.s16=-t.s16;
                        signdes=1;
                    }

                    units=numbers[t.s16%10];                   //allways exist

                    if (t.s16>=10){
                        tens=numbers[(t.s16/10)%10];           //tens
                        signdes|=2;
                    }

                    if (t.s16>=100){
                        signdes=4;                             //only "+1xx"
                    }

                    switch(signdes){
                    case 0: SETdisplay(SYMBOL_EMPTY,units,SYMBOL_GRADUS); break;  /* +3o     _3o */
                    case 1: SETdisplay(SYMBOL_MINUS,units,SYMBOL_GRADUS); break;  /* -3o     -3o */
                    case 2: SETdisplay(tens,units,SYMBOL_GRADUS); break;          /* +33o    33o */
                    case 3: SETdisplay(SYMBOL_MINUS,tens,units); break;           /* -33o    -33 */
                    case 4: SETdisplay(SYMBOL_1,tens,units); break;               /* +133o   133 */
                    }
                 }else{
                    SETdisplay_non;
                }
                delayds(DELAY_DISPLAY_OF_TEMP-1);          //wait without "convert"
            }
            if (++i == n) i=0;                             //next sensor
        } //end of if (state)
    } //end of while(1)
} //end of main





















































































































































































































































































































