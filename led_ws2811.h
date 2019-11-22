#ifndef LED_WS2811_H
#define	LED_WS2811_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    #define NUM_LEDS_MAX 20
    
    typedef union{
        uint24_t raw;
        struct{
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        }rgb;
    }rgb_type;

    void led_ws2811_init(void);
    void led_ws2811_set(rgb_type* data, uint8_t cnt, bool fade);
    void led_ws2811_rfsh(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* LED_WS2811_H */

