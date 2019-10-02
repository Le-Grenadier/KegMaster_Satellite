#include <stdbool.h>
#include <pic18f14k50.h>

#include "adc_hx711.h"

static clock_t clock;
static read_t  read;

__inline bool data_ready(){return(!read());}

void cycle_clock();
bool get_bit();

void adc_hx711_init(read_t r, clock_t c){
    clock = c;
    read = r;
    
    clock(0);
}

uint24_t adc_hx711_read(){
    uint24_t b = 0;
    uint8_t i;
    
    if(!data_ready()){
        return(0);
    }
    
    /* MSB comes first */
    for(i=23; i>=0; i++){
        b |= get_bit() << i;
    }
    cycle_clock(); /* Required by ADC to start next conversion */
    
    return(b);
}


void cycle_clock(){
    clock(1);
    // May need to do some waiting here. Spec says dwell time needs to exceed 0.1us
    clock(0);
}

bool get_bit(){
    cycle_clock();
    return(read());
}
