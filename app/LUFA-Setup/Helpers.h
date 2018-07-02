#ifndef COMMUNICATIONMODULE_HELPERS_H
#define COMMUNICATIONMODULE_HELPERS_H

#include "app/LUFA-Setup/Descriptors.h"
#include "app/LUFA-Setup/LUFAConfig.h"
#include <avr/power.h>

void setUpUsbSerial(void);

void usbWriteByte(uint8_t byte);

void usbWriteString(const uint8_t *data);

#endif //COMMUNICATIONMODULE_HELPERS_H
