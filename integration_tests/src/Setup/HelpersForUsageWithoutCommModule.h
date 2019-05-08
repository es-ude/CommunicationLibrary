#ifndef COMMUNICATIONMODULE_HELPERSFORUSAGEWITHOUTCOMMMODULE_H
#define COMMUNICATIONMODULE_HELPERSFORUSAGEWITHOUTCOMMMODULE_H

#include <stdint.h>

uint8_t
readByteFromShortAddress(uint8_t address);

void
writeByteToShortAddress(uint8_t address,
                        uint8_t byte);

void
readByteWiseFromShortAddress(uint8_t address,
                             uint8_t *data,
                             uint8_t length);

void
writeByteWiseToShortAddress(uint8_t address,
                            uint8_t *data,
                            uint8_t length);

uint8_t
readByteFromLongAddress(uint16_t address);

void
readBurstFromLongAddress(uint16_t address,
                         uint8_t *data,
                         uint8_t length);

void
writeByteToLongAddress(uint16_t address,
                       uint8_t value);

void
writeBurstToLongAddress(uint16_t address,
                        const uint8_t *data,
                        uint8_t value);

void
setShortRegister(uint8_t address,
                 uint8_t value);

void
setLongRegister(uint16_t address,
                uint8_t value);

void
setRegister(uint16_t address,
            uint8_t value);

void
enablePromiscuous(void);

uint8_t
getReceivedMessageSize(void);

void
initMrf(void);

bool
gotNewMessage(void);

void
debugPrintHex(uint8_t byte);

#endif //COMMUNICATIONMODULE_HELPERSFORUSAGEWITHOUTCOMMMODULE_H
