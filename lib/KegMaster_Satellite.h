/*
 * Contains definitions to support inter-controller communications
 */
#ifndef KEGMASTER_SATELLITE_H
#define KEGMASTER_SATELLITE_H

#include "mcc.h"
#include "led_ws2811.h"

typedef enum {
    KegMaster_SateliteMsgId_GpioRead,
    KegMaster_SateliteMsgId_GpioSet,
    KegMaster_SateliteMsgId_GpioSetDflt,
    KegMaster_SateliteMsgId_InterruptRead,
    KegMaster_SateliteMsgId_InterruptReset,
    KegMaster_SateliteMsgId_ADCRead,
    KegMaster_SateliteMsgId_LedSet
}KegMaster_SatelliteMsgId;


typedef struct{
    KegMaster_SatelliteMsgId id;
    union{
        struct{
            uint8_t id;
            uint8_t state;
            uint16_t holdTime;
        } gpio;
        
        struct{
            uint8_t id;
            uint24_t count;
        } intrpt; 
        
        struct{
            uint8_t id;
            uint32_t value;
        } adc;
        struct{
            uint8_t cnt;
            bool    fade;
            rgb_type value[20]; /* Variable size */
        } led_set;
    }data;
} KegMaster_SatelliteMsgType;

#define KegMaster_SatelliteMsg_Sz(member) ( \
    sizeof(((KegMaster_SatelliteMsgType*)(0))->id) + sizeof(((KegMaster_SatelliteMsgType*)(0))->data.member) \
    )
#endif