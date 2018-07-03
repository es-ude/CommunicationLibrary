//
// Created by Peter Zdankin on 01.04.18.
//

#ifndef COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
#define COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H


#include "lib/include/Peripheral.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/platform/io.h"

typedef struct TransferLayerConfig {
    Allocator  allocate;
    Deallocator deallocate;
} TransferLayerConfig;

typedef struct SPIPeripheralNew {
  volatile uint8_t *data_direction_register;
  volatile uint8_t *data_register;
  uint8_t pin_number;
} SPIPeripheralNew;

typedef struct SPIConfigNew {
  volatile uint8_t *spi_control_register;
  volatile uint8_t *spi_status_register;
  volatile uint8_t *spi_data_register;
  volatile uint8_t *spi_slave_select_input_data_register;
  volatile uint8_t *spi_slave_select_input_data_direction_register;
  uint8_t spi_slave_select_input_pin;
} SPIConfigNew;

typedef struct SPIConfig{
  volatile uint8_t *ddr;
  volatile uint8_t *port;
  volatile uint8_t *spcr;
  volatile uint8_t *spdr;
  volatile uint8_t *spsr;
  enum sck_rate sck_rate;

} SPIConfig;


size_t PeripheralInterfaceSPI_requiredSize();

PeripheralInterface PeripheralInterface_create(TransferLayerConfig transferLayerConfig, SPIConfig spiConfig);
PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t * const memory, const SPIConfigNew * const spiConfig);

#endif //COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
