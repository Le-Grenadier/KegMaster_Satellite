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

#include "gpio.h"
#include "i2c_slave.h"

#include "tsk_timer.h"
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
volatile uint8_t  iic_rx_buf[90];
volatile uint8_t* iic_rx_ptr;

volatile uint8_t    iic_tx_buf[40];
volatile uint8_t   *iic_tx_ptr;
volatile uint8_t   *iic_tx_endPtr;
volatile uint24_t   iic_data_tmout;

/**
 Section: Local Functions
 */

void i2c_slave_open(void) {
    uint8_t addr_ofst;
    addr_ofst = I2C1_SLAVE_ADDRESS;
    addr_ofst += 1 * gpio_inputStateGet(0);
    addr_ofst += 2 * gpio_inputStateGet(1);

    i2c_slave_setIsrHandler(i2c_slave_ISR);
    i2c1_driver_initSlaveHardware();
    i2c1_driver_setAddr(addr_ofst << 1);
    i2c1_driver_setMask(I2C1_SLAVE_MASK);
    i2c1_driver_setBusCollisionISR(i2c_slave_BusCollisionISR);
    i2c_slave_setWriteIntHandler(i2c_slave_DefWrInterruptHandler);
    i2c_slave_setReadIntHandler(i2c_slave_DefRdInterruptHandler);
    i2c_slave_setAddrIntHandler(i2c_slave_DefAddrInterruptHandler);
    i2c_slave_setWCOLIntHandler(i2c_slave_BusCollisionISR);
    
    iic_rx_ptr = iic_rx_buf;
    iic_tx_ptr = iic_tx_buf;
    iic_tx_endPtr = iic_tx_buf;
}

/*-----------------------------------------------------------------------------
 I2C ISR
 
 Entered if address match occurs 
 -----------------------------------------------------------------------------*/
void i2c_slave_ISR(void) {
    uint8_t data; 
        
    /* Reset data buffers if starting a new interaction                            */
    /* This hopes to catch any discrepancies in data size between master and slave */
    if(TSK_timer_get() > (iic_data_tmout+5*TSK_TICKS_PER_MSEC)){
        iic_rx_ptr = iic_rx_buf; 
		iic_tx_ptr = iic_tx_buf;
    }
    iic_data_tmout = TSK_timer_get();
    
    state = i2c1_driver_isRead() ? I2C_TX : I2C_RX;// : I2C_TX;
    data = i2c_slave_read(); 
    *iic_rx_ptr = data;
    
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
    i2c1_driver_clearBusCollision();
}

uint8_t i2c_slave_read(void) {
    return i2c1_driver_getRXData();
}

/*---------------------------------------------------------
 Returns pointer to next byte of data 
 ---------------------------------------------------------*/
unsigned char*  i2c_slave_getTxDataPtr(void){
    uint8_t *ptr; 
    bool tx_complete;
    
    ptr = (uint8_t*)iic_tx_ptr;
    iic_tx_ptr++;
    
    tx_complete = iic_tx_ptr >= iic_tx_endPtr;

    if( tx_complete ){
        iic_tx_ptr = iic_tx_buf;
        iic_tx_endPtr = iic_tx_buf;
    }
 
    return(ptr);
}

void i2c_slave_write(uint8_t data) {
    i2c1_driver_TXData(data);
}

void i2c_slave_write_data(uint8_t* data, uint8_t sz) {
    uint8_t i;
    uint8_t* end;

    end = (uint8_t*)iic_tx_buf + sizeof(iic_tx_buf);
    
    for(i=0; i<sz && (iic_tx_endPtr < end); i++ ){
        *iic_tx_endPtr = data[i];
        iic_tx_endPtr++;
        }
}


void i2c_slave_enable(void) {
    i2c1_driver_initSlaveHardware();
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
    *iic_rx_ptr = data;
    /* Read data to clear buffer - Don't advance pointer for address */                
    if(i2c1_driver_isData()){
        switch(data){
            case I2C_MSG_STOP_BYTE:
                if( *(iic_rx_ptr - 1) == I2C_MSG_PRIME_BYTE){
                    /* Reset Output - KegMaster_procMsg() will load data */
                    iic_tx_ptr = iic_tx_buf;
                    iic_tx_endPtr = iic_tx_buf;
					
                    KegMaster_procMsg((void*)iic_rx_buf);
                    
                    /* Reset Input */
                    iic_rx_ptr = iic_rx_buf;

                    break;
                }
                
            default:
                iic_rx_ptr++;
                iic_rx_ptr = ((int)iic_rx_ptr < ((int)iic_rx_buf + sizeof(iic_rx_buf))) ?  iic_rx_ptr : iic_rx_buf;
                break;
        }

    } else {
        /* Do things with address if desired
           - For now, nothing. Maybe something later  */
        i2c_slave_AddrCallBack();
    }
    
           
}

/* Write Event Interrupt Handlers */
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
     && dataPtr < iic_tx_endPtr ){
        data = *dataPtr;
        i2c_slave_write(data);
    }
    else{
        /* Error - reset buffers / write zero to wire */
        iic_tx_ptr = iic_tx_buf;
        iic_tx_endPtr = iic_tx_buf;
        i2c_slave_write(0);
    }
}

/* Address Event Interrupt Handlers */
void i2c_slave_AddrCallBack(void) {
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

/* Collision Event Interrupt Handlers */
void  i2c_slave_WCOLCallBack(void) {
    if ( i2c_slave_WCOLInterruptHandler) {
         i2c_slave_WCOLInterruptHandler();
    }
}

void i2c_slave_setWCOLIntHandler(interruptHandler handler) {
    i2c_slave_WCOLInterruptHandler = handler;
}

void i2c_slave_DefWCOLInterruptHandler(void) {
    i2c1_driver_clearBusCollision();
}


