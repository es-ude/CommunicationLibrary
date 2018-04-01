//
// Created by Peter Zdankin on 01.04.18.
//


#include "lib/include/TransferLayer/PeripheralInterface.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"

typedef struct SPIImpl {
    PeripheralInterface interface;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    uint8_t f_osc;
} SPIImpl;


typedef struct PeripheralSPIImpl{
    PeripheralInterface interface;
    SPIImpl *spi;
} PeripheralSPIImpl;

void destroy(PeripheralInterface *self);
void init(PeripheralInterface *self);



Deallocator (deallocator);

PeripheralInterface * PeripheralInterface_create(TransferLayerConfig transferLayerConfig, SPIConfig spiConfig){
    PeripheralSPIImpl *PIImpl = transferLayerConfig.allocate(sizeof(PeripheralSPIImpl));
    PIImpl->spi = transferLayerConfig.allocate(sizeof(SPIImpl));
    PIImpl->spi->ddr = spiConfig.ddr;
    PIImpl->spi->port = spiConfig.port;
    PIImpl->spi->spcr = spiConfig.spcr;
    PIImpl->spi->spdr = spiConfig.spdr;
    PIImpl->spi->spsr = spiConfig.spsr;
    PIImpl->spi->f_osc = spiConfig.sck_rate;
    deallocator = transferLayerConfig.deallocate;
    PIImpl->interface.init = init;
    PIImpl->interface.destroy = destroy;
    return (PeripheralInterface *) PIImpl;
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

    //Last 2 bits for f_osc
    *(self->spcr)|=(0b00000011 & self->f_osc);
}

static void set_ss(SPIImpl *self){
    //Pull up Slave Select Line
    set_bit(self->ddr, spi_ss_pin);
}


void init(PeripheralInterface * self){
    PeripheralSPIImpl *peripheralImpl = (PeripheralSPIImpl *)self;
    SPIImpl *spi = peripheralImpl->spi;
    set_ddr(spi);
    set_spcr(spi);
    set_ss(spi);
}

void destroy(PeripheralInterface *self){
    PeripheralSPIImpl *peripheralSPI = (PeripheralInterface *)self;
    deallocator(peripheralSPI->spi);
    deallocator(self);
}