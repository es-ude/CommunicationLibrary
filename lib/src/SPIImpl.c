//
// Created by Peter Zdankin on 28.02.18.
//

#include <stdint.h>
#include "lib/include/SPI.h"
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct SPIImpl {
    SPI interface;
    volatile uint8_t *ddr;
} SPIImpl;

SPI * SPI_createSPI(uint8_t *ddr, Allocator allocate) {
    SPIImpl *implementation = allocate(sizeof(SPIImpl));
    implementation->ddr = ddr;
    return (SPI*) implementation;
}