#ifndef GPIO_H
#define	GPIO_H

#include <xc.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "pin_manager.h"
#include "pic18f14k50.h"

extern uint16_t GPIO_holdTime[10];

/*=============================================================================
  Function Prototypes
=============================================================================*/
void GPIO_Initialize(void);

/*----------------------------------------------------------------------------
  Registers pins as input or output based on the provided address. Stores in/out 
  as separate identities and can be accessed independently. e.g. readPin(0) may 
  be configured to access different or the same physical pin (dealer's choice).
 
 Pull-ups must be configured separately.
 ----------------------------------------------------------------------------*/
void gpio_registerPin(volatile unsigned char* gpioAddr, uint8_t mask, uint8_t id);

/* get GPIO input state - Must have been configured as input. */
uint8_t gpio_inputStateGet(uint8_t id);

/* get GPIO output state - Must have been configured as output */
uint8_t gpio_outputStateGet(uint8_t id);

/*------------------------------------------------------------------------------
Clocks Output State - Based on current output level
 - Simply setting the output was not fast enough for some uses.
------------------------------------------------------------------------------*/
void gpio_outputClock(uint8_t id);

/*------------------------------------------------------------------------------
Sets Output State - Must have been configured as output. 
------------------------------------------------------------------------------*/
void gpio_outputStateSet(uint8_t id, uint8_t state);

/*------------------------------------------------------------------------------
Sets Output Default State
 - Initial default state is '0' or 'Off'.
 - Default state persists for remainder of power cycle.
------------------------------------------------------------------------------*/                                                                                  
void gpio_outputDfltSet(uint8_t id, uint8_t dflt);

/*------------------------------------------------------------------------------
Sets Output Dwell time
 - Call 'set dwell' prior to configure 'hold time'
 - Must set dwell time prior to each setState call, else default dwell time used
------------------------------------------------------------------------------*/
void gpio_outputDwellSet(uint8_t id, uint24_t dwell);

/*------------------------------------------------------------------------------
 Expire GPIO State - back to default, based on dwell time
  - This is in attempt to avoid things like leaving the CO2 valve open if/when 
    we lose connection with the main system for some reason.
 
  - Relies on 1ms periodic call
 ------------------------------------------------------------------------------*/
void gpio_outputDwellProc(uint24_t timer);

#endif	/* GPIO_H */

