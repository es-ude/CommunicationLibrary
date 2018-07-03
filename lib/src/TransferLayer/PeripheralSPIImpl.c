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

typedef struct SPIPeripheral {
  volatile uint8_t *DDR;
  uint8_t PIN;
  volatile uint8_t *PORT;
} SPIPeripheral;

typedef struct PeripheralInterfaceImpl {
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
  SPIPeripheral *device;
} PeripheralInterfaceImpl;

struct NewPeripheralInterfaceImpl {
  struct PeripheralInterface interface;
  const SPIConfigNew *config;
};

typedef struct NewPeripheralInterfaceImpl *NewPeripheralInterfaceImpl;

static PeripheralInterfaceImpl *interfacePTR;

static void destroy(PeripheralInterface self);

static void init(PeripheralInterface self);

static void initControlRegister(PeripheralInterface self, const SPIConfigNew *config);
static void initStatusRegister(PeripheralInterface self, const SPIConfigNew *config);
static void initIOLine(PeripheralInterface self);
static void setUpDataOrder(PeripheralInterface self);

static void writeBlocking(PeripheralInterface self, const uint8_t *buffer, uint16_t length);

static void readBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length);

static void writeNonBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length);

static void readNonBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length);

static void setReadCallback(PeripheralInterface self, PeripheralCallback callback);

static void setWriteCallback(PeripheralInterface self, PeripheralCallback callback);

static void setCallbackClearFlags(PeripheralInterface self, bool clearReadCallbackOnCall, bool clearWriteCallbackOnCall);

static void configurePeripheral(PeripheralInterface self, Peripheral *device);

static void selectPeripheral(PeripheralInterface self, Peripheral *device);

static void deselectPeripheral(PeripheralInterface self, Peripheral *device);

static bool isBusy(PeripheralInterface self);

static void handleInterrupt();

static void set_bit(volatile uint8_t *field, uint8_t bit_number);

void handleInterrupt() {
  interfacePTR->handleInterrupt();
}

Deallocator (deallocator);

size_t PeripheralInterfaceSPI_requiredSize(void) {
  return sizeof(struct NewPeripheralInterfaceImpl);
}

static void new_init(PeripheralInterface self);

PeripheralInterface PeripheralInterfaceSPI_createNew(uint8_t *const memory, const SPIConfigNew *const spiConfig) {

  NewPeripheralInterfaceImpl impl = (NewPeripheralInterfaceImpl) memory;
  impl->interface.init = new_init;
  impl->config = spiConfig;
  return (PeripheralInterface) impl;
}

static void new_init(PeripheralInterface self) {
  NewPeripheralInterfaceImpl impl = (NewPeripheralInterfaceImpl) self;
  set_bit(impl->config->control_register, spi_enable_bit);
  set_bit(impl->config->control_register, spi_master_slave_select);
}


/**
 * Create a PeripheralInterface, given a TransferlayerConfig and a SPIConfig
 * @param transferLayerConfig - struct with function Pointers to allocation and deallocation function
 * @param spiConfig - struct with &DDRB, &PORTB, &SPCR, &SPDR, &SPSR, and the SPI Speed f_osc
 * @return Pointer to PeripheralInterface
 */
PeripheralInterface PeripheralInterface_create(TransferLayerConfig transferLayerConfig, SPIConfig spiConfig) {
  PeripheralInterfaceImpl *PIImpl = transferLayerConfig.allocate(sizeof(PeripheralInterfaceImpl));
  PIImpl->ddr = spiConfig.ddr;
  PIImpl->port = spiConfig.port;
  PIImpl->spcr = spiConfig.spcr;
  PIImpl->spdr = spiConfig.spdr;
  PIImpl->spsr = spiConfig.spsr;
  PIImpl->f_osc = spiConfig.sck_rate;
  PIImpl->readCallback.function = NULL;
  PIImpl->readCallback.argument = NULL;
  PIImpl->writeCallback.function = NULL;
  PIImpl->writeCallback.argument = NULL;


  PIImpl->interface.init = init;
  PIImpl->interface.writeNonBlocking = writeNonBlocking;
  PIImpl->interface.readNonBlocking = readNonBlocking;
  PIImpl->interface.writeBlocking = writeBlocking;
  PIImpl->interface.readBlocking = readBlocking;
  PIImpl->interface.destroy = destroy;
  PIImpl->interface.setReadCallback = setReadCallback;
  PIImpl->interface.setWriteCallback = setWriteCallback;
  PIImpl->interface.setCallbackClearFlags = setCallbackClearFlags;
  PIImpl->interface.configurePeripheral = configurePeripheral;
  PIImpl->interface.selectPeripheral = selectPeripheral;
  PIImpl->interface.deselectPeripheral = deselectPeripheral;
  PIImpl->interface.isBusy = isBusy;
  PIImpl->interface.handleInterrupt = handleInterrupt;
  PIImpl->interruptData.index = 0;
  PIImpl->interruptData.buffer = NULL;
  PIImpl->interruptData.busy = false;
  PIImpl->interruptData.length = 0;
  PIImpl->handleInterrupt = NULL;

  deallocator = transferLayerConfig.deallocate;

  return (PeripheralInterface) PIImpl;
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
static void set_ddr(PeripheralInterfaceImpl *self) {
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
static void set_spcr(PeripheralInterfaceImpl *self) {
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
 * Enable SPI specific interrupts
 * @param self - The PeripheralInterface
 */
static void enableInterrupts(PeripheralInterfaceImpl *self) {
  //TODO global Interrupts?
  set_bit(self->spcr, spi_interrupt_enable);
}

/**
 * Disable SPI specific interrupts
 * @param self - The PeripheralInterface
 */
static void disableInterrupts(PeripheralInterfaceImpl *self) {
  clear_bit(self->spcr, spi_interrupt_enable);
}

/**
 * Initialize all necessary things to enable SPI
 * @param self - The PeripheralInterface
 */
void init(PeripheralInterface self) {
  PeripheralInterfaceImpl *peripheralImpl = (PeripheralInterfaceImpl *) self;
  set_ddr(peripheralImpl);
  set_spcr(peripheralImpl);
}

/**
 * Write a byte to the SPI Data Register
 * @param self - The PeripheralInterface Implementation
 * @param byte - The value to transmit
 */
static void write(PeripheralInterfaceImpl *self, uint8_t byte) {
  *(self->spdr) = byte;
}

/**
 * Read a byte from the SPI Data Register
 * @param self - The PeripheralInterface Implementation
 * @return - The content of the SPI Data Register
 */
static uint8_t read(PeripheralInterfaceImpl *self) {
  return *(self->spdr);
}

/**
 * Returns, whether the PeripheralInterface is currently busy with communication
 * When the transmission is finished, the spi_interrupt_flag in the SPSR is set.
 * Therefore we check whether this flag is not set, which indicates that it is in fact busy.
 *
 * Working:
 * SPSR = 0b0xxxxxxx
 *
 * Done:
 * SPSR = 0b1xxxxxxx
 *
 * @param self - The PeripheralInterface
 * @return - true if currently busy
 */
static bool spiBusy(PeripheralInterfaceImpl *self) {
  return !(*(self->spsr) & (1 << spi_interrupt_flag));
}


/**
 * Check if a callback is set and call it, check if clear flag is set and clear if so
 * @param impl - The PeripheralInterface
 */
static void readCallback(PeripheralInterfaceImpl *impl) {
  if (impl->readCallback.function != NULL) {
    impl->readCallback.function(impl->readCallback.argument);
  }
  if (impl->clearReadCallback) {
    impl->readCallback.function = NULL;
    impl->readCallback.argument = NULL;
  }
}

static void writeCallback(PeripheralInterfaceImpl *impl) {
  if (impl->writeCallback.function != NULL) {
    impl->writeCallback.function(impl->writeCallback.argument);
  }
  if (impl->clearWriteCallback) {
    impl->writeCallback.function = NULL;
    impl->writeCallback.argument = NULL;
  }
}

/**
 * One method, which may be called when calling handleInterrupt() inside the ISR
 * This method is called when a SPI transmission is finished, therefore a previous byte is sent.
 * If a callback is included, it will be called after the last byte.
 * The callback won't be cleared
 * The Interrupts are disabled with sending the last byte
 *
 * First Byte is written in writeNonBlocking
 *
 * Last Byte is written in writeInInterrupt and completion triggers WriteCallback
 *
 */
static void writeInInterrupt() {
  if (interfacePTR->interruptData.index < interfacePTR->interruptData.length) {
    write(interfacePTR, interfacePTR->interruptData.buffer[interfacePTR->interruptData.index]);
    interfacePTR->interruptData.index++;
  } else {
    writeCallback(interfacePTR);
    interfacePTR->interruptData.busy = false;
    disableInterrupts(interfacePTR);
  }
}

/**
 * The other method, which may be called when calling handleInterrupt() inside the ISR
 * This method is called, when a SPI transmission is completed, i.e. a byte was received.
 * If a callback is included, it will be called with the last read byte
 */
static void readInInterrupt() {
  interfacePTR->interruptData.buffer[interfacePTR->interruptData.index] = read(interfacePTR);
  (interfacePTR->interruptData.index)++;
  if (interfacePTR->interruptData.index >= interfacePTR->interruptData.length) {
    readCallback(interfacePTR);
    interfacePTR->interruptData.busy = false;
    disableInterrupts(interfacePTR);
  }
    //TODO activate this on real hardware for SPI to start the next transmission
  else {
    write(interfacePTR, 0x00);
  }
}

/**
 * Sets up the InterruptData and the Interrupthandler for both Reading and Writing
 * @param impl - The PeripheralInterfaceImplementation
 * @param buffer - The Buffer to transmit/write in
 * @param length - Max Length of the buffer
 * @param interruptHandler - The method to call when handling an interrupt
 */
static void
setupNonBlocking(PeripheralInterfaceImpl *impl, uint8_t *buffer, uint16_t length, void (*interruptHandler)(void)) {
  impl->interruptData.busy = true;
  impl->handleInterrupt = interruptHandler;
  impl->interruptData.length = length;
  impl->interruptData.buffer = buffer;
  impl->interruptData.index = 0;
  enableInterrupts(impl);
  interfacePTR = impl;
}

//TODO test this on real hardware
/**
 * Write a bytebuffer with a specified length over SPI in a non blocking way
 * @param self - The PeripheralInterface
 * @param buffer - a buffer which is to be transmitted
 * @param length - The maximum transmission length
 * @warning - Length shouldn't be bigger than the buffer
 */
void writeNonBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length) {

  PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *) self;
  if (impl->interruptData.busy == false) {
    setupNonBlocking(impl, buffer, length, writeInInterrupt);
    interfacePTR->handleInterrupt(); //Write First Byte
  } else {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
}

//TODO test this on real hardware
/**
 * Read "length" bytes into a buffer in a non blocking way
 * @param self - The PeripheralInterface
 * @param buffer - The destination buffer
 * @param length - Maximum length to read
 * @warning - Length shouldn't be bigger than the buffer
 */
void readNonBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length) {
  PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *) self;
  if (impl->interruptData.busy == false) {
    setupNonBlocking(impl, buffer, length, readInInterrupt);
    write(impl, 0x00); // Start transmission
  } else {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
}


/**
 * Write a bytebuffer with a specified length over SPI and block until it is finished
 * @param self - The PeripheralInterface
 * @param buffer - The buffer
 * @param length - Length of the buffer
 * @warning - Length shouldn't be bigger than the buffer
 */
//TODO test this on real hardware
void writeBlocking(PeripheralInterface self, const uint8_t *buffer, uint16_t length) {
  PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
  for (uint16_t i = 0; i < length; ++i) {
    write(peripheralSPI, buffer[i]);
    while (spiBusy(peripheralSPI)) {}
  }
}

/**
 * Read "length" bytes into a buffer and wait until it is finished
 * @param self - The PeripheralInterface
 * @param buffer - The destination buffer
 * @param length - The amount to read
 * @warning - Length shouldn't be bigger than the buffer
 */
//TODO test this on real hardware
void readBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length) {
  PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
  for (uint16_t i = 0; i < length; ++i) {
    write(peripheralSPI, 0x00);
    while (spiBusy(peripheralSPI)) {}
    buffer[i] = read(peripheralSPI);
  }
  readCallback(peripheralSPI);
}


/**
 * Free allocated memory
 * @param self - The PeripheralInterface
 */
void destroy(PeripheralInterface self) {
  deallocator(self);
}

/**
 * Set the function to be called, when any read is finished
 * @param self - The PeripheralInterface
 * @param callback - The function to be called
 * @param callback_parameter - The parameters to pass the function
 */
void setReadCallback(PeripheralInterface self, PeripheralCallback callback) {
  PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
  peripheralSPI->readCallback = callback;
}

/**
 * Set the function to be called, when any write is finished
 * @param self - The PeripheralInterface
 * @param callback - The function to be called
 * @param callback_parameter - The parameters to pass the function
 */
void setWriteCallback(PeripheralInterface self, PeripheralCallback callback) {
  PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
  peripheralSPI->writeCallback = callback;
}

/**
 * Sets whether after calling one of the callbacks, the callback should be cleared and not called at the next write/read
 * @param self - The PeripheralInterface
 * @param clearReadCallbackOnCall - Clear ReadCallback after calling it?
 * @param clearWriteCallbackOnCall - Clear WriteCallback after calling it?
 */
void setCallbackClearFlags(PeripheralInterface self, bool clearReadCallbackOnCall, bool clearWriteCallbackOnCall) {
  PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
  peripheralSPI->clearReadCallback = clearReadCallbackOnCall;
  peripheralSPI->clearWriteCallback = clearWriteCallbackOnCall;
}

/**
 * Configure a Peripheral to be a slave
 * @param self - The PeripheralInterface
 * @param device - The device to become a slave
 */
void configurePeripheral(PeripheralInterface self, Peripheral *device) {
  SPIPeripheral *peripheral = (SPIPeripheral *) device;
  PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
  peripheralSPI->device = device;
  set_bit(peripheral->DDR, peripheral->PIN);
  set_bit(peripheral->PORT, peripheral->PIN);
}

/**
 * Pull down the SS for the specified device to indicate the start of a transmission
 * @param self - The PeripheralInterface
 * @param device - The Device
 */
void selectPeripheral(PeripheralInterface self, Peripheral *device) {
  SPIPeripheral *peripheral = (SPIPeripheral *) device;
  clear_bit(peripheral->PORT, peripheral->PIN);
}

/**
 * Pull up the SS for the specified device to indicate the end of a transmission
 * @param self - The PeripheralInterface
 * @param device - The Device
 */
void deselectPeripheral(PeripheralInterface self, Peripheral *device) {
  SPIPeripheral *peripheral = (SPIPeripheral *) device;
  set_bit(peripheral->PORT, peripheral->PIN);
}

/**
 * Returns whether the device is currently busy with transmitting a message
 * @param self - The PeripheralInterface
 * @return true if the device is busy
 */
bool isBusy(PeripheralInterface self) {
  PeripheralInterfaceImpl *peripheralSPI = (PeripheralInterfaceImpl *) self;
  return peripheralSPI->interruptData.busy;
}