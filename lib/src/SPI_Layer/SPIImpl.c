//
// Created by Peter Zdankin on 28.02.18.
//

#include <stdint.h>
#include "lib/include/SPI_Layer/SPI.h"
#include "lib/include/SPI_Layer/SPIImpl.h"
#include "lib/include/platform/io.h"

typedef struct SPIImpl {
    SPI interface;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    uint8_t f_osc;
} SPIImpl;

static void init(SPI *self);
static void writeToSPDR(SPI *self, uint8_t data);
static uint8_t readFromSPDR(SPI *self);


//The Slave selection is pretty decoupled from the rest of SPI, we might consider placing it in its own module
static void selectSlave(volatile uint8_t *PORT, uint8_t pin);
static void deselectSlave(volatile uint8_t *PORT, uint8_t pin);
static void configureAsSlave(volatile uint8_t *ddr, uint8_t pin,volatile  uint8_t *port);

SPI * SPI_createSPI(SPIConfig config) {
    SPIImpl *implementation = config.allocate(sizeof(SPIImpl));
    implementation->ddr = config.ddr;
    implementation->port = config.port;
    implementation->spcr = config.spcr;
    implementation->spdr = config.spdr;
    implementation->f_osc = config.sck_rate;
    implementation->interface.initSPI = init;
    implementation->interface.writeToSPDR = writeToSPDR;
    implementation->interface.readFromSPDR = readFromSPDR;
    implementation->interface.configureAsSlave = configureAsSlave;
    implementation->interface.selectSlave = selectSlave;
    implementation->interface.deselectSlave = deselectSlave;
    return (SPI*) implementation;
}

static void set_bit(volatile uint8_t *value, uint8_t pin){
    *(value) |= (1<<pin);
}

static void unset_bit(volatile uint8_t *value, uint8_t pin){
    *(value) &= ~(1<<pin);
}

static void set_ddr(SPIImpl *self){
    set_bit(self->ddr, spi_mosi_pin);
    set_bit(self->ddr, spi_sck_pin);
}

static void set_spcr(SPIImpl *self){
    set_bit(self->spcr, spi_enable);
    set_bit(self->spcr, spi_master_slave_select);

    *(self->spcr)|=(0b00000011 & self->f_osc);
}

static void selectSS(SPIImpl *self){
    set_bit(self->ddr, spi_ss_pin);
}

void init(SPI *self){
    SPIImpl *spi = (SPIImpl *)self;
    set_ddr(spi);
    set_spcr(spi);
    selectSS(spi);
}


void writeToSPDR(SPI *self, uint8_t data){
    SPIImpl *spi = (SPIImpl *)self;
    *(spi->spdr) = data;
}

uint8_t readFromSPDR(SPI *self){
    SPIImpl *spi = (SPIImpl *)self;
    return *(spi->spdr);
}

/**
 * Configure a slave by
 *  1) Setting their pin as output in the Data Definition Register
 *  2) Writing a '1' to their port
 */
void configureAsSlave(volatile uint8_t *ddr, uint8_t pin,volatile  uint8_t *port){
    set_bit(ddr, pin);
    set_bit(port,pin);
}


void selectSlave(volatile uint8_t *PORT, uint8_t pin){
    unset_bit(PORT, pin);
}

void deselectSlave(volatile uint8_t *PORT, uint8_t pin){
    set_bit(PORT, pin);
}