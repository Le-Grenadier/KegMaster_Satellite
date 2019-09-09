/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC18F14K50
        Driver Version    :  2.11
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

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set IO_RA0 aliases
#define IO_RA0_PORT                 PORTAbits.RA0
#define IO_RA0_GetValue()           PORTAbits.RA0

// get/set IO_RA1 aliases
#define IO_RA1_PORT                 PORTAbits.RA1
#define IO_RA1_GetValue()           PORTAbits.RA1

// get/set IO_RA4 aliases
#define IO_RA4_TRIS                 TRISAbits.TRISA4
#define IO_RA4_LAT                  LATAbits.LATA4
#define IO_RA4_PORT                 PORTAbits.RA4
#define IO_RA4_WPU                  WPUAbits.WPUA4
#define IO_RA4_ANS                  ANSELbits.ANS3
#define IO_RA4_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define IO_RA4_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define IO_RA4_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define IO_RA4_GetValue()           PORTAbits.RA4
#define IO_RA4_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define IO_RA4_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define IO_RA4_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define IO_RA4_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define IO_RA4_SetAnalogMode()      do { ANSELbits.ANS3 = 1; } while(0)
#define IO_RA4_SetDigitalMode()     do { ANSELbits.ANS3 = 0; } while(0)

// get/set IO_RA5 aliases
#define IO_RA5_TRIS                 TRISAbits.TRISA5
#define IO_RA5_LAT                  LATAbits.LATA5
#define IO_RA5_PORT                 PORTAbits.RA5
#define IO_RA5_WPU                  WPUAbits.WPUA5
#define IO_RA5_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define IO_RA5_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define IO_RA5_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define IO_RA5_GetValue()           PORTAbits.RA5
#define IO_RA5_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define IO_RA5_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define IO_RA5_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define IO_RA5_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)

// get/set SDA1 aliases
#define SDA1_TRIS                 TRISBbits.TRISB4
#define SDA1_LAT                  LATBbits.LATB4
#define SDA1_PORT                 PORTBbits.RB4
#define SDA1_WPU                  WPUBbits.WPUB4
#define SDA1_ANS                  ANSELHbits.ANS10
#define SDA1_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define SDA1_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define SDA1_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define SDA1_GetValue()           PORTBbits.RB4
#define SDA1_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define SDA1_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define SDA1_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define SDA1_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define SDA1_SetAnalogMode()      do { ANSELHbits.ANS10 = 1; } while(0)
#define SDA1_SetDigitalMode()     do { ANSELHbits.ANS10 = 0; } while(0)

// get/set IO_RB5 aliases
#define IO_RB5_TRIS                 TRISBbits.TRISB5
#define IO_RB5_LAT                  LATBbits.LATB5
#define IO_RB5_PORT                 PORTBbits.RB5
#define IO_RB5_WPU                  WPUBbits.WPUB5
#define IO_RB5_ANS                  ANSELHbits.ANS11
#define IO_RB5_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define IO_RB5_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define IO_RB5_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define IO_RB5_GetValue()           PORTBbits.RB5
#define IO_RB5_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define IO_RB5_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define IO_RB5_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define IO_RB5_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define IO_RB5_SetAnalogMode()      do { ANSELHbits.ANS11 = 1; } while(0)
#define IO_RB5_SetDigitalMode()     do { ANSELHbits.ANS11 = 0; } while(0)

// get/set SCL1 aliases
#define SCL1_TRIS                 TRISBbits.TRISB6
#define SCL1_LAT                  LATBbits.LATB6
#define SCL1_PORT                 PORTBbits.RB6
#define SCL1_WPU                  WPUBbits.WPUB6
#define SCL1_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define SCL1_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define SCL1_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define SCL1_GetValue()           PORTBbits.RB6
#define SCL1_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define SCL1_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define SCL1_SetPullup()          do { WPUBbits.WPUB6 = 1; } while(0)
#define SCL1_ResetPullup()        do { WPUBbits.WPUB6 = 0; } while(0)

// get/set IO_RB7 aliases
#define IO_RB7_TRIS                 TRISBbits.TRISB7
#define IO_RB7_LAT                  LATBbits.LATB7
#define IO_RB7_PORT                 PORTBbits.RB7
#define IO_RB7_WPU                  WPUBbits.WPUB7
#define IO_RB7_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define IO_RB7_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define IO_RB7_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define IO_RB7_GetValue()           PORTBbits.RB7
#define IO_RB7_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define IO_RB7_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define IO_RB7_SetPullup()          do { WPUBbits.WPUB7 = 1; } while(0)
#define IO_RB7_ResetPullup()        do { WPUBbits.WPUB7 = 0; } while(0)

// get/set RC0 procedures
#define RC0_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define RC0_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define RC0_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define RC0_GetValue()              PORTCbits.RC0
#define RC0_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define RC0_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define RC0_SetAnalogMode()         do { ANSELbits.ANS4 = 1; } while(0)
#define RC0_SetDigitalMode()        do { ANSELbits.ANS4 = 0; } while(0)

// get/set RC1 procedures
#define RC1_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define RC1_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define RC1_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define RC1_GetValue()              PORTCbits.RC1
#define RC1_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define RC1_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define RC1_SetAnalogMode()         do { ANSELbits.ANS5 = 1; } while(0)
#define RC1_SetDigitalMode()        do { ANSELbits.ANS5 = 0; } while(0)

// get/set RC2 procedures
#define RC2_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define RC2_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define RC2_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define RC2_GetValue()              PORTCbits.RC2
#define RC2_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define RC2_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define RC2_SetAnalogMode()         do { ANSELbits.ANS6 = 1; } while(0)
#define RC2_SetDigitalMode()        do { ANSELbits.ANS6 = 0; } while(0)

// get/set channel_AN7 aliases
#define channel_AN7_TRIS                 TRISCbits.TRISC3
#define channel_AN7_LAT                  LATCbits.LATC3
#define channel_AN7_PORT                 PORTCbits.RC3
#define channel_AN7_ANS                  ANSELbits.ANS7
#define channel_AN7_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define channel_AN7_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define channel_AN7_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define channel_AN7_GetValue()           PORTCbits.RC3
#define channel_AN7_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define channel_AN7_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define channel_AN7_SetAnalogMode()      do { ANSELbits.ANS7 = 1; } while(0)
#define channel_AN7_SetDigitalMode()     do { ANSELbits.ANS7 = 0; } while(0)

// get/set IO_RC4 aliases
#define IO_RC4_TRIS                 TRISCbits.TRISC4
#define IO_RC4_LAT                  LATCbits.LATC4
#define IO_RC4_PORT                 PORTCbits.RC4
#define IO_RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define IO_RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define IO_RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define IO_RC4_GetValue()           PORTCbits.RC4
#define IO_RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define IO_RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)

// get/set IO_RC5 aliases
#define IO_RC5_TRIS                 TRISCbits.TRISC5
#define IO_RC5_LAT                  LATCbits.LATC5
#define IO_RC5_PORT                 PORTCbits.RC5
#define IO_RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define IO_RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define IO_RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define IO_RC5_GetValue()           PORTCbits.RC5
#define IO_RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define IO_RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)

// get/set channel_AN8 aliases
#define channel_AN8_TRIS                 TRISCbits.TRISC6
#define channel_AN8_LAT                  LATCbits.LATC6
#define channel_AN8_PORT                 PORTCbits.RC6
#define channel_AN8_ANS                  ANSELHbits.ANS8
#define channel_AN8_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define channel_AN8_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define channel_AN8_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define channel_AN8_GetValue()           PORTCbits.RC6
#define channel_AN8_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define channel_AN8_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define channel_AN8_SetAnalogMode()      do { ANSELHbits.ANS8 = 1; } while(0)
#define channel_AN8_SetDigitalMode()     do { ANSELHbits.ANS8 = 0; } while(0)

// get/set channel_AN9 aliases
#define channel_AN9_TRIS                 TRISCbits.TRISC7
#define channel_AN9_LAT                  LATCbits.LATC7
#define channel_AN9_PORT                 PORTCbits.RC7
#define channel_AN9_ANS                  ANSELHbits.ANS9
#define channel_AN9_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define channel_AN9_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define channel_AN9_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define channel_AN9_GetValue()           PORTCbits.RC7
#define channel_AN9_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define channel_AN9_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define channel_AN9_SetAnalogMode()      do { ANSELHbits.ANS9 = 1; } while(0)
#define channel_AN9_SetDigitalMode()     do { ANSELHbits.ANS9 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/