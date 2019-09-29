/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef GPIO_H
#define	GPIO_H

#include <assert.h>
#include <stdbool.h>

#include "pin_manager.h"
#include "pic18f14k50.h"

extern unsigned short GPIO_holdTime[];
/*=============================================================================
  Function Prototypes
=============================================================================*/
void GPIO_Initialize();

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
void gpio_outputDwellSet(uint8_t id, uint16_t dwell);

/*------------------------------------------------------------------------------
 Expire GPIO State - back to default, based on dwell time
  - This is in attempt to avoid things like leaving the CO2 valve open if/when 
    we lose connection with the main system for some reason.
 
  - Relies on 1ms periodic call
 ------------------------------------------------------------------------------*/
void gpio_outputDwellProc(void);

#endif	/* GPIO_H */

