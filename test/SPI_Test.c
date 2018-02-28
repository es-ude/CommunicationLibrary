//
// Created by Peter Zdankin on 28.02.18.
//

#include "unity.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/include/PeripheralSPIImpl.h"
#include "lib/include/SPI.h"

static uint8_t memory[20];
static MockAllocateConfig alloc_config = {.size_allocated = 0, .returned_address = &memory};
static volatile uint8_t port;
static SPI *spi;

volatile uint8_t mySPCR = 0;
volatile uint8_t mySPDR = 0;


#define SPCR (*(volatile uint8_t *)&mySPCR)

#define SPDR (*(volatile uint8_t *)&mySPDR)


void configureMock(void){
    configureMockAllocate(&alloc_config);
}

void setupDummyRegisters(void){
    SPCR = 0;
    SPDR = 0;
}

void setUp(void){
    setupDummyRegisters();
    configureMock();
    spi = SPI_createSPI(mockAllocate);
}

void test_createSPIConfigReturnsCorrectPointer(void){
    TEST_ASSERT_EQUAL_PTR(alloc_config.returned_address, spi);
}
