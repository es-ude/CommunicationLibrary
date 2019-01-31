#ifndef COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
#define COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H

#include <stdbool.h>
#include "src/Peripheral/PeripheralIntern.h"
#include "Peripheral/PeripheralSPIImpl.h"
#include "CException.h"
#include "Peripheral/Exception.h"
#include "src/Peripheral/SpiPinNumbers.h"

extern void debug(const uint8_t *string);
extern void debugPrintHex(uint8_t byte);

static void writeNonBlocking(PeripheralInterface *self, PeripheralInterface_NonBlockingWriteContext context);
static void setWriteCallback(PeripheralInterface *self, PeripheralInterface_Callback callback);
static void resetWriteCallback(PeripheralInterface *self);
static void handleWriteInterrupt(PeripheralInterface *self);

static void readNonBlocking(PeripheralInterface *self, uint8_t *buffer, uint16_t length);
static void setReadCallback(PeripheralInterface *self, PeripheralInterface_Callback callback);
static void resetReadCallback(PeripheralInterface *self);
static void handleReadInterrupt(PeripheralInterface *self);

static void setClockRateDividerBitValues(volatile uint8_t *control_register, uint8_t value);

static void configurePeripheral (Peripheral *device);
static void setClockRateDivider(PeripheralInterfaceSPIImpl *impl, uint8_t rate);
static void setSPIMode(volatile uint8_t *control_register, uint8_t mode);
static void setDataOrder(volatile uint8_t *control_register, uint8_t data_order);
static void enableDoubleSpeed(volatile uint8_t *status_register);
static void disableDoubleSpeed(volatile uint8_t *status_register);

static void becomeSPIMaster(PeripheralInterfaceSPIImpl *impl);
static void tearDownMaster(PeripheralInterfaceSPIImpl *impl);

static void selectPeripheral(PeripheralInterface *self, Peripheral *device);

static void deselectPeripheral(PeripheralInterface *self, Peripheral *device);

// returns true on success, false otherwise
static bool tryToClaimInterfaceWithPeripheral(PeripheralInterfaceSPIImpl *, SPISlave *);

static void releaseInterface(PeripheralInterfaceSPIImpl *impl);

static void setInterfaceFunctionPointers(PeripheralInterface *self);

static void waitUntilByteTransmitted(volatile uint8_t *status_register);

static void activateSlaveSelectLine(SPISlave *spi_chip);

static void deactivateSlaveSelectLine(SPISlave *spi_chip);

static void setUpIOLines(const SPIConfig *config);

static void setUpControlRegister(volatile uint8_t *control_register);
static uint8_t transfer(PeripheralInterfaceSPIImpl *self, uint8_t byte);
static void writeByteNonBlocking(PeripheralInterfaceSPIImpl *self, uint8_t byte);
static void writeByteBlocking(PeripheralInterface *impl, uint8_t byte);
static uint8_t readByteBlocking(PeripheralInterface *impl);

#endif //COMMUNICATIONMODULE_PERIPHERALSPIIMPLINTERN_H
