#ifndef COMMUNICATIONMODULE_PERIPHERALAPI_H
#define COMMUNICATIONMODULE_PERIPHERALAPI_H

#include "lib/include/Peripheral.h"

/**
 * https://gcc.gnu.org/onlinedocs/gcc/Inline.html
This combination of inline and extern has almost the effect of a macro.
 The way to use it is to put a function definition in a header file with these keywords,
 and put another copy of the definition (lacking inline and extern) in a library file.
 The definition in the header file causes most calls to the function to be inlined.
 If any uses of the function remain, they refer to the single copy in the library.
 */


inline void Peripheral_writeByte(Peripheral *self, uint8_t byte){
    self->writeByte(self, byte);
}

inline uint8_t Peripheral_readByte(Peripheral *self) {
    return self->readByte(self);
}

inline void Peripheral_setInterruptHandler(Peripheral *self, InterruptHandler handler) {
    self->setInterruptHandler(self, handler);
}

inline void Peripheral_handleInterrupt(Peripheral *self) {
    self->handleInterrupt(self);
}


#endif //COMMUNICATIONMODULE_PERIPHERALAPI_H
