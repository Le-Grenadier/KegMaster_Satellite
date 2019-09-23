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
#include <string.h>
#include "mcc_generated_files/mcc.h"

#include "adc.h"
#include "device_config.h"
#include "i2c_slave.h"
#include "interrupt_manager.h"
#include "ext_int.h"

#include "KegMaster_Satellite.h"
#include "gpio.h"

/*
Variables
*/
adc_channel_t adc_channels[] = { channel_AN7, channel_AN8, channel_AN9 };
int INT_count[] = {0,0,0};
bool GPIO_dfltState[] = {0,0,0};
unsigned short GPIO_holdTime[] = {500, 500, 500};
adc_result_t ADC_result[] = {0,0,0};
char  iic_buf[255] = {0};
char* iic_buf_ptr;

/*
 Function Prototypes
 */
void Run(void);
void INT_Reset(unsigned char id);
void INT0_MyInterruptHandler(void);
void INT1_MyInterruptHandler(void);
void INT2_MyInterruptHandler(void);
void TMR0_MyInterruptHandler(void);
void proc_msg(KegMaster_SatelliteMsgType* msg);
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

    i2c1_driver_open();
    i2c_slave_open();
    iic_buf_ptr = iic_buf;
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

void expire_gpio(void){    
    for(int i = 0; i<sizeof(GPIO_holdTime)/sizeof(GPIO_holdTime[0]); i++){
        if(0 == (--GPIO_holdTime[i])){
            GPIO_SetPin(i, GPIO_dfltState[i]);
        }
    }
}

void Run(void){    
    KegMaster_SatelliteMsgType* msg;
    KegMaster_SatelliteMsgType m;

    msg = get_msg();
    // Check for new message

    // Process message
    proc_msg(msg);
    m.data.adc.id +=1;
    m.data.adc.id %=3;
        m.id = KegMaster_SateliteMsgId_ADCRead;
        //proc_msg(&m);
        
    // Expire GPIO
    expire_gpio();
}

void INT_Reset(unsigned char id){
    INT_count[id] = 0;
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
    TMR0_StartTimer();

    // It must take less than 1 ms to process 
    Run();
}

void proc_msg(KegMaster_SatelliteMsgType* msg){
    switch(msg->id){
            case KegMaster_SateliteMsgId_GpioSet:
                GPIO_SetPin(msg->data.gpio.id, msg->data.gpio.state);
                GPIO_holdTime[msg->data.gpio.id] = msg->data.gpio.holdTime;
                break;
                
            case KegMaster_SateliteMsgId_GpioRead:
                msg->data.gpio.state = GPIO_ReadPin(msg->data.gpio.id);
                i2c_slave_write_data((uint8_t*)msg, sizeof(*msg));
                break;
                
            case KegMaster_SateliteMsgId_GpioSetDflt:
                GPIO_dfltState[msg->data.gpio.id] = msg->data.gpio.state;
                break;
                     
            case KegMaster_SateliteMsgId_InterruptRead:
                msg->data.intrpt.count = INT_count[msg->data.intrpt.id];
                i2c_slave_write_data((uint8_t*)msg, sizeof(*msg));
                break;
                
            case KegMaster_SateliteMsgId_InterruptReset:
                INT_Reset(msg->data.intrpt.id);
                break;
                
            case KegMaster_SateliteMsgId_ADCRead:
                ADC_SelectChannel((adc_channel_t)adc_channels[msg->data.adc.id]);
                ADC_StartConversion();
                msg->data.adc.value = ADC_GetConversion((adc_channel_t)adc_channels[msg->data.adc.id]);/* This is a blocking call but I'm okay with that rn */
                i2c_slave_write_data((uint8_t*)msg, sizeof(*msg));
                break;
                
            default:
                break;
        }
}

KegMaster_SatelliteMsgType* get_msg(){
    
    static KegMaster_SatelliteMsgType msg;
    volatile short sz_data, sz_msg, bp; 
    char* start;
    char* end;
    char search[2];
    
    sz_data = i2c_slave_get_data(iic_buf_ptr, sizeof(iic_buf) - ( iic_buf_ptr - iic_buf_ptr ) );
    iic_buf_ptr += sz_data;
    search[0] = 0xFF;
    search[1] = 0x01;
    start = strstr(iic_buf, search);
    search[1] = 0x04;
    end = strstr(iic_buf, search);
    sz_msg = end - start;
    if( sz_msg > 0 )
    {
        bp = sz_msg;
    }
    return(&msg);
}

/**
 End of File
*/