/**
  Generated Main Source File

  Company:
    John Grenard
 
  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC18F14K50
        Driver Version    :  2.00
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
#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mcc.h"

#include "adc.h"
#include "adc_hx711.h"
#include "device_config.h"
#include "i2c_slave.h"
#include "interrupt_manager.h"
#include "led_ws2811.h"
#include "tsk_timer.h"
#include "ext_int.h"

#include "KegMaster_Satellite.h"
#include "gpio.h"

/*
Variables
*/
volatile uint24_t INT_count[] = {0,0,0};


/*
 Function Prototypes
 */
void Run(void);
void INT0_MyInterruptHandler(void);
void INT1_MyInterruptHandler(void);
void INT2_MyInterruptHandler(void);
void TMR0_MyInterruptHandler(void);
KegMaster_SatelliteMsgType* get_msg(void);
void expire_gpio(void);

/*
                         Main application
 */
void main(void)
{
    #define RANDOM_OFST 13
    static uint24_t timer_100ms = 0;
    static uint24_t timer_200ms = 0;
    static uint24_t timer_1ms = 0;
    uint24_t timeNow;
    
    // Initialize the device
    SYSTEM_Initialize();
    
    INT_Reset(0);
    INT0_SetInterruptHandler(INT0_MyInterruptHandler);
    
    INT_Reset(1);
    INT1_SetInterruptHandler(INT1_MyInterruptHandler);
    
    INT_Reset(2);
    INT2_SetInterruptHandler(INT2_MyInterruptHandler);

    /* init adc_hx711 */
    adc_hx711_init(&get_adc_hx711_data, &set_adc_hx711_clock);
    
    /* Init LED Driver */
    led_ws2811_init();
            
    /* I2C */
    i2c1_driver_open();
    i2c_slave_open();
    i2c1_driver_restart();
    i2c1_driver_start();
    mssp1_enableIRQ(); // Enable MSSP (I2C) Interrupts)
    
    /* Business Happens Here - Isochronous Data Processing for timing and stuff */
    TMR0_SetInterruptHandler(TMR0_MyInterruptHandler);
    TMR0_StartTimer();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    while(1){
        timeNow = TSK_timer_get();
        if(timeNow > timer_100ms ){
            timer_100ms = TSK_timer_get() + 100;
            Run();
        }
        
        if(false){// && timeNow > (timer_200ms + RANDOM_OFST)){
            timer_200ms = TSK_timer_get() + 200;
            led_ws2811_rfsh();
        }
        
        if(timeNow > timer_1ms){
            timer_1ms = timeNow + 1;
            // Expire GPIO Dwell times
            gpio_outputDwellProc(timeNow);
        }
    }
}

void Run(void){   
    static uint8_t adc_id; 
    uint32_t scale = 0;
    
    if( ADC_IsConversionDone() ){
        adc_id += 1;
        adc_id %= (sizeof(adc_channels) / sizeof(adc_channels[0]));
        ADC_SelectChannel((adc_channel_t)adc_channels[adc_id]);
        ADC_StartConversion();
    }

    // Read HX711 ADC
    if( adc_hx711_read(&scale) )
    {
        adc_values[3] = scale;
    }
}

void INT0_MyInterruptHandler(void){
    static uint24_t tskTimePrev = 0;
    uint24_t tskTimeNow;
    
    /* Debounce Interrupts */
    tskTimeNow = TSK_timer_get();
    if( tskTimePrev == tskTimeNow ){
        return;
    }
    
    /* Low band filter */
    tskTimePrev = tskTimeNow;
    if((tskTimeNow - tskTimePrev) > 100){
        INT_count[0] = INT_count[0]+1;
    }
}

void INT1_MyInterruptHandler(void){
    static uint24_t tskTimePrev = 0;
    uint24_t tskTimeNow;
    
    /* Debounce Interrupts */
    tskTimeNow = TSK_timer_get();
    if( tskTimePrev == tskTimeNow ){
        return;
    }
    tskTimePrev = tskTimeNow;
    if((tskTimeNow - tskTimePrev) > 100){
        INT_count[1] = INT_count[1]+1;
    }
}

void INT2_MyInterruptHandler(void){
    static uint24_t tskTimePrev = 0;
    uint24_t tskTimeNow;
    
    /* Debounce Interrupts */
    tskTimeNow = TSK_timer_get();
    if( tskTimePrev == tskTimeNow ){
        return;
    }
    tskTimePrev = tskTimeNow;
    if((tskTimeNow - tskTimePrev) > 100){
        INT_count[2] = INT_count[2]+1;
    }
}

void TMR0_MyInterruptHandler(void){  
    TSK_timer++;
}

/**
 End of File
*/