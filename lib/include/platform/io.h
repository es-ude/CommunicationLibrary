#ifndef UNITYOUTPUTIMPLEMENTATIONS_IO_H
#define UNITYOUTPUTIMPLEMENTATIONS_IO_H

#include <stdint.h>

#if defined (__AVR_ATmega328P__)
#include "lib/include/platform/atmega328p/io.h"
#elseif defined (__AVR_ATmega32U4__)
#include "lib/include/platform/atmega32u4/io.h"
#else
#include "lib/include/platform/dummy/io.h"
#endif
#endif //UNITYOUTPUTIMPLEMENTATIONS_IO_H