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

void KegMaster_procMsg(KegMaster_SatelliteMsgType* msg){
    switch(msg->id){
            case KegMaster_SateliteMsgId_GpioSet:
                GPIO_SetPin(msg->data.gpio.id, msg->data.gpio.state);
                GPIO_holdTime[msg->data.gpio.id] = msg->data.gpio.holdTime;
                break;
                
            case KegMaster_SateliteMsgId_GpioRead:
                msg->data.gpio.state = GPIO_ReadPin(msg->data.gpio.id);
                i2c_slave_write_data((uint8_t*)msg, sizeof(*msg));
                break;
                
            case KegMaster_SateliteMsgId_GpioSetDflt:
                GPIO_dfltState[msg->data.gpio.id] = msg->data.gpio.state;
                break;
                     
            case KegMaster_SateliteMsgId_InterruptRead:
                msg->data.intrpt.count = INT_count[msg->data.intrpt.id];
                i2c_slave_write_data((uint8_t*)msg, sizeof(*msg));
                break;
                
            case KegMaster_SateliteMsgId_InterruptReset:
                INT_Reset(msg->data.intrpt.id);
                break;
                
            case KegMaster_SateliteMsgId_ADCRead:
                msg->data.adc.value = adc_values[msg->data.adc.id];/* This is a blocking call but I'm okay with that rn */
                i2c_slave_write_data((uint8_t*)msg, sizeof(*msg));
                break;
                
            default:
                break;
        }
}
