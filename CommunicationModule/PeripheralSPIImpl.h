#ifndef COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
#define COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H


#include "CommunicationModule/Peripheral.h"
#include <stddef.h>

/*!
 * \file PeripheralSPIImpl.h

 * \brief SPI Implementation of the PeripheralInterface

 *  This header provides all declarations
 *  needed to initialize the driver for the SPI
 *  hardware implementation
 *  for the PeripheralInterface interface.
 *  The implementation is supposed to support
 *  all atmega platforms by just correctly defining
 *  the SPIConfig and SPISlave data types.
 *  Here an example configuration for an atmega32u4
 *  where the io pin for the connected slave is the
 *  fifth pin (we start counting at 0) on port B
 *
 ~~~
 *      SPISlave mrf_spi_client = {
 *          .data_register = &PORTB,
 *          .data_direction_register = &DDRB,
 *          .select_chip_pin_number = 4,
 *          .data_order = SPI_DATA_ORDER_MSB_FIRST,
 *          .spi_mode = SPI_MODE_0,
 *          .idle_signal = SPI_IDLE_SIGNAL_HIGH,
 *          .clock_rate_divider = SPI_CLOCK_RATE_DIVIDER_64,
 *      };
 ~~~
 *
 * And the configuration for the spi hardware interface on
 * that platform after including `<avr/io.h>`
 *
 *~~~
 * SPIConfig spi_config = {
 *   .data_register = &SPDR,
     .clock_pin = PORTB1,
 *   .miso_pin = PORTB3,
 *   .mosi_pin = PORTB2,
 *   .slave_select_pin = PORTB0,
 *   .io_lines_data_direction_register = &DDRB,
 *   .io_lines_data_register = &PORTB,
 *   .status_register = &SPSR,
 *   .control_register = &SPCR,
 * };
 *~~~
 *
 */

/*! These values are used to configure the spi hardware interface
 *
 * Values from a set of names sharing the same biggest possible prefix
 * are mutually exclusive. E.g. you can obviously only use one single value
 * for your clock rate divider.
 * To find out which set of values you need to use for your use case,
 * have look at the datasheet of the peripheral device you want to interact with.
 * Choose a clock rate divider to match a supported clock rate of your peripheral
 * device depending on the clock rate of your microcontroller. E.g. your peripheral
 * device datasheet recommends a clock rate of 1 MHz and your microcontroller runs
 * at 16MHz, so you choose SPI_CLOCK_RATE_DIVIDER_16.
 */
enum SPIControlRegisterParameters {
  SPI_DATA_ORDER_MSB_FIRST,
  SPI_DATA_ORDER_LSB_FIRST,
  SPI_CLOCK_RATE_DIVIDER_4,
  SPI_CLOCK_RATE_DIVIDER_8,
  SPI_CLOCK_RATE_DIVIDER_16,
  SPI_CLOCK_RATE_DIVIDER_32,
  SPI_CLOCK_RATE_DIVIDER_64,
  SPI_CLOCK_RATE_DIVIDER_128,
  SPI_MODE_0,
  SPI_MODE_1,
  SPI_MODE_2,
  SPI_MODE_3,
  SPI_IDLE_SIGNAL_LOW,
  SPI_IDLE_SIGNAL_HIGH,
};

/*! Software representation of the spi slave connected to the hosts spi hardware interface
 *
 */
typedef struct SPISlave {
  volatile uint8_t *data_direction_register; /*!< this has to match the port the slave select line is connected to */
  volatile uint8_t *data_register; /*!< data register for controlling the slave select line */
  uint8_t slave_select_pin_number; /*!< set this to match the pin number your slave select line is connected to */
  uint8_t clock_rate_divider; /*!< use this to divide the microcontrollers clock rate to match the spi clock rate supported by the slave */
  uint8_t data_order; /*!< whether to transmit each byte with the most or least significant bit leading */
  uint8_t idle_signal; /*!< configure whether the idle signal for your slave is supposed to be low or high */
  uint8_t spi_mode; /*!< the spi mode to use with your slave (this information should be included in its datasheet */
} SPISlave;

/*! Configuration struct for the hardware spi interface.
 * For information on how to set this up, look at the example above and your microcontrollers datasheet.
 */
typedef struct SPIConfig {
  volatile uint8_t *control_register;
  volatile uint8_t *status_register;
  volatile uint8_t *data_register;
  volatile uint8_t *io_lines_data_direction_register; /*!< the io_lines registers refer to the port used for the spi hardware interface clock pin and slave select pin */
  volatile uint8_t *io_lines_data_register;
  uint8_t miso_pin;
  uint8_t mosi_pin;
  uint8_t clock_pin;
  uint8_t slave_select_pin; /*!< This refers to the pin that can be used by the microcontroller to receive slave select signals. */
} SPIConfig;


/*!  Use this to find out how much memory you need to provide to the create function.
 *
 * @return The size of the SPI implementation.
 */
size_t PeripheralInterfaceSPI_getADTSize(void);

/**
 * The SPIConfig struct handed over to this function needs be alive as long as you
 * are using this PeripheralInterface.
 */
PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t * const memory, const SPIConfig * const spiConfig);

/**
 * Do not use this definition in production code. The structures below
 * are meant to be only used to create new variables of the corresponding type.
 * Everywhere else use the functions to access the data structures.
 *
 * Assumptions:
 * - MOSI, MISO, SCK, SS(slave select input) lines all reside on the same port.
 * - we have three registers controlling the hardware spi implementation as described
 *   in the datasheet of e.g. the atmega328p
 *   - these registers behave the same across all our platforms
 */
typedef struct InterruptData InterruptData;

/**
 * Do not use this struct directly ever. Use the functions above.
 * It is just defined here to allow for fine grained control of the memory
 */
struct InterruptData {
  const uint8_t *output_buffer;
  uint8_t *input_buffer;
  size_t output_buffer_length;
  size_t input_buffer_length;
  PeripheralInterface_Callback write_callback;
  PeripheralInterface_Callback read_callback;
};

/**
 * Do not use this struct directly ever. Use the functions above.
 * It is just defined here to allow for fine grained control of the memory
 */
struct PeripheralInterfaceSPIImpl {
  struct PeripheralInterface interface;
  SPIConfig *config;
  SPISlave *current_peripheral;
  InterruptData interrupt_data;
};

#endif //COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
