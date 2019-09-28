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

#include <assert.h>
#include <math.h>
#include <string.h>
#include <xc.h>

#include "i2c_slave.h"

#include "kegMaster.h"

#define I2C1_SLAVE_ADDRESS 8 
#define I2C1_SLAVE_MASK    0xFF // Mask value of 0 is applied to the same address bit as "Don't care" 

/**
 Section: Global Variables
 */

typedef enum {Address, I2C_TX, I2C_RX} i2c_state_t;
    
static volatile i2c_state_t state = Address;
static volatile i2c_state_t nextState =  Address;
volatile uint8_t i2c1Data;
volatile uint8_t sAddr;
extern unsigned char  iic_buf[255];
extern unsigned char* iic_buf_ptr;

volatile uint8_t    i2c_slave_writeData[256];
volatile uint8_t   *i2c_slave_writeDataPtr;
volatile uint8_t   *i2c_slave_writeDataEndPtr;

/**
 Section: Local Functions
 */

void i2c_slave_open(void) {

    i2c_slave_setIsrHandler(i2c_slave_ISR);
    i2c1_driver_initSlaveHardware();
    i2c1_driver_setAddr(I2C1_SLAVE_ADDRESS << 1);
    i2c1_driver_setMask(I2C1_SLAVE_MASK);
    i2c1_driver_setBusCollisionISR(i2c_slave_BusCollisionISR);
    i2c_slave_setWriteIntHandler(i2c_slave_DefWrInterruptHandler);
    i2c_slave_setReadIntHandler(i2c_slave_DefRdInterruptHandler);
    i2c_slave_setAddrIntHandler(i2c_slave_DefAddrInterruptHandler);
    i2c_slave_setWCOLIntHandler(i2c_slave_BusCollisionISR);
    
    iic_buf_ptr = iic_buf;
    i2c_slave_writeDataPtr = i2c_slave_writeData;
    i2c_slave_writeDataEndPtr = i2c_slave_writeData;
}

void i2c_slave_close(void) {
    i2c1_driver_close();
}

/*-----------------------------------------------------------------------------
 I2C ISR
 
 Entered if address match occurs 
 -----------------------------------------------------------------------------*/
void i2c_slave_ISR(void) {
    uint8_t data; 
    
    state = i2c1_driver_isRead() ? I2C_TX : I2C_RX;// : I2C_TX;
    data = i2c_slave_read(); 
    *iic_buf_ptr = data;
    
    switch(state){
        case I2C_TX:
            if(0 == i2c1_driver_isWriteCollision()) //if there is no write collision
                 i2c_slave_WrCallBack();
            else
                {
                  i2c_slave_WCOLCallBack();
                  i2c1_driver_restart();
                }
            break;
           
        case I2C_RX:
            i2c_slave_RdCallBack();

        default:
            break;
    }
    
    i2c1_driver_releaseClock(); /* Signal that we're done with current data \ Allows master to send more data */
    SSPCON1bits.SSPOV = 0; /* Clear overflow flag */
    mssp1_clearIRQ(); /* Prepare for next data */
}

void i2c_slave_BusCollisionISR(void) {
    SSPCON1bits.WCOL = 0;/* Clear Collision bit */
}

uint8_t i2c_slave_read(void) {
    return i2c1_driver_getRXData();
}

/* 
 * Copies data into provided buffer (up to sz bytes). 
 * Returns bytes of data read. 
 */
unsigned char*  i2c_slave_getTxDataPtr(void){
    uint8_t *ptr; 
    bool tx_complete;
    
    {
        ptr = (uint8_t*)i2c_slave_writeDataPtr;
        i2c_slave_writeDataPtr++;
    }
    
    tx_complete = i2c_slave_writeDataPtr >= i2c_slave_writeDataEndPtr;

    if( tx_complete ){
        i2c_slave_writeDataPtr = i2c_slave_writeData;
        i2c_slave_writeDataEndPtr = i2c_slave_writeData;
    }
 
    return(ptr);
}

void i2c_slave_write(uint8_t data) {
    i2c1_driver_TXData(data);
}

void i2c_slave_write_data(uint8_t* data, uint8_t sz) {
    uint8_t i;
    uint8_t* end;
    end = ( (uint8_t*)i2c_slave_writeData + sizeof( i2c_slave_writeData ) );
    for(i=0; i<sz && (i2c_slave_writeDataEndPtr < end); i++ ){
        *i2c_slave_writeDataEndPtr = data[i];
        i2c_slave_writeDataEndPtr++;
        }
}


void i2c_slave_enable(void) {
    i2c1_driver_initSlaveHardware();
}

void i2c_slave_sendAck(void) {
    i2c1_driver_sendACK();
}

void i2c_slave_sendNack(void) {
    i2c1_driver_sendNACK();
}

// Read Event Interrupt Handlers
void i2c_slave_RdCallBack(void) {
    // Add your custom callback code here
    if (i2c_slave_RdInterruptHandler) {
        i2c_slave_RdInterruptHandler();
    }
}

void i2c_slave_setIsrHandler(interruptHandler handler) {
    i2c1_driver_setI2cISR(handler);
}

void i2c_slave_setBusCollisionISR(interruptHandler handler) {
    i2c1_driver_setBusCollisionISR(handler);
}

void i2c_slave_setReadIntHandler(interruptHandler handler) {
    i2c_slave_RdInterruptHandler = handler;
}

void i2c_slave_DefRdInterruptHandler(void) {
    char data; 
    
    data = i2c1_driver_getRXData();
    *iic_buf_ptr = data;
    /* Read data to clear buffer - Don't advance pointer for address */                
    if (1 == i2c1_driver_isData()){
        switch(data){
            default:
                iic_buf_ptr++;
                iic_buf_ptr = ((int)iic_buf_ptr < ((int)iic_buf + sizeof(iic_buf))) ?  iic_buf_ptr : iic_buf;
                break;
                
            case I2C_MSG_STOP_BYTE:
                if( *(iic_buf_ptr - 1) == I2C_MSG_PRIME_BYTE){
                    KegMaster_procMsg((void*)iic_buf);
                    iic_buf_ptr = iic_buf; // Reset input 
                }
                break;
        }

        /* Handle buffer overflow condition */
    } else {
        /* Do things with address if desired
           - For now, nothing. Maybe something later  */
        i2c_slave_AddrCallBack();
    }
    
           
}

// Write Event Interrupt Handlers
void i2c_slave_WrCallBack(void) {
    // Add your custom callback code here
    if (i2c_slave_WrInterruptHandler) {
        i2c_slave_WrInterruptHandler();
    }
}

void i2c_slave_setWriteIntHandler(interruptHandler handler) {
    i2c_slave_WrInterruptHandler = handler;
}

void i2c_slave_DefWrInterruptHandler(void) {
    uint8_t *dataPtr;
    uint8_t data;
    
    SSPSTATbits.BF = 0;  /* Set buffer has no data */
    
    dataPtr = i2c_slave_getTxDataPtr();
    if( dataPtr != NULL 
     && dataPtr != i2c_slave_writeDataEndPtr ){
        data = *dataPtr;
        i2c_slave_write(data);
    }
}

// Address Event Interrupt Handlers
void i2c_slave_AddrCallBack(void) {
    // Add your custom callback code here
    if (i2c_slave_AddrInterruptHandler) {
        i2c_slave_AddrInterruptHandler();
    }
}

void i2c_slave_setAddrIntHandler(interruptHandler handler) {
    i2c_slave_AddrInterruptHandler = handler;
}

void i2c_slave_DefAddrInterruptHandler(void) {
    sAddr = i2c1_driver_getAddr();
}

// Collision Event Interrupt Handlers
void  i2c_slave_WCOLCallBack(void) {
    // Add your custom callback code here
    if ( i2c_slave_WCOLInterruptHandler) {
         i2c_slave_WCOLInterruptHandler();
    }
}

void i2c_slave_setWCOLIntHandler(interruptHandler handler) {
    i2c_slave_WCOLInterruptHandler = handler;
}

void i2c_slave_DefWCOLInterruptHandler(void) {
}


