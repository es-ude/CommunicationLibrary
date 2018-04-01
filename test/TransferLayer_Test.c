//
// Created by Peter Zdankin on 01.04.18.
//

#include "lib/include/TransferLayer/PeripheralInterface.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "unity.h"
#include "lib/include/platform/io.h"

static PeripheralInterface *interface;
static MemoryManagement *dynamic_memory;

typedef struct SPIImpl {
    PeripheralInterface interface;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    uint8_t f_osc;
} SPIImpl;

typedef struct PeripheralSPIImpl{
    PeripheralInterface interface;
    SPIImpl *spi;
} PeripheralSPIImpl;

volatile uint8_t mySPCR = 0;
volatile uint8_t mySPDR = 0;
volatile uint8_t mySPSR = 0;
volatile uint8_t myDDR = 0;
volatile uint8_t myPORTB = 0;

static uint8_t f_osc = f_osc_16;

#define SPCR (*(volatile uint8_t *)&mySPCR)

#define SPDR (*(volatile uint8_t *)&mySPDR)

#define SPSR (*(volatile uint8_t *)&mySPSR)

#define DDRB (*(volatile uint8_t *)&myDDR)

#define PORTB (*(volatile uint8_t *)&myPORTB)

void setupDummyRegisters(void){
    SPCR = 0;
    SPDR = 0;
    SPSR = 0;
    DDRB = 0;
    PORTB = 0;
}


void setUp(){
    setupDummyRegisters();
    dynamic_memory = MemoryManagement_createMockImpl();
    SPIConfig spiConfig = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc, dynamic_memory->allocate, dynamic_memory->deallocate};
    TransferLayerConfig transferConfig = {dynamic_memory->allocate, dynamic_memory->deallocate};
    interface = PeripheralInterface_create(transferConfig, spiConfig);
}

void tearDown(){
    interface->destroy(interface);
    TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_numberOfAllocatedObjects(dynamic_memory));
}

//METHODS

void test_interfaceNotNull(void){
    TEST_ASSERT_NOT_NULL(interface);
}

void test_initNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->init);
}

void test_destroyNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->destroy);
}

//SPI

void test_spiInitNotNull(void){
    SPIImpl *spiImpl = ((PeripheralSPIImpl *)interface)->spi;
    TEST_ASSERT_NOT_NULL(spiImpl->interface.init);
}

void test_ddr(void){
    TEST_ASSERT_EQUAL(0, DDRB);
}

void test_spcr(void){
    SPIImpl *spiImpl = ((PeripheralSPIImpl *)interface)->spi;
    TEST_ASSERT_EQUAL_INT(0, *(spiImpl->spcr));
}



void test_f_osc_4(void){
    TEST_ASSERT_BIT_LOW(0, f_osc_4);
    TEST_ASSERT_BIT_LOW(1, f_osc_4);
}

void test_f_osc_16(void){
    TEST_ASSERT_BIT_HIGH(0, f_osc_16);
    TEST_ASSERT_BIT_LOW (1, f_osc_16);
}

void test_f_osc_64(void){
    TEST_ASSERT_BIT_LOW (0, f_osc_64);
    TEST_ASSERT_BIT_HIGH(1, f_osc_64);
}

void test_f_osc_128(void){
    TEST_ASSERT_BIT_HIGH(0, f_osc_128);
    TEST_ASSERT_BIT_HIGH(1, f_osc_128);
}

void test_spiDDR(void){
    SPIImpl *spiImpl = ((PeripheralSPIImpl *)interface)->spi;

    interface->init(interface);

    TEST_ASSERT_BIT_HIGH(spi_mosi_pin, *(spiImpl->ddr));
    TEST_ASSERT_BIT_HIGH(spi_sck_pin, *(spiImpl->ddr));
}

void test_spiSPCR(void){
    SPIImpl *spiImpl = ((PeripheralSPIImpl *)interface)->spi;
    interface->init(interface);
    TEST_ASSERT_BIT_HIGH(spi_enable, *(spiImpl->spcr));
    TEST_ASSERT_BIT_HIGH(spi_master_slave_select, *(spiImpl->spcr));
    TEST_ASSERT_BIT_HIGH(spi_ss_pin, *(spiImpl->ddr));
    uint8_t tempSPCR = *(spiImpl->spcr);
    tempSPCR &= 0b00000011;
    TEST_ASSERT_EQUAL(f_osc, tempSPCR);
}