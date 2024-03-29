
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "mcc.h"
#include "pic18f14k50.h"

#include "gpio.h"
#include "pin_manager.h"
#include "tsk_timer.h"


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
    volatile unsigned char*  pinBus;
    uint8_t                  pinMask;
    volatile unsigned char*  tris;
    uint8_t                  trisMask;
} ioMap;


/*==============================================================================
 * Variables 
==============================================================================*/
uint8_t         inputPins_cnt;
const ioMap*    inputPins[MAX_IO];
uint8_t         outputPins_cnt;
const ioMap*    outputPins[MAX_IO];
bool            outputStateDflt[MAX_IO] = {0};
uint24_t        outputDwellTime[MAX_IO] = {DWELL_TIME_DFLT};
uint24_t        outputDwellTimer[MAX_IO] = {DWELL_TIME_DFLT};

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
    inputPins_cnt = 0;
    outputPins_cnt = 0;
    memset(inputPins, 0, sizeof(inputPins));
    memset(outputPins, 0, sizeof(outputPins));
    
    UCON = false; /* RA0 and RA1 are only available if USB is disabled */
    IOCAbits.IOCA0 = 1; /* Set RA0 Interrupt on change bit to allow Digital IO */
    IOCAbits.IOCA1 = 1; /* Set RA0 Interrupt on change bit to allow Digital IO */
            
    /* Input Configuration */
    gpio_registerPin(&PORTA, _PORTA_RA5_MASK, 0); // I2C Addr low bit
    gpio_registerPin(&PORTA, _PORTA_RA4_MASK, 1); // I2C Addr high bit
    gpio_registerPin(&PORTA, _PORTA_RA1_MASK, 2);
    gpio_registerPin(&PORTA, _PORTA_RA0_MASK, 3); // Scale adc data input
    
    /* Output configuration */
    gpio_registerPin(&LATB, _LATB_LB7_MASK, 0);
    gpio_registerPin(&LATB, _LATB_LB5_MASK, 1); // Scale adc clock output
    gpio_registerPin(&LATC, _LATC_LC5_MASK, 2);
    gpio_registerPin(&LATC, _LATC_LC4_MASK, 3);
    //
    //gpio_registerPin(&LATC, _LATC_LC7_MASK, 4); // LED Driver - Stolen from ADC


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
    cnt =   MAX_IO;
    
    if( id < cnt ){
        pins[id] = ioDef;
        outputDwellTime[id] = DWELL_TIME_DFLT;
        
        /* Un-configuring pins not supported for now */
        inputPins_cnt += (in != NULL);
        outputPins_cnt += (out != NULL);
        
        /* Set IO direction if required */
        if(pins[id]->tris != NULL){
            *pins[id]->tris |= (in != NULL)  ?  pins[id]->trisMask : 0x00;
            *pins[id]->tris &= (out != NULL) ? ~pins[id]->trisMask : 0xFF;
        }
    }
}

/* Get GPIO input state - Must have been configured. */
uint8_t gpio_inputStateGet(uint8_t id){
    uint8_t i;
    
    if(id >= inputPins_cnt || inputPins[id] == NULL){
        return(0);
    }
    
    i = (0 != (*inputPins[id]->pinBus & inputPins[id]->pinMask));
    return(i);
}

/*------------------------------------------------------------------------------
Clocks Output State - Based on current output level
 - Simply setting the output was not fast enough for some uses.
------------------------------------------------------------------------------*/
void gpio_outputClock(uint8_t id){
    uint8_t crnt, clk_s, buss; 
    
    crnt = gpio_outputStateGet(id);
    clk_s = !crnt;
    
    if( id < outputPins_cnt 
     && outputPins[id] != NULL ){
        buss = *outputPins[id]->pinBus;
        *outputPins[id]->pinBus = clk_s ? (buss | outputPins[id]->pinMask) : (buss & ~outputPins[id]->pinMask);
        *outputPins[id]->pinBus = crnt  ? (buss | outputPins[id]->pinMask) : (buss & ~outputPins[id]->pinMask);
    }
}

/* Get GPIO output state - Must have been configured. */
uint8_t gpio_outputStateGet(uint8_t id){
    return( outputPins[id] != NULL ? (*outputPins[id]->pinBus & outputPins[id]->pinMask) != 0 : 0 );
}

/*------------------------------------------------------------------------------
Sets Output State - Must have been configured as output. 
------------------------------------------------------------------------------*/
void gpio_outputStateSet(uint8_t id, uint8_t state){
    if( id < outputPins_cnt 
     && outputPins[id] != NULL ){
        *outputPins[id]->pinBus = state ? (*outputPins[id]->pinBus | outputPins[id]->pinMask) : (*outputPins[id]->pinBus & ~outputPins[id]->pinMask);
        
        outputDwellTimer[id] = TSK_timer_get() + outputDwellTime[id];
    }
}

/*------------------------------------------------------------------------------
Sets Output Default State
 - Initial default state is '0' or 'Off'.
 - Default state persists for remainder of power cycle.
------------------------------------------------------------------------------*/                                                                                  
void gpio_outputDfltSet(uint8_t id, uint8_t dflt){
    /* Allow default state to be set before pins are configured */
    if( id < MAX_IO ){
        outputStateDflt[id] = dflt;
    }
}

/*------------------------------------------------------------------------------
Sets Output Dwell time
 - Call 'set dwell' prior to configure 'hold time'
 - Must set dwell time prior to each setState call, else default dwell time used
------------------------------------------------------------------------------*/                                                                                  
void gpio_outputDwellSet(uint8_t id, uint24_t dwell){
    /* Allow dwell time to be set before pins are configured */
    if( id < MAX_IO ){
        outputDwellTime[id] = dwell;
    }
}

/*------------------------------------------------------------------------------
 Expire GPIO State - back to default
  - This is in attempt to avoid things like leaving the CO2 valve open if/when 
    we lose connection with the main system for some reason.
  
  - Relies on 1ms periodic call
 ------------------------------------------------------------------------------*/
void gpio_outputDwellProc(uint24_t timer){   
    static uint8_t i;
    bool notDflt;
    bool dwellExpr;
    
    for(i = 0; i<outputPins_cnt; i++){
        
        /* Check shouldn't be required, but leave for safety */
        if(!outputPins[i]){
            continue;
        }
        
        notDflt = ( gpio_outputStateGet(i) != outputStateDflt[i] );
        /*--------------------------------------
         Dwell expired if: 
          - tskTime > exprTime (normal case)
          - tskTime overflowed and we're less than one 'dwell time past expr'
           
         NOTE: The second case tries to capture the situation where the gpio 
               should expire at 'max_uint24_val' and the timer has overflowed. 
               Also allow up to 10 mS of delay, in case we missed some cycles.
         --------------------------------------*/
        dwellExpr = ( (timer >= outputDwellTimer[i])  /* Expired - normal case */
                   || ( (0 - outputDwellTimer[i] < timer) &&  (timer < outputDwellTime[i]+10))); /* expired - overflow case */
        
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
        if((pin == inputMap[i].pinBus) && (mask == inputMap[i].pinMask)){
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
        if((pin == outputMap[i].pinBus) && (mask == outputMap[i].pinMask)){
            break;
        }
    }
    return(i>=tblSz ? NULL : &outputMap[i]);
}
