#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdbool.h>
#include <string.h>

#include "led_ws2811.h"
#include "pin_manager.h"
#include "interrupt_manager.h"

#define setbit(n) (1<<n)
#define getbit(n, data) (data>>n & 1)

/* Fade in slower than fade out, since LED's brightnes curves tend to be harsh down low */
#define fade_byte(act,dsrd) (( act<dsrd ? act + ((dsrd-act)/20 + 1) : \
                              (act>dsrd ? act - ((act-dsrd)/10 + 1) : act)))

static const rgb_type CYAN = {.rgb = {10,30,170}};

static uint8_t num_leds_crnt;
/*---------------------------------------------------------
 Three copies of data. One for current/desired, the user 
 setpoint, and 'breathe base'. Useful for the 'breathe' feature.
 ---------------------------------------------------------*/
static rgb_type leds_crnt[NUM_LEDS_MAX] = {0}; /* Don't set directly */
static rgb_type leds_stpt[NUM_LEDS_MAX] = {0}; /* Don't set directly */
static rgb_type breathe_base[NUM_LEDS_MAX] = {0};

static rgb_type* leds_dsrd;
static bool breathe = true; 

static void send_reset(void);
static void send_led(rgb_type* led);
static __inline void send_one(void);
static __inline void send_zero(void);
static void set_all(rgb_type state);
    
/*---------------------------------------------------------
 Init Driver 
  - Preset output state to
---------------------------------------------------------*/
void led_ws2811_init(){    
    TRISCbits.RC7 = 0;
    LATCbits.LATC7 = 1;
    send_reset();
    
    leds_dsrd = leds_stpt;
    set_all(CYAN);
    led_ws2811_setBreathe(true);
}

/*---------------------------------------------------------
 Will update the desired LED state and perform an initial 
 data refresh. 
 NOTE: If 'fade' by calling the 'setBreathe' function is 
 set, subsequent calls to rfsh will be required to reach final state.
---------------------------------------------------------*/
void led_ws2811_setData(rgb_type* data, uint8_t cnt){
    uint8_t i;
    breathe = false;
    num_leds_crnt = cnt > NUM_LEDS_MAX ? NUM_LEDS_MAX : cnt;
    
    for(i=0;i<num_leds_crnt && (data+i) != NULL; i++){
        leds_stpt[i].raw = data[i].raw;
    }
    led_ws2811_rfsh();
}
/*---------------------------------------------------------
 Will update the desired LED 'breathe' state. A setting of 
 'true' will cause the leds to pulse from the 'desired state' 
 to zero, with consecutive calls to refresh. 
---------------------------------------------------------*/
void led_ws2811_setBreathe(bool newBreathe){
    breathe = newBreathe;
}

/*---------------------------------------------------------
 Will refresh the LED state using the pre-set data.
 Call led_ws2811_set() to change desired output state.
---------------------------------------------------------*/
void led_ws2811_rfsh(){
    uint8_t i, new;
    for(i=0;i<num_leds_crnt; i++){
        leds_crnt[i].rgb.red   = breathe ? fade_byte(leds_crnt[i].rgb.red,   leds_dsrd[i].rgb.red)   : leds_dsrd[i].rgb.red;
        leds_crnt[i].rgb.green = breathe ? fade_byte(leds_crnt[i].rgb.green, leds_dsrd[i].rgb.green) : leds_dsrd[i].rgb.green;
        leds_crnt[i].rgb.blue  = breathe ? fade_byte(leds_crnt[i].rgb.blue,  leds_dsrd[i].rgb.blue)  : leds_dsrd[i].rgb.blue;
    } 
    
    /* 'breathe' */
    if(breathe && leds_crnt[0].raw == leds_dsrd[0].raw){
        leds_dsrd = leds_crnt[0].raw == breathe_base[0].raw ? leds_stpt : breathe_base;
    }
    
    for(i=0;i<num_leds_crnt; i++){
        /* Disable interrupts to prevent odd flashes of color */
        //INTERRUPT_GlobalInterruptDisable();
        send_led(&leds_crnt[i]);
        //INTERRUPT_GlobalInterruptEnable();
    }
}

static void send_reset(){
    uint8_t i;
    
    /* Set low for > 50 us */
    for(i=0; i<(50*4+10); i++){
        LATCbits.LATC7 = 0;
    }
}

/*---------------------------------------------------------
 Sends 24 bits to LED strip
---------------------------------------------------------*/
static void send_led(rgb_type* led){
    signed char i;
    uint8_t b;

    for(i=7;i>=0;i--){
        b = getbit(i, led->rgb.green);
        b ? send_one() : send_zero();
        }
    for(i=7;i>=0;i--){
        b = getbit(i, led->rgb.red);
        b ? send_one() : send_zero();
        }
    
    for(i=7;i>=0;i--){
        b = getbit(i, led->rgb.blue);
        b ? send_one() : send_zero();
        }
}

/* The WS2811 Driver requires rather specific timing, which is handled here */
static __inline void send_one(){
    while(0){
    channel_AN9_SetHigh();\
    channel_AN9_SetLow();
    channel_AN9_SetHigh();\
    channel_AN9_SetLow();
    channel_AN9_SetHigh();\
    channel_AN9_SetLow();
    channel_AN9_SetHigh();\
    channel_AN9_SetLow();
    }
    channel_AN9_SetHigh();\
    channel_AN9_SetHigh();\
    channel_AN9_SetHigh();\
    channel_AN9_SetHigh();\
    channel_AN9_SetHigh();\
    channel_AN9_SetHigh();\
    channel_AN9_SetHigh();\
    channel_AN9_SetLow();
    NOP();
}
/* The WS2811 Driver requires rather specific timing, which is handled here */

static __inline void send_zero(){
    channel_AN9_SetHigh();
    NOP();
    channel_AN9_SetLow();
    }

/* Set all LEDs to the same state */

static void set_all(rgb_type state){
    uint8_t i;
    num_leds_crnt = 15;//NUM_LEDS_MAX;
    for(i=0;i<NUM_LEDS_MAX; i++){
        leds_stpt[i].raw = state.raw;
    }
}
 