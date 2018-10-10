#include "lib/src/Peripheral/PeripheralSPIImplIntern.h"
#include "lib/src/BitManipulation.h"


size_t PeripheralInterfaceSPI_requiredSize(void) {
  return sizeof(struct PeripheralInterfaceImpl);
}

PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t *const memory, const SPIConfig *const spiConfig) {
  PeripheralInterfaceImpl impl = (PeripheralInterfaceImpl) memory;
  impl->config = spiConfig;
  impl->current_peripheral = NULL;
  setInterfaceFunctionPointers(&impl->interface);
  return (PeripheralInterface) impl;
}

void setInterfaceFunctionPointers(PeripheralInterface self) {
  self->init = init;
  self->configurePeripheral = configurePeripheralNew;
  self->selectPeripheral = selectPeripheral;
  self->deselectPeripheral = deselectPeripheral;
  self->writeBlocking = writeBlocking;
  self->readBlocking = readBlocking;
}


static void init(PeripheralInterface self) {
  PeripheralInterfaceImpl impl = (PeripheralInterfaceImpl) self;
  // Important: setup of the io lines has to happen before anything else
  setUpIOLines(impl->config);
  setUpControlRegister(impl->config->control_register);
}

static void setUpControlRegister(volatile uint8_t *control_register) {
  BitManipulation_setBit(control_register, spi_enable_bit);
  BitManipulation_setBit(control_register, master_slave_select_bit);
}

static void setUpIOLines(const SPIConfig *config) {
  BitManipulation_setBit(config->io_lines_data_direction_register, config->slave_select_pin);
  BitManipulation_setBit(config->io_lines_data_register, config->slave_select_pin);
  BitManipulation_setBit(config->io_lines_data_direction_register, config->mosi_pin);
  BitManipulation_clearBit(config->io_lines_data_direction_register, config->miso_pin);
  BitManipulation_setBit(config->io_lines_data_direction_register, config->clock_pin);
}

static void configurePeripheralNew(Peripheral *device) {
  PeripheralSPI *spi_chip = (PeripheralSPI *) device;
  BitManipulation_setBit(spi_chip->data_direction_register, spi_chip->select_chip_pin_number);
  deactivateSlaveSelectLine(spi_chip);
}

void selectPeripheral(PeripheralInterface self, Peripheral *device) {
  PeripheralSPI *spi_chip = (PeripheralSPI *) device;
  PeripheralInterfaceImpl impl = (PeripheralInterfaceImpl) self;
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
    BitManipulation_setBit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_HIGH) {
    BitManipulation_clearBit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else {
    Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

uint8_t transfer(PeripheralInterfaceImpl self, uint8_t data) {
  *self->config->data_register = data;
  waitUntilByteTransmitted(self->config->status_register);
  return *self->config->data_register;
}

void writeBlocking(PeripheralInterface self, const uint8_t *buffer, uint16_t length) {
  for(uint16_t i = 0; i < length; ++i) {
    transfer((PeripheralInterfaceImpl )self, buffer[i]);
  }
}

void readBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length) {
  for(uint16_t i=0; i < length; i++) {
    buffer[i] = transfer((PeripheralInterfaceImpl) self, 0);
  }
}

void waitUntilByteTransmitted(volatile uint8_t *status_register) {
  while (!(*status_register & (1 << spi_interrupt_flag_bit))) {}
}

void releaseInterface(PeripheralInterfaceImpl impl) {
  impl->current_peripheral = NULL;
}

static bool tryToClaimInterfaceWithPeripheral(PeripheralInterfaceImpl impl, PeripheralSPI *device) {
  bool claimed = false;
  if (impl->current_peripheral == NULL) {
    claimed = true;
    impl->current_peripheral = device;
  }
  return claimed;
}

static void deselectPeripheral(PeripheralInterface self, Peripheral *device) {
  PeripheralInterfaceImpl impl = (PeripheralInterfaceImpl) self;

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
    BitManipulation_clearBit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else if (spi_chip->idle_signal == SPI_IDLE_SIGNAL_HIGH) {
    BitManipulation_setBit(spi_chip->data_register, spi_chip->select_chip_pin_number);
  }
  else {
    Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
}

static void setClockRateDivider(PeripheralInterfaceImpl impl, uint8_t rate_divider) {
  volatile uint8_t *control_register = impl->config->control_register;
  volatile uint8_t *status_register = impl->config->status_register;
  switch(rate_divider) {

    case SPI_CLOCK_RATE_DIVIDER_4:
      setClockRateDividerBitValues(control_register, 0b00);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_8:
      setClockRateDividerBitValues(control_register, 0b00);
      enableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_16:
      setClockRateDividerBitValues(control_register, 0b01);
      disableDoubleSpeed(status_register);
      break;

    case SPI_CLOCK_RATE_DIVIDER_32:
      setClockRateDividerBitValues(control_register, 0b01);
      enableDoubleSpeed(status_register);
      break;

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
  BitManipulation_setByte(control_register, bit_mask, values);
}

void enableDoubleSpeed(volatile uint8_t *status_register) {
  BitManipulation_setBit(status_register, double_spi_speed_bit);
}

void disableDoubleSpeed(volatile uint8_t *status_register) {
  BitManipulation_clearBit(status_register, double_spi_speed_bit);
}

void setSPIMode(volatile uint8_t *control_register, uint8_t spi_mode) {
  void (*set_clock_polarity) (volatile uint8_t *, uint8_t);
  void (*set_clock_phase) (volatile uint8_t *, uint8_t);
  switch(spi_mode) {
    case SPI_MODE_0:
      set_clock_polarity = BitManipulation_clearBit;
      set_clock_phase = BitManipulation_clearBit;
      break;

    case SPI_MODE_1:
      set_clock_polarity = BitManipulation_clearBit;
      set_clock_phase = BitManipulation_setBit;
      break;

    case SPI_MODE_2:
      set_clock_polarity = BitManipulation_setBit;
      set_clock_phase = BitManipulation_clearBit;
      break;

    case SPI_MODE_3:
      set_clock_polarity = BitManipulation_setBit;
      set_clock_phase = BitManipulation_setBit;
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);
  }
  set_clock_phase(control_register, clock_phase_bit);
  set_clock_polarity(control_register, clock_polarity_bit);
}

void setDataOrder(volatile uint8_t *control_register, uint8_t data_order) {
  switch (data_order) {

    case SPI_DATA_ORDER_LSB_FIRST:
      BitManipulation_setBit(control_register, data_order_bit);
      break;

    case SPI_DATA_ORDER_MSB_FIRST:
      BitManipulation_clearBit(control_register, data_order_bit);
      break;

    default:
      Throw(PERIPHERAL_SELECT_EXCEPTION);

  }
}