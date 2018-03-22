//
// Created by Peter Zdankin on 28.02.18.
//

#ifndef COMMUNICATIONMODULE_SPI_H
#define COMMUNICATIONMODULE_SPI_H
#include <stdint.h>
#include "lib/include/communicationLayer/InterruptData.h"


typedef struct PeripheralInterface PeripheralInterface;

struct PeripheralInterface{

    /**
     * The init function should set all register flags necessary to use SPI in Master configuration
     * @param self - The PeripheralInterface
     */
    void (*init)(PeripheralInterface *self);


    /**
     * Set the Callback to be called, when the specified amount of reading is finished
     * @param callback - function
     */
    void (*setReadCallback)(PeripheralInterface *self,void(*callback)(void));

    /**
     * Set the Callback to be called, when the specified amount of writing is finished
     * @param callback - function
     */
    void (*setWriteCallback)(PeripheralInterface *self,void(*callback)(void));


    /**
     * Write length bytes of data starting at 0
     * @param self - The PeripheralInterface
     * @param data - Pointer to data to be transmitted
     * @param length - Amount Bytes to transmit
     */
    void (*writeBlocking)(PeripheralInterface *self, uint8_t *data, uint16_t length);


    /**
     * Read length bytes into the data buffer
     * @param self - The PeripheralInterface
     * @param data - Pointer to data where to store message
     * @param length - Amount Bytes to read
     * @warning Ensure data is big enough to store length bytes
     */
    void (*readBlocking)(PeripheralInterface *self, uint8_t *data, uint16_t length);

    /**
     * Write a byte
     * @param self - the Peripheral device
     * @param data - a byte
     */
    void (*write)(PeripheralInterface *self, uint8_t data);

    /**
     * Read a byte
     * @param self - the peripheral device
     * @return - a byte received
     */
    uint8_t (*read)(PeripheralInterface *self);


    /**
     * Writes a byte and waits until it gets a byte back
     * Mainly used for synchronous communication
     * @param self - the peripheral device
     * @param data - byte to transfer
     * @return - a byte received
     */
    uint8_t (*transfer)(PeripheralInterface *self, uint8_t data);

    /**
     * Configure the specified pin as a SPI slave by configuring it as an output and pulling it high
     * @param ddr - DDR where the slave pin is located
     * @param pin - The pin of the SPI slave
     * @param port - The PORT where the pin is located
     */
    void (*configureAsSlave)(volatile uint8_t *ddr, uint8_t pin,volatile  uint8_t *port);

    /**
     * Set specified pin down to indicate a transmission
     * @param PORT - PORT where the slave pin is located
     * @param pin - The pin of the SPI slave
     */
    void (*selectSlave)(volatile uint8_t *PORT, uint8_t pin);

    /**
     * Pull specified pin up to indicate the end of a transmission
     * @param PORT - PORT where the slave pin is located
     * @param pin - The pin of the SPI slave
     */
    void (*deselectSlave)(volatile uint8_t *PORT, uint8_t pin);

    /**
     * Handle an interrupt
     */
    void (*handleInterrupt)(PeripheralInterface *self);

    /**
     * Enable peripheralspecific interrupts
     * Does not change global interrupt
     * @param self - The peripheral
     */
    void (*enableInterrupt) (PeripheralInterface *self); //DEPRACATED

    /**
     * Disable peripheral interrupts
     * Does not change global interrupt
     * @param self - The peripheral
     */
    void (*disableInterrupt) (PeripheralInterface *self); //DEPRACATED

    /**
     * Clean up the PeripheralInterface
     * @param self The Interface to be freed
     */
    void (*destroy) (PeripheralInterface *self);


    /**
     * The data the PeripheralInterface needs to access when a peripheralspecific interrupt happens
     */
    InterruptData *interruptData;
};

#endif //COMMUNICATIONMODULE_SPI_H
