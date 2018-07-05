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
  SPI_DATA_ORDER_MSB_FIRST,
  SPI_DATA_ORDER_LSB_FIRST
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
  SPI_IDLE_SIGNAL_LOW,
  SPI_IDLE_SIGNAL_HIGH,
};

typedef struct PeripheralSPI {
  volatile uint8_t *data_direction_register;
  volatile uint8_t *data_register;
  uint8_t select_chip_pin_number;
  uint8_t clock_rate_divider;
  uint8_t data_order;
  uint8_t clock_polarity;
  uint8_t clock_phase;
  uint8_t idle_signal;
} PeripheralSPI;

typedef struct SPIConfig {
  volatile uint8_t *control_register;
  volatile uint8_t *status_register;
  volatile uint8_t *data_register;
  volatile uint8_t *io_lines_data_direction_register;
  uint8_t miso_pin;
  uint8_t mosi_pin;
  uint8_t clock_pin;
} SPIConfig;


size_t PeripheralInterfaceSPI_requiredSize(void);
PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t * const memory, const SPIConfig * const spiConfig);

#endif //COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
