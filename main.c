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
#include "ext_int.h"

#include "KegMaster_Satellite.h"
#include "gpio.h"

/*
Variables
*/
unsigned short adc_values[] = {0,0,0};
int INT_count[] = {0,0,0};


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
    while(1);
}

void Run(void){   
    static uint8_t adc_id; 
    if( ADC_IsConversionDone() ){
        adc_id += 1;
        adc_id %= sizeof(adc_channels) / sizeof(adc_channels[0]);

        ADC_SelectChannel((adc_channel_t)adc_channels[adc_id]);
        ADC_StartConversion();
    }

    // Expire GPIO Dwell
    gpio_outputDwellProc();
}

void INT0_MyInterruptHandler(void){
    INT_count[0]++;
}

void INT1_MyInterruptHandler(void){
    INT_count[1]++;
}

void INT2_MyInterruptHandler(void){
    INT_count[2]++;
}

void TMR0_MyInterruptHandler(void){  
    TMR0_Reload();
    
    // It must take less than 1 ms to process 
    Run();
}

/**
 End of File
*/