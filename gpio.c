
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "mcc.h"
#include "pic18f14k50.h"

#include "gpio.h"
#include "pin_manager.h"


/*==============================================================================
 * Literal Constants 
==============================================================================*/
/* Max of 20 because I'm feeling lazy and don't want to optimize atm */
#define MAX_IO 10
#define INPUT 1
#define OUTPUT 0
#define DWELL_TIME_DFLT 500


/*==============================================================================
 * Types 
==============================================================================*/
typedef struct{
    volatile unsigned char*  pin;
    uint8_t   pinMask;
    volatile unsigned char* tris;
    uint8_t trisMask;
} ioMap;


/*==============================================================================
 * Variables 
==============================================================================*/
const ioMap*    inputPins[MAX_IO];
const ioMap*    outputPins[MAX_IO];
bool            outputStateDflt[MAX_IO] = {0};
uint16_t        outputDwellTime[MAX_IO] = {DWELL_TIME_DFLT};

/*==============================================================================
 * Memory Constants 
==============================================================================*/
static const ioMap inputMap[] = 
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

static const ioMap outputMap[] =     
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

/*==============================================================================
 * Local Prototypes 
==============================================================================*/
static const ioMap* find_inputMap(volatile unsigned char*  pin, uint8_t mask);
static const ioMap* find_outputMap(volatile unsigned char* pin, uint8_t mask);

/*==============================================================================
 * Procedures 
==============================================================================*/
/* Init */
void GPIO_Initialize(){
    memset(inputPins, 0, sizeof(inputPins));
    memset(outputPins, 0, sizeof(outputPins));
    
    /* Input Configuration */
    gpio_registerPin(&PORTA, _PORTA_RA0_MASK, 0);
    gpio_registerPin(&PORTA, _PORTA_RA1_MASK, 1);
    gpio_registerPin(&PORTA, _PORTA_RA4_MASK, 2);
    gpio_registerPin(&PORTA, _PORTA_RA5_MASK, 3);
    
    /* Output configuration */
    gpio_registerPin(&LATB, _LATB_LB5_MASK, 0);
    gpio_registerPin(&LATB, _LATB_LB7_MASK, 1);
    gpio_registerPin(&LATC, _LATC_LC4_MASK, 2);
}

/*----------------------------------------------------------------------------
  Registers pins as input or output. Handles registers. Stores in/out as 
  separate identities and can be accessed independently. e.g. readPin(0) may 
 be configured to access different or the same physical pin (dealer's choice).
 
 Pull-ups must be configured separately.
 ----------------------------------------------------------------------------*/
void gpio_registerPin(volatile unsigned char* gpioAddr, uint8_t mask, uint8_t id){
    const ioMap*    in;
    const ioMap*    out;
    const ioMap*    ioDef;
    const ioMap**   pins;
    uint8_t         cnt;
    
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

/* Get GPIO input state - Must have been configured. */
uint8_t gpio_inputStateGet(uint8_t id){
    return( inputPins[id] != NULL ? (*inputPins[id]->pin & inputPins[id]->pinMask) != 0 : 0 );
}

/* Get GPIO output state - Must have been configured. */
uint8_t gpio_outputStateGet(uint8_t id){
    return( outputPins[id] != NULL ? (*outputPins[id]->pin & outputPins[id]->pinMask) != 0 : 0 );
}

/*------------------------------------------------------------------------------
Sets Output State - Must have been configured as output. 
------------------------------------------------------------------------------*/
void gpio_outputStateSet(uint8_t id, uint8_t state){
    if( id < sizeof( outputPins ) / sizeof( outputPins[0] )
     && outputPins[id] != 0 ){
        *outputPins[id]->pin = state ? outputPins[id]->pinMask : (*outputPins[id]->pin & ~outputPins[id]->pinMask);
        
        /*-------------------------------------------------
         Update Dwell time if necessary 
          - Save a few cycles and bypass dwell expiry proc 
            if default state is being set.
        -------------------------------------------------*/
        if( state != outputStateDflt[id] ){
            gpio_outputDwellSet( id, (0 == outputStateDflt[id]) ? DWELL_TIME_DFLT : outputStateDflt[id] );
        } else {
            gpio_outputDwellSet( id, 0 ); 
        }
    }
}

/*------------------------------------------------------------------------------
Sets Output Default State
 - Initial default state is '0' or 'Off'.
 - Default state persists for remainder of power cycle.
------------------------------------------------------------------------------*/                                                                                  
void gpio_outputDfltSet(uint8_t id, uint8_t dflt){
    if( id < sizeof( outputStateDflt ) / sizeof( outputStateDflt[0] )){
        outputStateDflt[id] = dflt;
    }
}

/*------------------------------------------------------------------------------
Sets Output Dwell time
 - Call 'set dwell' prior to configure 'hold time'
 - Must set dwell time prior to each setState call, else default dwell time used
------------------------------------------------------------------------------*/                                                                                  
void gpio_outputDwellSet(uint8_t id, uint16_t dwell){
    if( id < sizeof( outputDwellTime ) / sizeof( outputDwellTime[0] ) ){
        outputDwellTime[id] = dwell;
    }
}

/*------------------------------------------------------------------------------
 Expire GPIO State - back to default
  - This is in attempt to avoid things like leaving the CO2 valve open if/when 
    we lose connection with the main system for some reason.
  
  - Relies on 1ms periodic call
 ------------------------------------------------------------------------------*/
void gpio_outputDwellProc(void){    
    bool notDflt;
    bool dwellExpr;
    
    for(int i = 0; i<sizeof(outputDwellTime)/sizeof(outputDwellTime[0]); i++){
        notDflt = gpio_outputStateGet(i) != outputStateDflt[i];
        dwellExpr = 0 >= (outputDwellTime[i] );
        outputDwellTime[i] = dwellExpr ? 0 : --outputDwellTime[i];
        
        if( notDflt && dwellExpr ){
            gpio_outputStateSet(i, outputStateDflt[i]);
        }
    }
}

/*==============================================================================
 * Local Functions 
==============================================================================*/
static const ioMap* find_inputMap(volatile unsigned char* pin, uint8_t mask){
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

static const ioMap* find_outputMap(volatile unsigned char* pin, uint8_t mask){
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
