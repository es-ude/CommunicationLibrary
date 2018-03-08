//
// Created by Peter Zdankin on 02.03.18.
//

#include "unity.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/include/SPI_Layer/SPIImpl.h"
#include "lib/include/platform/io.h"
#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/communicationLayer/CommunicationLayerImpl.h"

static uint8_t memory[40];
static MockAllocateConfig alloc_config = {.size_allocated = 0, .returned_address = &memory};
static CommunicationLayer *cl;
static SPI *spi;

void configureMock(void){
    MockAllocate_configure(&alloc_config);
}

volatile uint8_t mySPCR = 0;
volatile uint8_t mySPDR = 0;
volatile uint8_t myDDRB = 0;
volatile uint8_t myPORTB = 0;

static sck_rate f_osc = f_osc_16;


#define SPCR (*(volatile uint8_t *)&mySPCR)

#define SPDR (*(volatile uint8_t *)&mySPDR)

#define DDRB (*(volatile uint8_t *)&myDDRB)

#define PORTB (*(volatile uint8_t *)&myPORTB)

void setUp(void){
    configureMock();
    SPIConfig config = {&DDRB, &PORTB, &SPCR, &SPDR, f_osc, MockAllocate_allocate};
    spi = SPI_createSPI(config);
    cl = CL_createCommunicationLayer(MockAllocate_allocate);
}

void test_SPIAndCLNotNull(void){
    TEST_ASSERT_NOT_NULL(spi);
    TEST_ASSERT_NOT_NULL(cl);
}

void test_InitNotNull(void){
    //TEST_ASSERT_NULL(cl->init);
}