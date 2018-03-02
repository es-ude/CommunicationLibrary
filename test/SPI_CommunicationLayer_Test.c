//
// Created by Peter Zdankin on 02.03.18.
//

#include "unity.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/communicationLayer/SPICommunicationLayerImpl.h.h"
#include "lib/include/SPI_Layer/SPIImpl.h"
#include "lib/include/platform/io.h"

static uint8_t memory[40];
static MockAllocateConfig alloc_config = {.size_allocated = 0, .returned_address = &memory};
static CommunicationLayer *communicationLayer;
static SPI *spi;

void configureMock(void){
    MockAllocate_configure(&alloc_config);
}

volatile uint8_t mySPCR = 0;
volatile uint8_t mySPDR = 0;
volatile uint8_t myDDR = 0;

static uint8_t f_osc = f_osc_16;


#define SPCR (*(volatile uint8_t *)&mySPCR)

#define SPDR (*(volatile uint8_t *)&mySPDR)

#define DDRB (*(volatile uint8_t *)&myDDR)

void setUp(void){
    configureMock();
    spi = SPI_createSPI(&DDRB, &SPCR, &SPDR, f_osc ,MockAllocate_allocate);
    communicationLayer = SPI_createCommunicationLayer(spi, MockAllocate_allocate);
}

void test_createCommunicationLayerReturnsCorrectPointer(void){
    TEST_ASSERT_EQUAL_PTR(alloc_config.returned_address, communicationLayer);
}