#ifndef COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
#define COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H

#include <stdbool.h>
#include "src/PeripheralIntern.h"
#include "CommunicationModule/PeripheralSPIImpl.h"
#include "CException.h"
#include "CommunicationModule/Exception.h"
#include "src/Peripheral/SpiPinNumbers.h"

/**
 * \brief
 * Assumptions:
 * - MOSI, MISO, SCK, SS(slave select input) lines all reside on the same port.
 * - we have three registers controlling the hardware spi implementation as described
 *   in the datasheet of e.g. the atmega328p
 *   - these registers behave the same across all our platforms
 */

typedef struct InterruptData InterruptData;

struct InterruptData {
  const uint8_t *output_buffer;
  uint8_t *input_buffer;
  size_t output_buffer_length;
  size_t input_buffer_length;
  PeripheralInterface_Callback write_callback;
  PeripheralInterface_Callback read_callback;
};

struct PeripheralInterfaceImpl {
  struct PeripheralInterface interface;
  SPIConfig config;
  SPISlave *current_peripheral;
  InterruptData interrupt_data;
};

extern void debug(const uint8_t *string);
extern void debugPrintHex(uint8_t byte);

typedef struct PeripheralInterfaceImpl *PeripheralInterfaceImpl;

static void writeBlocking(PeripheralInterface self, const uint8_t *buffer, uint16_t length);
static void writeNonBlocking(PeripheralInterface self, PeripheralInterface_NonBlockingWriteContext context);
static void setWriteCallback(PeripheralInterface self, PeripheralInterface_Callback callback);
static void resetWriteCallback(PeripheralInterface self);
static void handleWriteInterrupt(PeripheralInterface self);

static void readBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length);
static void readNonBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length);
static void setReadCallback(PeripheralInterface self, PeripheralInterface_Callback callback);
static void resetReadCallback(PeripheralInterface self);
static void handleReadInterrupt(PeripheralInterface self);

static void setClockRateDividerBitValues(volatile uint8_t *control_register, uint8_t value);

static void configurePeripheral (Peripheral *device);
static void setClockRateDivider(PeripheralInterfaceImpl impl, uint8_t rate);
static void setSPIMode(volatile uint8_t *control_register, uint8_t mode);
static void setDataOrder(volatile uint8_t *control_register, uint8_t data_order);
static void enableDoubleSpeed(volatile uint8_t *status_register);
static void disableDoubleSpeed(volatile uint8_t *status_register);

static void setupMaster(PeripheralInterfaceImpl impl);
static void tearDownMaster(PeripheralInterfaceImpl impl);

static void selectPeripheral(PeripheralInterface self, Peripheral *device);

static void deselectPeripheral(PeripheralInterface self, Peripheral *device);

// returns true on success, false otherwise
static bool tryToClaimInterfaceWithPeripheral(PeripheralInterfaceImpl, SPISlave *);

static void releaseInterface(PeripheralInterfaceImpl impl);

static void setInterfaceFunctionPointers(PeripheralInterface self);

static void waitUntilByteTransmitted(volatile uint8_t *status_register);

static void activateSlaveSelectLine(SPISlave *spi_chip);

static void deactivateSlaveSelectLine(SPISlave *spi_chip);

static void setUpIOLines(const SPIConfig *config);

static void setUpControlRegister(volatile uint8_t *control_register);
static uint8_t transfer(PeripheralInterfaceImpl self, uint8_t byte);
static void writeByteNonBlocking(PeripheralInterfaceImpl self, uint8_t byte);
#endif //COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
