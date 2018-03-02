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
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    uint8_t f_osc;
} SPIImpl;

static void init(SPI *self);
static void writeToSPDR(SPI *self, uint8_t data);
static uint8_t readFromSPDR(SPI *self);

SPI * SPI_createSPI(volatile uint8_t *ddr, volatile uint8_t *spcr, volatile uint8_t *spdr, uint8_t sck_rate, Allocator allocate) {
    SPIImpl *implementation = allocate(sizeof(SPIImpl));
    implementation->ddr = ddr;
    implementation->spcr = spcr;
    implementation->spdr = spdr;
    implementation->f_osc = sck_rate;
    implementation->interface.initSPI = init;
    implementation->interface.writeToSPDR = writeToSPDR;
    implementation->interface.readFromSPDR = readFromSPDR;
    return (SPI*) implementation;
}

static void set_ddr(SPIImpl *self){
    *(self->ddr) |= (1<<spi_mosi_pin)|(1<<spi_sck_pin);
}

static void set_spcr(SPIImpl *self){
    *(self->spcr)|=(1<<spi_enable)|(1<<spi_master_slave_select);
    *(self->spcr)|=(0b00000011 & self->f_osc);
}

void init(SPI *self){
    SPIImpl *spi = (SPIImpl *)self;
    set_ddr(spi);
    set_spcr(spi);
}


void writeToSPDR(SPI *self, uint8_t data){
    SPIImpl *spi = (SPIImpl *)self;
    *(spi->spdr) = data;
}

uint8_t readFromSPDR(SPI *self){
    SPIImpl *spi = (SPIImpl *)self;
    return *(spi->spdr);
}