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

};

#endif //COMMUNICATIONMODULE_SPI_H
