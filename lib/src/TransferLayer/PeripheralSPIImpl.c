//
// Created by Peter Zdankin on 01.04.18.
//


#include <stdbool.h>
#include "lib/include/TransferLayer/PeripheralInterface.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "lib/include/TransferLayer/InterruptData.h"

struct InterruptData{
    uint8_t *buffer;
    uint16_t length;
    uint16_t index;
    bool busy;
};

typedef struct PeripheralInterfaceImpl {
    PeripheralInterface interface;
    PeripheralCallback readCallback;
    void *readCallbackParameter;
    PeripheralCallback writeCallback;
    void *writeCallbackParameter;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    uint8_t f_osc;
    InterruptData interruptData;
} PeripheralInterfaceImpl;



static PeripheralInterface *interfacePTR;


struct Peripheral{
    volatile uint8_t *DDR;
    uint8_t PIN;
    volatile  uint8_t *PORT;
};

void (*handleInterrupt);


static void destroy(PeripheralInterface *self);
static void init(PeripheralInterface *self);
static void writeBlocking(PeripheralInterface *self, const uint8_t *buffer, uint16_t length);
static void readBlocking(PeripheralInterface *self, uint8_t *buffer, uint16_t length);
static void writeNonBlocking(PeripheralInterface *self,const uint8_t *buffer, uint16_t length);
static void setReadCallback(PeripheralInterface *self, PeripheralCallback callback, void *callback_parameter);
static void setWriteCallback(PeripheralInterface *self, PeripheralCallback callback, void *callback_parameter);
static void configurePeripheral(PeripheralInterface *self, Peripheral *device);
static void selectPeripheral(PeripheralInterface *self, Peripheral *device);
static void deselectPeripheral(PeripheralInterface *self, Peripheral *device);

Deallocator (deallocator);

PeripheralInterface * PeripheralInterface_create(TransferLayerConfig transferLayerConfig, SPIConfig spiConfig){
    PeripheralInterfaceImpl *PIImpl = transferLayerConfig.allocate(sizeof(PeripheralInterfaceImpl));
    PIImpl->ddr = spiConfig.ddr;
    PIImpl->port = spiConfig.port;
    PIImpl->spcr = spiConfig.spcr;
    PIImpl->spdr = spiConfig.spdr;
    PIImpl->spsr = spiConfig.spsr;
    PIImpl->f_osc = spiConfig.sck_rate;
    deallocator = transferLayerConfig.deallocate;
    PIImpl->interface.init = init;
    PIImpl->interface.writeNonBlocking = writeNonBlocking;
    PIImpl->interface.writeBlocking = writeBlocking;
    PIImpl->interface.readBlocking = readBlocking;
    PIImpl->interface.destroy = destroy;
    PIImpl->interface.setReadCallback = setReadCallback;
    PIImpl->interface.setWriteCallback = setWriteCallback;
    PIImpl->interface.configurePeripheral = configurePeripheral;
    PIImpl->interface.selectPeripheral = selectPeripheral;
    PIImpl->interface.deselectPeripheral = deselectPeripheral;
    interfacePTR = (PeripheralInterface *)PIImpl;
    return (PeripheralInterface *) PIImpl;
}

static void set_bit(volatile uint8_t *value, uint8_t pin){
    *(value) |= (1<<pin);
}

static void unset_bit(volatile uint8_t *value, uint8_t pin){
    *(value) &= ~(1<<pin);
}

static void set_ddr(PeripheralInterfaceImpl *self){
    set_bit(self->ddr, spi_mosi_pin);
    set_bit(self->ddr, spi_sck_pin);
}

static void set_spcr(PeripheralInterfaceImpl *self){
    set_bit(self->spcr, spi_enable);
    set_bit(self->spcr, spi_master_slave_select);

    //Last 2 bits for f_osc
    *(self->spcr)|=(0b00000011 & self->f_osc);
}

static void set_ss(PeripheralInterfaceImpl *self){
    //Pull up Slave Select Line
    set_bit(self->ddr, spi_ss_pin);
}

static void enableInterrupts(PeripheralInterfaceImpl *self){
    set_bit(self->spcr, spi_interrupt_enable);
}

static void disableInterrupts(PeripheralInterfaceImpl *self){
    unset_bit(self->spcr, spi_interrupt_enable);
}


void init(PeripheralInterface * self){
    PeripheralInterfaceImpl *peripheralImpl = (PeripheralInterfaceImpl *)self;
    set_ddr(peripheralImpl);
    set_spcr(peripheralImpl);
    set_ss(peripheralImpl);
}

static void write(PeripheralInterface *self, uint8_t byte){
    PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *)self;
    *(peripheralSPI->spdr)  = byte;
}

static uint8_t read(PeripheralInterfaceImpl *self){
    return *(self->spdr);
}

static bool spiBusy(PeripheralInterfaceImpl *self){
    return *(self->spsr) & (1<<spi_interrupt_flag);
}



static void writeInInterrupt(){

}

void writeNonBlocking(PeripheralInterface *self,const uint8_t *buffer, uint16_t length){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interfacePTR;
    if(impl->interruptData.busy == false) {
        handleInterrupt = writeInInterrupt;
        impl->interruptData.busy = true;
    }
}



//TODO test this on real hardware
void writeBlocking(PeripheralInterface *self, const uint8_t *buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; ++i) {
        PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
        write(peripheralSPI, buffer[i]);
        while (spiBusy(peripheralSPI)) {}
    }
}

//TODO test this on real hardware
void readBlocking(PeripheralInterface *self, uint8_t *buffer, uint16_t length) {
    PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
    for (uint16_t i = 0; i < length; ++i) {
        write(peripheralSPI, 0x00);
        while (spiBusy(peripheralSPI)) {}
        buffer[i] = read(peripheralSPI);
    }
}



void destroy(PeripheralInterface *self){
    deallocator(self);
}

void setReadCallback(PeripheralInterface *self, PeripheralCallback callback, void *callback_parameter){
    PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *)self;
    peripheralSPI->readCallback = callback;
    peripheralSPI->readCallbackParameter = callback_parameter;
}

void setWriteCallback(PeripheralInterface *self, PeripheralCallback callback, void *callback_parameter){
    PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *)self;
    peripheralSPI->writeCallback = callback;
    peripheralSPI->writeCallbackParameter = callback_parameter;
}

void configurePeripheral(PeripheralInterface *self, Peripheral *device){
    set_bit(device->DDR, device->PIN);
    set_bit(device->PORT, device->PIN);
}

void selectPeripheral(PeripheralInterface *self, Peripheral *device){
    unset_bit(device->PORT, device->PIN);
}

void deselectPeripheral(PeripheralInterface *self, Peripheral *device){
    set_bit(device->PORT, device->PIN);
}