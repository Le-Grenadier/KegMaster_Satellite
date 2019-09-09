/*
 * Contains definitions to support inter-controller communications
 */
#ifndef KEGMASTER_SATELLITEPIC
#define KEGMASTER_SATELLITEPIC

typedef enum {
    KegMaster_SateliteMsgId_GpioRead,
    KegMaster_SateliteMsgId_GpioSet,
    KegMaster_SateliteMsgId_GpioSetDflt,
    KegMaster_SateliteMsgId_InterruptRead,
    KegMaster_SateliteMsgId_InterruptReset,
    KegMaster_SateliteMsgId_ADCRead
}KegMaster_SatelliteMsgId;


typedef struct{
    KegMaster_SatelliteMsgId id;
    union{
        struct{
            unsigned char id;
            unsigned char state;
            unsigned short holdTime;
        } gpio;
        
        struct{
            unsigned char id;
            unsigned short count;
        } intrpt; /* short name BC 'interrupt' is a keyword in MPLAB (apparently) and messes with syntax highlighting */
        
        struct{
            unsigned char id;
            unsigned short value;
        } adc;
    }data;
} KegMaster_SatelliteMsgType;

#endif