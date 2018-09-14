#ifndef COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
#define COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H

#include <stdbool.h>
#include "lib/src/PeripheralIntern.h"
#include "lib/include/Peripheral/PeripheralSPIImpl.h"
#include "CException.h"
#include "lib/include/Exception.h"
#include "lib/src/Peripheral/SpiPinNumbers.h"

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

struct PeripheralInterfaceImpl {
  struct PeripheralInterface interface;
  const SPIConfig *config;
  PeripheralSPI *current_peripheral;
};

extern void debug(const uint8_t *string);
extern void debugPrintHex(uint8_t byte);

typedef struct PeripheralInterfaceImpl *PeripheralInterfaceImpl;

static void writeBlocking(PeripheralInterface self, const uint8_t *buffer, uint16_t length);

static void readBlocking(PeripheralInterface self, uint8_t *buffer, uint16_t length);

static void setClockRateDividerBitValues(volatile uint8_t *control_register, uint8_t value);

static void configurePeripheralNew(Peripheral *device);
static void setClockRateDivider(PeripheralInterfaceImpl impl, uint8_t rate);

static void setDataOrder(volatile uint8_t *control_register, uint8_t data_order);
static void enableDoubleSpeed(volatile uint8_t *status_register);
static void disableDoubleSpeed(volatile uint8_t *status_register);
static void setRegisterWithBitMask(volatile uint8_t *register_ptr, uint8_t bit_mask, uint8_t value);

static void selectPeripheral(PeripheralInterface self, Peripheral *device);

static void deselectPeripheral(PeripheralInterface self, Peripheral *device);

// returns true on success, false otherwise
static bool tryToClaimInterfaceWithPeripheral(PeripheralInterfaceImpl, PeripheralSPI *);

static void set_bit(volatile uint8_t *field, uint8_t bit_number);
static void clear_bit(volatile uint8_t *field, uint8_t bit_number);


static void init(PeripheralInterface self);

static void releaseInterface(PeripheralInterfaceImpl impl);

void setInterfaceFunctionPointers(PeripheralInterface self);

void waitUntilByteTransmitted(volatile uint8_t *status_register);

static void activateSlaveSelectLine(PeripheralSPI *spi_chip);

static void deactivateSlaveSelectLine(PeripheralSPI *spi_chip);

static void setUpIOLines(const SPIConfig *config);

static void setUpControlRegister(volatile uint8_t *control_register);

#endif //COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
