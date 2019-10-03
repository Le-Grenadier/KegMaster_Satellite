#ifndef ADC_HX711_H
#define ADC_HX711_H

#include <pic18f14k50.h>

#include "mcc.h"

#include "gpio.h"

typedef void (*clock_t)(bool state);
typedef bool (*read_t)(void);

void     adc_hx711_init(read_t r, clock_t c);
uint24_t adc_hx711_read();

__inline static bool get_adc_hx711_data(void){return(gpio_inputStateGet(3));}
__inline static void set_adc_hx711_clock(bool s){gpio_outputStateSet(2,s);}

#endif