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

enum {
  SPI_DATA_ORDER_MSB,
  SPI_DATA_ORDER_LSB
};

enum {
  SPI_CLOCK_RATE_DIVIDER_4,
  SPI_CLOCK_RATE_DIVIDER_16,
  SPI_CLOCK_RATE_DIVIDER_32,
  SPI_CLOCK_RATE_DIVIDER_64,
  SPI_CLOCK_RATE_DIVIDER_128,
};

enum {
  SPI_CLOCK_POLARITY_LEADING_EDGE_RISING,
  SPI_CLOCK_POLARITY_LEADING_EDGE_FALLING,
};

enum {
  SPI_CLOCK_PHASE_LEADING_EDGE_SAMPLE,
  SPI_CLOCK_PHASE_LEADING_EDGE_SETUP,
};

enum {
  SPI_DOUBLE_SPEED_ENABLED,
  SPI_DOUBLE_SPEED_DISABLED,
};

typedef struct SPIPeripheralNew {
  volatile uint8_t *data_direction_register;
  volatile uint8_t *data_register;
  uint8_t select_chip_pin_number;
  uint8_t clock_rate_divider;
  uint8_t data_order;
  uint8_t clock_polarity;
  uint8_t clock_phase;
  uint8_t double_speed_enabled;
} SPIPeripheralNew;

typedef struct SPIConfigNew {
  volatile uint8_t *control_register;
  volatile uint8_t *status_register;
  volatile uint8_t *data_register;
  volatile uint8_t *io_lines_data_register;
  volatile uint8_t *io_lines_data_direction_register;
  uint8_t slave_select_input_pin;
  uint8_t miso_pin;
  uint8_t mosi_pin;
  uint8_t clock_pin;
} SPIConfigNew;

typedef struct SPIConfig{
  volatile uint8_t *ddr;
  volatile uint8_t *port;
  volatile uint8_t *spcr;
  volatile uint8_t *spdr;
  volatile uint8_t *spsr;
  enum sck_rate sck_rate;
  uint8_t miso_pin;
  uint8_t mosi_pin;
  uint8_t clock_pin;

} SPIConfig;


size_t PeripheralInterfaceSPI_requiredSize();

PeripheralInterface PeripheralInterface_create(TransferLayerConfig transferLayerConfig, SPIConfig spiConfig);
PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t * const memory, const SPIConfigNew * const spiConfig);

#endif //COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
