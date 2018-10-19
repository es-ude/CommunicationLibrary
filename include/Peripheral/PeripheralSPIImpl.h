#ifndef COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
#define COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H


#include "include/Peripheral.h"
#include <stddef.h>

enum {
  SPI_DATA_ORDER_MSB_FIRST,
  SPI_DATA_ORDER_LSB_FIRST,
  SPI_CLOCK_RATE_DIVIDER_4,
  SPI_CLOCK_RATE_DIVIDER_8,
  SPI_CLOCK_RATE_DIVIDER_16,
  SPI_CLOCK_RATE_DIVIDER_32,
  SPI_CLOCK_RATE_DIVIDER_64,
  SPI_CLOCK_RATE_DIVIDER_128,
  SPI_CLOCK_POLARITY_LEADING_EDGE_RISING,
  SPI_CLOCK_POLARITY_LEADING_EDGE_FALLING,
  SPI_MODE_0,
  SPI_MODE_1,
  SPI_MODE_2,
  SPI_MODE_3,
  SPI_CLOCK_PHASE_LEADING_EDGE_SAMPLE,
  SPI_CLOCK_PHASE_LEADING_EDGE_SETUP,
  SPI_IDLE_SIGNAL_LOW,
  SPI_IDLE_SIGNAL_HIGH,
};

typedef struct PeripheralSPI {
  volatile uint8_t *data_direction_register;
  volatile uint8_t *data_register;
  uint8_t select_chip_pin_number;
  uint8_t clock_rate_divider;
  uint8_t data_order;
  uint8_t idle_signal;
  uint8_t spi_mode;
} PeripheralSPI;

typedef struct SPIConfig {
  volatile uint8_t *control_register;
  volatile uint8_t *status_register;
  volatile uint8_t *data_register;
  volatile uint8_t *io_lines_data_direction_register;
  volatile uint8_t *io_lines_data_register;
  uint8_t miso_pin;
  uint8_t mosi_pin;
  uint8_t clock_pin;
  uint8_t slave_select_pin;
} SPIConfig;


size_t PeripheralInterfaceSPI_getADTSize(void);

/**
 * The SPIConfig struct handed over to this function needs be alive as long as you
 * are using this PeripheralInterface.
 */
PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t * const memory, const SPIConfig * const spiConfig);

#endif //COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
