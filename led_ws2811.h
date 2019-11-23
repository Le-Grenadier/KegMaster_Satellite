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
        uint32_t raw;
        struct{
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t white; /* Not used atm, mostly include since the Sphere and the PIC are having byte alignment issues */
        }rgb;
    }rgb_type;

    void led_ws2811_init(void);
    void led_ws2811_setData(rgb_type* data, uint8_t cnt);
    void led_ws2811_updtBreathe(bool newBreathe);
    void led_ws2811_rfsh(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* LED_WS2811_H */

