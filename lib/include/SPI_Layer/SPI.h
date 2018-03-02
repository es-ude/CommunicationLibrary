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
};

#endif //COMMUNICATIONMODULE_SPI_H
