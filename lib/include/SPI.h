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
    * transmits a single byte over spi and returns the received message
    * @param data - uint8_t to transmit
    * @return - Received uint8_t
    */
    uint8_t (*transmit)(SPI *self, uint8_t data);
};

#endif //COMMUNICATIONMODULE_SPI_H
