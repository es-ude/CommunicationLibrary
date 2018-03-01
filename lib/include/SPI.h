//
// Created by Peter Zdankin on 28.02.18.
//

#ifndef COMMUNICATIONMODULE_SPI_H
#define COMMUNICATIONMODULE_SPI_H
#include <stdint.h>



typedef struct SPI SPI;

struct SPI{

    /**
    * The init function should set all register flags necessary to use SPI in Master configuration
    */
    void (*initSPI)(SPI *self);

    /**
     * Write a byte to the SPDR
     * @param self - the SPI Master
     * @param data - a byte
     */
    void (*writeToSPDR)(SPI *self, uint8_t data);

    /**
     * Read a byte from SPDR
     * @param self - the SPI Master
     * @return a byte received
     */
    uint8_t (*readFromSPDR)(SPI *self);


    //TODO Delete this after some iterations
    //You would expect to have such a method
    //As we need async transmission, reading from SPDR will only happen after an interrupt.
    //If we write, wait for the interrupt and then read, we have to wait.
    //Therefore we need a write and a read method
        /**
        * transmits a single byte over spi and returns the received message
        * @param data - uint8_t to transmit
        * @return - Received uint8_t
        */
        //uint8_t (*transmit)(SPI *self, uint8_t data);
};

#endif //COMMUNICATIONMODULE_SPI_H
