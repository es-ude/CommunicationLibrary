#include <stdbool.h>
#include "lib/include/Peripheral.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "lib/include/TransferLayer/InterruptData.h"
#include "CException.h"
#include "lib/include/Exception.h"
#include "lib/src/TransferLayer/SpiPinNumbers.h"

/**
 * \brief
 * Assumptions:
 * - MOSI, MISO, SCK, SS(slave select input) lines all reside on the same port.
 * - we have three registers controlling the hardware spi implementation as described
 *   in the datasheet of e.g. the atmega328p
 *   - these registers behave the same across all our platforms
 */

struct InterruptData {
  uint8_t *buffer;
  uint16_t length;
  uint16_t index;
  bool busy;
};

typedef struct SPIPeripheralOld {
  volatile uint8_t *DDR;
  uint8_t PIN;
  volatile uint8_t *PORT;
} SPIPeripheralOld;

typedef struct PeripheralInterfaceImplOld {
  struct PeripheralInterface interface;
  PeripheralCallback readCallback;
  PeripheralCallback writeCallback;

  volatile uint8_t *ddr;
  volatile uint8_t *port;
  volatile uint8_t *spcr;
  volatile uint8_t *spdr;
  volatile uint8_t *spsr;
  bool clearWriteCallback;
  bool clearReadCallback;
  uint8_t f_osc;

  void (*handleInterrupt)(void);

  InterruptData interruptData;
  SPIPeripheralOld *device;
} PeripheralInterfaceImplOld;

struct NewPeripheralInterfaceImpl {
  struct PeripheralInterface interface;
  const SPIConfig *config;
  PeripheralSPI *current_peripheral;
};

extern void debug(const uint8_t *string);
extern void debugPrintHex(uint8_t byte);

typedef struct NewPeripheralInterfaceImpl *NewPeripheralInterfaceImpl;

static void init(PeripheralInterface self);

static void writeBlockingNew(PeripheralInterface self, const uint8_t *buffer, uint16_t length);

static void readBlockingNew(PeripheralInterface self, uint8_t *buffer, uint16_t length);

static void setClockRateDividerBitValues(volatile uint8_t *control_register, uint8_t value);

static void configurePeripheralNew(Peripheral *device);
static void setClockRateDivider(NewPeripheralInterfaceImpl impl, uint8_t rate);
static void setClockPolarity(volatile uint8_t *control_register, uint8_t polarity);
static void setClockPhase(volatile uint8_t *control_register, uint8_t phase);
static void setDataOrder(volatile uint8_t *control_register, uint8_t data_order);
static void enableDoubleSpeed(volatile uint8_t *status_register);
static void disableDoubleSpeed(volatile uint8_t *status_register);
static void setRegisterWithBitMask(volatile uint8_t *register_ptr, uint8_t bit_mask, uint8_t value);

static void selectPeripheralNew(PeripheralInterface self, Peripheral *device);

static void deselectPeripheralNew(PeripheralInterface self, Peripheral *device);

// returns true on success, false otherwise
static bool tryToClaimInterfaceWithPeripheral(NewPeripheralInterfaceImpl, PeripheralSPI *);

static void set_bit(volatile uint8_t *field, uint8_t bit_number);
static void clear_bit(volatile uint8_t *field, uint8_t bit_number);
static uint8_t get_bit(volatile uint8_t *field, uint8_t bit_number);

Deallocator (deallocator);

size_t PeripheralInterfaceSPI_requiredSize(void) {
  return sizeof(struct NewPeripheralInterfaceImpl);
}

static void new_init(PeripheralInterface self);

static void releaseInterface(NewPeripheralInterfaceImpl impl);

void setInterfaceFunctionPointers(PeripheralInterface self);

void waitUntilByteTransmitted(volatile uint8_t *status_register);

static void activateSlaveSelectLine(PeripheralSPI *spi_chip);

static void deactivateSlaveSelectLine(PeripheralSPI *spi_chip);

static void setUpIOLines(const SPIConfig *config);

static void setUpControlRegister(volatile uint8_t *control_register);

PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t *const memory, const SPIConfig *const spiConfig) {
  NewPeripheralInterfaceImpl impl = (NewPeripheralInterfaceImpl) memory;
  impl->config = spiConfig;
  impl->current_peripheral = NULL;
  setInterfaceFunctionPointers(&impl->interface);
  return (PeripheralInterface) impl;
}

void setInterfaceFunctionPointers(PeripheralInterface self) {
  self->init = new_init;
  self->configurePeripheral = configurePeripheralNew;
  self->selectPeripheral = selectPeripheralNew;
  self->deselectPeripheral = deselectPeripheralNew;
  self->writeBlocking = writeBlockingNew;
  self->readBlocking = readBlockingNew;
}


static void new_init(PeripheralInterface self) {
  NewPeripheralInterfaceImpl impl = (NewPeripheralInterfaceImpl) self;
  // Important: setup of the io lines has to happen before anything else
  setUpIOLines(impl->config);
  setUpControlRegister(impl->config->control_register);
}

static void setUpControlRegister(volatile uint8_t *control_register) {
  set_bit(control_register, spi_enable_bit);
  set_bit(control_register, master_slave_select_bit);
}

static void setUpIOLines(const SPIConfig *config) {
  set_bit(config->io_lines_data_direction_register, config->slave_select_pin);
  set_bit(config->io_lines_data_register, config->slave_select_pin);
  set_bit(config->io_lines_data_direction_register, config->mosi_pin);
  clear_bit(config->io_lines_data_direction_register, config->miso_pin);
  set_bit(config->io_lines_data_direction_register, config->clock_pin);
}

static void configurePeripheralNew(Peripheral *device) {
  PeripheralSPI *spi_chip = (PeripheralSPI *) device;
  set_bit(spi_chip->data_direction_register, spi_chip->select_chip_pin_number);
  deactivateSlaveSelectLine(spi_chip);
}

void selectPeripheralNew(PeripheralInterface self, Peripheral *device) {
  PeripheralSPI *spi_chip = (PeripheralSPI *) device;
  NewPeripheralInterfaceImpl impl = (NewPeripheralInterfaceImpl) self;
  volatile uint8_t *control_register = impl->config->control_register;
  bool claimed = tryToClaimInterfaceWithPeripheral(impl, spi_chip);
  if (claimed) {
    CEXCEPTION_T exception;
    Try {
          setClockRateDivider(impl, spi_chip->clock_rate_divider);
          setSPIMode(control_register, spi_chip->spi_mode);
          setDataOrder(control_register, spi_chip->data_order);
          activateSlaveSelectLine(spi_chip);
        }
    Catch(exception) {
      releaseInterface(impl);
      Throw(exception);
    }
  }
  else {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
}

void activateSlaveSelectLine(PeripheralSPI *spi_chip) {
  if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_LOW) {
    set_bit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_HIGH) {
    clear_bit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else {
    Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

uint8_t transfer(NewPeripheralInterfaceImpl self, uint8_t data) {
  *self->config->data_register = data;
  waitUntilByteTransmitted(self->config->status_register);
  return *self->config->data_register;
}

void writeBlockingNew(PeripheralInterface self, const uint8_t *buffer, uint16_t length) {
  for(uint16_t i = 0; i < length; ++i) {
    transfer((NewPeripheralInterfaceImpl )self, buffer[i]);
  }
}

void readBlockingNew(PeripheralInterface self, uint8_t *buffer, uint16_t length) {
  for(uint16_t i=0; i < length; i++) {
    buffer[i] = transfer((NewPeripheralInterfaceImpl) self, 0);
  }
}

void waitUntilByteTransmitted(volatile uint8_t *status_register) {
  while (!(*status_register & (1 << spi_interrupt_flag_bit))) {}
}

void releaseInterface(NewPeripheralInterfaceImpl impl) {
  impl->current_peripheral = NULL;
}

static bool tryToClaimInterfaceWithPeripheral(NewPeripheralInterfaceImpl impl, PeripheralSPI *device) {
  bool claimed = false;
  if (impl->current_peripheral == NULL) {
    claimed = true;
    impl->current_peripheral = device;
  }
  return claimed;
}

static void deselectPeripheralNew(PeripheralInterface self, Peripheral *device) {
  NewPeripheralInterfaceImpl impl = (NewPeripheralInterfaceImpl) self;

  if (device == impl->current_peripheral) {
    deactivateSlaveSelectLine(impl->current_peripheral);
    releaseInterface(impl);
  }
  else {
    Throw(PERIPHERAL_INTERFACE_DESELECTED_WRONG_PERIPHERAL_EXCEPTION);
  }
}

static void deactivateSlaveSelectLine(PeripheralSPI *spi_chip) {
  if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_LOW) {
    clear_bit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_HIGH) {
    set_bit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else {
    Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

static void setClockRateDivider(NewPeripheralInterfaceImpl impl, uint8_t rate_divider) {
  volatile uint8_t *control_register = impl->config->control_register;
  volatile uint8_t *status_register = impl->config->status_register;
  switch(rate_divider) {

    case SPI_CLOCK_RATE_DIVIDER_4:
      setClockRateDividerBitValues(control_register, 0b00);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_16:
      setClockRateDividerBitValues(control_register, 0b01);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_32:
      setClockRateDividerBitValues(control_register, 0b01);
      enableDoubleSpeed(status_register);

    case SPI_CLOCK_RATE_DIVIDER_64:
      setClockRateDividerBitValues(control_register, 0b10);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_128:
      setClockRateDividerBitValues(control_register, 0b11);
      disableDoubleSpeed(status_register);
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

void setClockRateDividerBitValues(volatile uint8_t *control_register, uint8_t values) {
  uint8_t bit_mask = 0b11;
  setRegisterWithBitMask(control_register, bit_mask, values);
}

void enableDoubleSpeed(volatile uint8_t *status_register) {
  set_bit(status_register, double_spi_speed_bit);
}

void disableDoubleSpeed(volatile uint8_t *status_register) {
  clear_bit(status_register, double_spi_speed_bit);
}

void setSPIMode(volatile uint8_t *control_register, uint8_t spi_mode) {
  void (*set_clock_polarity) (volatile uint8_t *, uint8_t);
  void (*set_clock_phase) (volatile uint8_t *, uint8_t);
  switch(spi_mode) {
    case SPI_MODE_0:
      set_clock_polarity = clear_bit;
      set_clock_phase = clear_bit;
      break;

    case SPI_MODE_1:
      set_clock_polarity = clear_bit;
      set_clock_phase = set_bit;
      break;

    case SPI_MODE_2:
      set_clock_polarity = set_bit;
      set_clock_phase = clear_bit;
      break;

    case SPI_MODE_3:
      set_clock_polarity = set_bit;
      set_clock_phase = set_bit;
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
  set_clock_phase(control_register, clock_phase_bit);
  set_clock_polarity(control_register, clock_polarity_bit);
}

void setClockPolarity(volatile uint8_t *control_register, uint8_t polarity) {
  switch (polarity) {

    case SPI_CLOCK_POLARITY_LEADING_EDGE_FALLING:
      set_bit(control_register, clock_polarity_bit);
      break;

    case SPI_CLOCK_POLARITY_LEADING_EDGE_RISING:
      clear_bit(control_register, clock_polarity_bit);
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

void setClockPhase(volatile uint8_t *control_register, uint8_t phase) {
  switch (phase) {

    case SPI_CLOCK_PHASE_LEADING_EDGE_SAMPLE:
      clear_bit(control_register, clock_phase_bit);
      break;

    case SPI_CLOCK_PHASE_LEADING_EDGE_SETUP:
      set_bit(control_register, clock_phase_bit);
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

void setDataOrder(volatile uint8_t *control_register, uint8_t data_order) {
  switch (data_order) {

    case SPI_DATA_ORDER_LSB_FIRST:
      set_bit(control_register, data_order_bit);
      break;

    case SPI_DATA_ORDER_MSB_FIRST:
      clear_bit(control_register, data_order_bit);
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);

  }
}

/**
 * Set a specified bit high
 *
 * uint8_t val = 0;
 * uint8_t *ptr = &val;
 * set_bit(ptr, 3);
 * TEST_ASSERT_EQUAL_UINT8(0b00001000, val);
 * @param value - Pointer to a 8 bit value
 * @param pin - The bit to set high
 */
static void set_bit(volatile uint8_t *value, uint8_t pin) {
  *(value) |= (1 << pin);
}

/**
 * Set a specified bit low
 *
 * uint8_t val = 255;
 * uint8_t *ptr = &val;
 * unset_bit(ptr, 3);
 * TEST_ASSERT_EQUAL_UINT8(0b11110111, val);
 * @param value - Pointer to a 8 bit value
 * @param pin - The bit to set low
 */
static void clear_bit(volatile uint8_t *value, uint8_t pin) {
  *(value) &= ~(1 << pin);
}

/**
 * Set Master Out Slave In (MOSI) and Serial Clock (SCK) to Output as we are using Master Mode.
 * Configure SS as output and pull high
 * @param self - The PeripheralInterface
 */
static void set_ddr(PeripheralInterfaceImplOld *self) {
  set_bit(self->ddr, spi_mosi_pin);
  clear_bit(self->ddr, spi_miso_pin);
  set_bit(self->ddr, spi_sck_pin);
  set_bit(self->ddr, spi_ss_pin);
  set_bit(self->port, spi_ss_pin);
}

/**
 * Enable SPI and use Master Mode. Additionally set the SPI speed
 * @param self - The PeripheralInterface
 */
static void set_spcr(PeripheralInterfaceImplOld *self) {
  set_bit(self->spcr, spi_enable);                    //Bit 6
  clear_bit(self->spcr, spi_data_order);              //Bit 5
  set_bit(self->spcr, spi_master_slave_select);       //Bit 4
  clear_bit(self->spcr, spi_clock_polarity);          //Bit 3
  clear_bit(self->spcr, spi_clock_phase);             //Bit 2


  //Last 2 bits are f_osc
  switch (self->f_osc) {
    case f_osc_4: {
      clear_bit(self->spcr, spi_clock_rate_select_0);
      clear_bit(self->spcr, spi_clock_rate_select_1);
      break;
    }
    case f_osc_16: {
      set_bit(self->spcr, spi_clock_rate_select_0);
      clear_bit(self->spcr, spi_clock_rate_select_1);
      break;
    }
    case f_osc_64: {
      clear_bit(self->spcr, spi_clock_rate_select_0);
      set_bit(self->spcr, spi_clock_rate_select_1);
      break;
    }
    case f_osc_128: {
      set_bit(self->spcr, spi_clock_rate_select_0);
      set_bit(self->spcr, spi_clock_rate_select_1);
      break;
    }
    default:
      //TODO: raise exception here
      break;
  }

}

/**
 * Initialize all necessary things to enable SPI
 * @param self - The PeripheralInterface
 */
void init(PeripheralInterface self) {
  PeripheralInterfaceImplOld *peripheralImpl = (PeripheralInterfaceImplOld *) self;
  set_ddr(peripheralImpl);
  set_spcr(peripheralImpl);
}




void setRegisterWithBitMask(volatile uint8_t *register_ptr, uint8_t bitmask, uint8_t value) {
  *register_ptr = (~bitmask & *register_ptr) | (bitmask & value);
}

uint8_t get_bit(volatile uint8_t *field, uint8_t bit_number) {
  return (uint8_t) ((*field >> bit_number) & 1);
}