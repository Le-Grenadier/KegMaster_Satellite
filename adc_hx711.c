#include <stdbool.h>
#include <pic18f14k50.h>

#include "adc_hx711.h"
#include "tsk_timer.h"

static clock_t clock;
static read_t  read;
static uint24_t timer; 

__inline bool data_ready(){return(!read());}

void cycle_clock();
bool get_bit();

void adc_hx711_init(read_t r, clock_t c){
    clock = c;
    read = r;
    timer = 0; 

    clock();
}

uint24_t adc_hx711_read(){
    uint24_t b = 0;
    int8_t i;

    /* Read no more often than every 200 ms */
    if( ( (TSK_timer_get() - timer) < 200
        || !data_ready() )  ){
        return(0);
    }
    timer = TSK_timer_get();
    
    /* MSB comes first */
    for(i=23; i>=0; i--){
        b |= get_bit() << i;
    }
    cycle_clock(); /* Required by ADC to start next conversion */
    
    return(b);
}


void cycle_clock(){
    clock();
}

bool get_bit(){
    cycle_clock();
    return(read());
}
