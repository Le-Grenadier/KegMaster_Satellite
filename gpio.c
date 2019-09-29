
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "mcc.h"
#include "pic18f14k50.h"

#include "gpio.h"
#include "pin_manager.h"

/* Max of 20 because I'm feeling lazy and don't want to optimize atm */
#define MAX_IO 10
#define INPUT 1
#define OUTPUT 0

typedef struct{
    volatile unsigned char*  pin;
    uint8_t   pinMask;
    volatile unsigned char* tris;
    uint8_t trisMask;
} ioMap;

ioMap* inputPins[MAX_IO];
ioMap* outputPins[MAX_IO];


ioMap inputMap[] = 
    {
    { &PORTA, _PORTA_RA0_MASK,  NULL,   0             },
    { &PORTA, _PORTA_RA1_MASK,  NULL,   0             },
    { &PORTA, _PORTA_RA3_MASK,  NULL,   0             },
    { &PORTA, _PORTA_RA4_MASK, &TRISA, _LATA_LA4_MASK },
    { &PORTA, _PORTA_RA5_MASK, &TRISA, _LATA_LA5_MASK },
										   
    { &PORTB, _PORTB_RB4_MASK, &TRISB, _LATB_LB4_MASK },
    { &PORTB, _PORTB_RB5_MASK, &TRISB, _LATB_LB5_MASK },
    { &PORTB, _PORTB_RB6_MASK, &TRISB, _LATB_LB6_MASK },
    { &PORTB, _PORTB_RB7_MASK, &TRISB, _LATB_LB7_MASK },
										   
    { &PORTC, _PORTC_RC0_MASK, &TRISC, _LATC_LC0_MASK },
    { &PORTC, _PORTC_RC1_MASK, &TRISC, _LATC_LC1_MASK },
    { &PORTC, _PORTC_RC2_MASK, &TRISC, _LATC_LC2_MASK },
    { &PORTC, _PORTC_RC3_MASK, &TRISC, _LATC_LC3_MASK },
    { &PORTC, _PORTC_RC4_MASK, &TRISC, _LATC_LC4_MASK },
    { &PORTC, _PORTC_RC5_MASK, &TRISC, _LATC_LC5_MASK },
    { &PORTC, _PORTC_RC6_MASK, &TRISC, _LATC_LC6_MASK },
    { &PORTC, _PORTC_RC7_MASK, &TRISC, _LATC_LC7_MASK },
};

ioMap outputMap[] =     
    {
    {  NULL,  0,               NULL,   0             },
    {  NULL,  0,               NULL,   0             },
    {  NULL,  0,               NULL,   0             },
    { &LATA, _LATA_LA4_MASK,  &TRISA, _LATA_LA4_MASK },
    { &LATA, _LATA_LA4_MASK,  &TRISA, _LATA_LA5_MASK },

    { &LATB, _LATB_LB4_MASK,  &TRISB, _LATB_LB4_MASK },
    { &LATB, _LATB_LB5_MASK,  &TRISB, _LATB_LB5_MASK },
    { &LATB, _LATB_LB6_MASK,  &TRISB, _LATB_LB6_MASK },
    { &LATB, _LATB_LB7_MASK,  &TRISB, _LATB_LB7_MASK },

    { &LATC, _LATC_LC0_MASK,  &TRISC, _LATC_LC0_MASK },
    { &LATC, _LATC_LC1_MASK,  &TRISC, _LATC_LC1_MASK },
    { &LATC, _LATC_LC2_MASK,  &TRISC, _LATC_LC2_MASK },
    { &LATC, _LATC_LC3_MASK,  &TRISC, _LATC_LC3_MASK },
    { &LATC, _LATC_LC4_MASK,  &TRISC, _LATC_LC4_MASK },
    { &LATC, _LATC_LC5_MASK,  &TRISC, _LATC_LC5_MASK },
    { &LATC, _LATC_LC6_MASK,  &TRISC, _LATC_LC6_MASK },
    { &LATC, _LATC_LC7_MASK,  &TRISC, _LATC_LC7_MASK },
    };

static ioMap* find_inputMap(volatile unsigned char*  pin, uint8_t mask);
static ioMap* find_outputMap(volatile unsigned char* pin, uint8_t mask);

/* Init */
void GPIO_Initialize(){
    memset(inputPins, 0, sizeof(inputPins));
    memset(outputPins, 0, sizeof(outputPins));
    
    /* Input Configuration */
    gpio_registerPinId(&PORTA, _PORTA_RA0_MASK, 0);
    gpio_registerPinId(&PORTA, _PORTA_RA1_MASK, 1);
    gpio_registerPinId(&PORTA, _PORTA_RA4_MASK, 2);
    gpio_registerPinId(&PORTA, _PORTA_RA5_MASK, 3);
    
    /* Output configuration */
    gpio_registerPinId(&LATB, _LATB_LB5_MASK, 0);
    gpio_registerPinId(&LATB, _LATB_LB7_MASK, 1);
    gpio_registerPinId(&LATC, _LATC_LC4_MASK, 2);
}

/*----------------------------------------------------------------------------
  Registers pins as input or output. Handles registers. Stores in/out as 
  separate identities and can be accessed independently. e.g. readPin(0) may 
 be configured to access different or the same physical pin (dealer's choice).
 
 Pull-ups must be configured separately.
 ----------------------------------------------------------------------------*/
void gpio_registerPinId(volatile unsigned char* gpioAddr, uint8_t mask, uint8_t id){
    ioMap *in;
    ioMap *out;
    ioMap **pins;
    ioMap *ioDef;
    uint8_t   cnt;
    
    in = find_inputMap(gpioAddr, mask);
    out = find_outputMap(gpioAddr, mask);
    
    //assert( (in && 1) ^ (out && 1) );
    
    ioDef = (in && 1) ? in : out;
    pins =  (in && 1) ? inputPins : outputPins;
    cnt =   (in && 1) ? sizeof(inputPins)/sizeof(inputPins[0]) : sizeof(outputPins)/sizeof(outputPins[0]);
    
    if( id < cnt ){
        pins[id] = ioDef;
        
        if(pins[id]->tris != NULL){
            *pins[id]->tris |= (in != NULL)  ?  pins[id]->trisMask : 0x00;
            *pins[id]->tris &= (out != NULL) ? ~pins[id]->trisMask : 0xFF;
        }
    }
}

/* readPin - Must have been configured. */
uint8_t gpio_readPin(uint8_t id){
    return( outputPins[id] != NULL ? (*outputPins[id]->pin & outputPins[id]->pinMask) != 0 : 0 );
}

/* setOutput - Must have been configured as output. */
void gpio_setPin(uint8_t id, uint8_t state){
    if( outputPins[id] != 0 ){
        *outputPins[id]->pin = state ? outputPins[id]->pinMask : (*outputPins[id]->pin & ~outputPins[id]->pinMask);
    }
}

static ioMap* find_inputMap(volatile unsigned char* pin, uint8_t mask){
    uint8_t i;
    uint8_t tblSz;
    
    tblSz = sizeof(inputMap)/sizeof(inputMap[0]);
    
    for(i=0; i<tblSz; i++ ){
        if((pin == inputMap[i].pin) && (mask == inputMap[i].pinMask)){
            break;
        }
    }
    return(i>=tblSz ? NULL : &inputMap[i]);
}

static ioMap* find_outputMap(volatile unsigned char* pin, uint8_t mask){
    uint8_t i;
    uint8_t tblSz;
    
    tblSz = sizeof(outputMap)/sizeof(outputMap[0]);
    
    for(i=0; i<tblSz; i++ ){
        if((pin == outputMap[i].pin) && (mask == outputMap[i].pinMask)){
            break;
        }
    }
    return(i>=tblSz ? NULL : &outputMap[i]);
}
