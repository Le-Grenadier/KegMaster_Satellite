#include <stdbool.h>
#include <pic18f14k50.h>

#include "adc_hx711.h"
#include "tsk_timer.h"

static clock_t clock;
static read_t  read;
static uint24_t timer; 

__inline bool data_ready(){return(!read());}

void cycle_clock(void);
uint8_t get_bit(void);

void adc_hx711_init(read_t r, clock_t c){
    clock = c;
    read = r;
    timer = 0; 
    
}

bool adc_hx711_read(uint32_t *val){
    int8_t i;
    int32_t b, x, y, z;

    /* Read no more often than every 200 ms */
    if( ( (TSK_timer_get() - timer) < 200
        || !data_ready() )  ){
        return(false);
    }
    timer = TSK_timer_get();

    /* MSB comes first */
    x=0;
    for(i=7; i>=0; i--){
        b = get_bit();
        x |= (!b << i);
    }
    y=0;
    for(i=7; i>=0; i--){
        b = get_bit();
        y |= (!b << i);
    }
    z=0;
    for(i=7; i>=0; i--){
        b = get_bit();
        z |= (!b << i);
    }
    
    /*-----------------------------------------------------
     one extra clock to set/keep gain at 128
      - 2 extra clocks would be gain of 64
      - 3 extra clocks would be gain of 32
      - See data sheet for more info 
    ------------------------------------------------------*/
    cycle_clock(); 
    
    *val = x << 16 | y << 8 | z;
    
    return(true);
}


void cycle_clock(){
    clock();
}

uint8_t get_bit(){
    cycle_clock();
    gpio_outputStateSet(2, 0); // Delay to stabilize
    return(read());
}
