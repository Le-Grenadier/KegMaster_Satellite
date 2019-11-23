/**
  TMR0 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    tmr0.c

  @Summary
    This is the generated driver implementation file for the TMR0 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for TMR0.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC18F14K50
        Driver Version    :  2.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/**
  Section: Included Files
*/

#include <xc.h>
#include "tmr0.h"

#include "gpio.h"
#include "tsk_timer.h"

/**
  Section: Global Variables Definitions
*/

void (*TMR0_InterruptHandler)(void);

volatile uint16_t timer0ReloadVal;


/**
  Section: TMR0 APIs
*/


void TMR0_Initialize(void)
{
    #define CLK_FREQ 8000000
    #define CLK_PER_MSEC CLK_FREQ / 1000
    #define PRESCALER 8
    
    // Set TMR0 to the options selected in the User Interface
    /*-----------------------------------------------------
     *  Load TMR0 value to the 16-bit reload variable
     * 
     * Timer interrupt triggers on overflow, set as reciprocal
     * -----------------------------------------------------*/

    TMR0_WriteTimer( 0xFFFF - (CLK_PER_MSEC)/PRESCALER);

    // Set Default Interrupt Handler
    TMR0_SetInterruptHandler(TMR0_DefaultInterruptHandler);

    // T0PS 1:256; T08BIT 16-bit; T0SE Increment_hi_lo; T0CS T0CKI; TMR0ON enabled; PSA not_assigned; 
    T0CON = 0x18;
    T0CONbits.PSA = 0;  // 0==Enable prescaler
    T0CONbits.T0PS = 2; // 1:8 prescaler
                
    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;

    // Enabling TMR0 interrupt.
    INTCONbits.TMR0IE = 1;

}

void TMR0_StartTimer(void)
{
    // Start the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 1;
}

void TMR0_StopTimer(void)
{
    // Stop the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 0;
}

uint16_t TMR0_ReadTimer(void)
{
    uint16_t readVal;
    uint8_t readValLow;
    uint8_t readValHigh;

    readValLow  = TMR0L;
    readValHigh = TMR0H;
    readVal  = ((uint16_t)readValHigh << 8) + readValLow;
    
    return readVal;
}

void TMR0_WriteTimer(uint16_t timerVal)
{
    #define PIC_SKIPPED_CYCLES 2
    #define ISR_SKIPPED_CYCLES 10

    // Write to the Timer0 registers, low register only
    timer0ReloadVal = timerVal + PIC_SKIPPED_CYCLES + ISR_SKIPPED_CYCLES;
    TMR0_Reload();
 }

void TMR0_Reload(void)
{
    uint8_t th; 
    
    //Write to the Timer0 register'    
    th = 0xFF & ( timer0ReloadVal >> 8 );
    TMR0H = th;
    TMR0L = timer0ReloadVal & 0xFF;
}

void TMR0_ISR(void)
{
    TMR0_Reload();

    LATAbits.LA4 = !PORTAbits.RA4;
    // clear the TMR0 interrupt flag
    INTCONbits.TMR0IF = 0;

    // ticker function call;
    // ticker is 1 -> Callback function gets called every time this ISR executes
    TMR0_CallBack();
}

void TMR0_CallBack(void)
{
    // Add your custom callback code here

    if(TMR0_InterruptHandler)
    {
        TMR0_InterruptHandler();
    }
}

void TMR0_SetInterruptHandler(void (* InterruptHandler)(void)){
    TMR0_InterruptHandler = InterruptHandler;
}

void TMR0_DefaultInterruptHandler(void){
    // add your TMR0 interrupt custom code
    // or set custom function using TMR0_SetInterruptHandler()
}

/**
  End of File
*/