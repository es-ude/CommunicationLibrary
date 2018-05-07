#ifndef UNITYOUTPUTIMPLEMENTATIONS_IO_H
#define UNITYOUTPUTIMPLEMENTATIONS_IO_H

#if defined (__AVR_ATmega328P__)
#include "lib/include/platform/atmega328p/io.h"
#else
#include "lib/include/platform/dummy/io.h"
#endif
#endif //UNITYOUTPUTIMPLEMENTATIONS_IO_H