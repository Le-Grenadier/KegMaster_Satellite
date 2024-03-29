/*
 * File:   kegMaster.c
 * Author: johngrenard
 *
 * Created on September 28, 2019, 10:21 AM
 */


#include <xc.h>
#include "kegMaster.h"
#include "KegMaster_Satellite.h"

#include "adc.h"
#include "ext_int.h"
#include "gpio.h"
#include "i2c_slave.h"
#include "led_ws2811.h"

void KegMaster_procMsg(KegMaster_SatelliteMsgType* msg){
    switch(msg->id){
        case KegMaster_SateliteMsgId_GpioRead:
            msg->data.gpio.state = gpio_inputStateGet(msg->data.gpio.id);
            i2c_slave_write_data((uint8_t*)msg, KegMaster_SatelliteMsg_Sz(gpio));
            break;

        case KegMaster_SateliteMsgId_GpioSet: 
            gpio_outputDwellSet( msg->data.gpio.id, msg->data.gpio.holdTime );
            gpio_outputStateSet( msg->data.gpio.id, msg->data.gpio.state );
            break;

        case KegMaster_SateliteMsgId_GpioSetDflt:
            gpio_outputDfltSet( msg->data.gpio.id, msg->data.gpio.state );
            break;

        case KegMaster_SateliteMsgId_InterruptRead:
            msg->data.intrpt.count = INT_count[msg->data.intrpt.id];
            i2c_slave_write_data((uint8_t*)msg, KegMaster_SatelliteMsg_Sz(intrpt));
            break;

        case KegMaster_SateliteMsgId_InterruptReset:
            INT_Reset(msg->data.intrpt.id);
            break;

        case KegMaster_SateliteMsgId_ADCRead:
            msg->data.adc.value = adc_values[msg->data.adc.id];
            i2c_slave_write_data((uint8_t*)msg, KegMaster_SatelliteMsg_Sz(adc));
            break;

        case KegMaster_SateliteMsgId_LedSetData:
            gpio_outputDfltSet(0, 0);
            gpio_outputDwellSet(0, /*0x0000FF*/ (2 == msg->data.led_setData.cnt) * 800 + 200);
            gpio_outputStateSet(0,1);
            led_ws2811_setData(msg->data.led_setData.value, msg->data.led_setData.cnt);
            break;
            
        case KegMaster_SateliteMsgId_LedSetBreathe:
            led_ws2811_updtBreathe(msg->data.led_setBreathe.breathe);
            break;
            
        default:
            break;
        }
}
