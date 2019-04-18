#ifndef COMMUNICATIONMODULE_HELPERS_H
#define COMMUNICATIONMODULE_HELPERS_H

#include "Descriptors.h"
#include "LUFAConfig.h"
#include "LUFA/Drivers/Peripheral/Serial.h"
#include <avr/power.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

// also enables global interrupts
void setUpUsbSerial(void);

void usbWriteString(const uint8_t *data);

void debug(const uint8_t *data);
void debugSized(const uint8_t *data, uint16_t size);

// needs to be called periodically in short intervals from the main method
void periodicUsbTask(void);

#endif //COMMUNICATIONMODULE_HELPERS_H
