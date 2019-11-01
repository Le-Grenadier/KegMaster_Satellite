/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <xc.h>
#include "../i2c1_driver.h"

/**
  Section: Macro Declarations
 */

#define I2C1_SLAVE_DEFAULT_ADDRESS          0x50

/**
  Section: Function declaration
 */
void i2c_slave_open(void);
void i2c_slave_close(void);

void i2c_slave_ISR ( void );

uint8_t i2c_slave_read(void);
unsigned char* i2c_slave_getTxDataPtr(void);
void i2c_slave_write(uint8_t data);
void i2c_slave_write_data(uint8_t* data, uint8_t sz);
void i2c_slave_enable(void);
void i2c_slave_sendAck(void);
void i2c_slave_sendNack(void);

// Read Event Interrupt Handlers
void i2c_slave_RdCallBack(void);
void i2c_slave_setReadIntHandler(interruptHandler handler);
void i2c_slave_DefRdInterruptHandler(void);


// Write Event Interrupt Handlers
void i2c_slave_WrCallBack(void);
void i2c_slave_setWriteIntHandler(interruptHandler handler);
void i2c_slave_DefWrInterruptHandler(void);


// Address Event Interrupt Handlers
void i2c_slave_AddrCallBack(void);
void i2c_slave_setAddrIntHandler(interruptHandler handler);
void i2c_slave_DefAddrInterruptHandler(void);


//ISR HANDLERS
void i2c_slave_setIsrHandler(interruptHandler handler);
void i2c_slave_setBusCollisionISR(interruptHandler handler);
void i2c_slave_BusCollisionISR(void);
void i2c_slave_setWCOLIntHandler(interruptHandler handler);
void i2c_slave_WCOLCallBack(void);
void (*i2c_slave_RdInterruptHandler)(void);
void (*i2c_slave_WrInterruptHandler)(void);
void (*i2c_slave_AddrInterruptHandler)(void);
void (*i2c_slave_WCOLInterruptHandler)(void);

#endif  // I2CSLAVE_H
