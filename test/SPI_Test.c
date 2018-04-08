//
// Created by Peter Zdankin on 28.02.18.
//

#include "unity.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/include/Peripheral/PeripheralInterfaceImpl.h"
#include "lib/include/Peripheral/SPIImpl.h"
#include "lib/include/platform/io.h"
#include "lib/include/communicationLayer/Message.h"
#include <stdlib.h>

static PeripheralInterface *spi;
static MemoryManagement *dynamic_memory;

volatile uint8_t mySPCR = 0;
volatile uint8_t mySPDR = 0;
volatile uint8_t mySPSR = 0;
volatile uint8_t myDDR = 0;
volatile uint8_t myPORTB = 0;

static uint8_t f_osc = f_osc_16;

typedef struct SPIImpl {
    PeripheralInterface interface;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    uint8_t f_osc;
    void (*freeFunction)(void *);
    void (*readCallback)(void);
    void (*writeCallback)(void);
} SPIImpl;

struct InterruptData{
    Message *m;
    PeripheralInterface *peripheral;
    bool busy;
};


#define SPCR (*(volatile uint8_t *)&mySPCR)

#define SPDR (*(volatile uint8_t *)&mySPDR)

#define SPSR (*(volatile uint8_t *)&mySPSR)

#define DDRB (*(volatile uint8_t *)&myDDR)

#define PORTB (*(volatile uint8_t *)&myPORTB)


void setupDummyRegisters(void){
    SPCR = 0;
    SPDR = 0;
    DDRB = 0;
    PORTB = 0;
}

void setUp(void){
    setupDummyRegisters();
    dynamic_memory = MemoryManagement_createMockImpl();
    InterruptData *interruptData = dynamic_memory->allocate(sizeof(InterruptData));
    SPIConfig config = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc, dynamic_memory->allocate, dynamic_memory->deallocate, interruptData};
    spi = SPI_createSPI(config);
}

void tearDown(void){
    spi->handleInterrupt = NULL;
    spi->destroy(spi);
    TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_numberOfAllocatedObjects(dynamic_memory));
}

//INITIALISATION

void test_createSPIConfigReturnsCorrectPointer(void){
    TEST_ASSERT_NOT_NULL(spi);
}

void test_ddr(void){
    TEST_ASSERT_EQUAL(0, DDRB);
}

void test_spcr(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
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
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    TEST_ASSERT_BIT_HIGH(spi_mosi_pin, *(spiImpl->ddr));
    TEST_ASSERT_BIT_HIGH(spi_sck_pin, *(spiImpl->ddr));
}

void test_spiSPCR(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    TEST_ASSERT_BIT_HIGH(spi_enable, *(spiImpl->spcr));
    TEST_ASSERT_BIT_HIGH(spi_master_slave_select, *(spiImpl->spcr));
    TEST_ASSERT_BIT_HIGH(spi_ss_pin, *(spiImpl->ddr));
    uint8_t tempSPCR = *(spiImpl->spcr);
    tempSPCR &= 0b00000011;
    TEST_ASSERT_EQUAL(f_osc, tempSPCR);
}

//METHODS

void test_spiInitNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->init);
}

void test_spiWriteNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->write);
}

void test_spiReadNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->read);
}

void test_spiSlaveSelectNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->selectSlave);
}

void test_spiSlaveDeselectNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->deselectSlave);
}

void test_spiConfigureSlaveNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->configureAsSlave);
}

void test_spiHandleInterruptNull(void){
    //This has to be null, because the upper layer will set this
    TEST_ASSERT_NULL(spi->handleInterrupt);
}

void test_spiEnableInterruptNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->enableInterrupt);
}

void test_spiDisableInterruptNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->disableInterrupt);
}

void test_spiDestroyNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->destroy);
}

void test_spiTransferNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->transfer);
}

void test_setReadCallbackNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->setReadCallback);
}

void test_setWriteCallbackNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->setWriteCallback);
}

void test_writeBlockingNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->writeBlocking);
}

void test_readBlockingNotNull(void){
    TEST_ASSERT_NOT_NULL(spi->readBlocking);
}




//CODE

void test_spiWriteChangesSPDR(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spiImpl->interface.write(spi, 42);
    TEST_ASSERT_EQUAL(42, *(spiImpl->spdr));
}

void test_enableInterruptEnablesInterruptBit(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spiImpl->interface.enableInterrupt(spi);
    TEST_ASSERT_BIT_HIGH(spi_interrupt_enable, *(spiImpl->spcr));
}

void test_disableInterruptDisablesInterruptBit(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spiImpl->interface.enableInterrupt(spi);
    TEST_ASSERT_BIT_HIGH(spi_interrupt_enable, *(spiImpl->spcr));
    spiImpl->interface.disableInterrupt(spi);
    TEST_ASSERT_BIT_LOW(spi_interrupt_enable, *(spiImpl->spcr));
}


void test_spiReadBeforeStartIsZero(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    TEST_ASSERT_EQUAL(0, spiImpl->interface.read(spi));
}

void test_spiWriteThenReadIsTheSame(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spiImpl->interface.write(spi,42);
    TEST_ASSERT_EQUAL(42, spiImpl->interface.read(spi));
}

void test_macrosAreNotNull(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    TEST_ASSERT_NOT_NULL(spiImpl->spsr);
}

void dummy(void){

}

void test_readCallback(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spi->setReadCallback(spi,dummy);
    TEST_ASSERT_EQUAL_PTR(dummy, spiImpl->readCallback);
}

void test_writeCallback(void){
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spi->setWriteCallback(spi,dummy);
    TEST_ASSERT_EQUAL_PTR(dummy, spiImpl->writeCallback);
}



void test_spiConfigureSetsDDRAndPORT(void){
    uint8_t spi_slave = 1;
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spiImpl->interface.configureAsSlave(&DDRB, spi_slave, &PORTB);
    TEST_ASSERT_BIT_HIGH(spi_slave, *(spiImpl->ddr));
    TEST_ASSERT_BIT_HIGH(spi_slave, *(spiImpl->port));
}

void test_spiSelectSlave(void){
    uint8_t spi_slave = 1;
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spiImpl->interface.configureAsSlave(&DDRB, spi_slave, &PORTB);
    spiImpl->interface.selectSlave(&PORTB, spi_slave);
    TEST_ASSERT_BIT_LOW(spi_slave, *(spiImpl->port));
}

void test_spiDeselectSlave(void){
    uint8_t spi_slave = 1;
    SPIImpl *spiImpl = (SPIImpl *)spi;
    spiImpl->interface.init(spi);
    spiImpl->interface.configureAsSlave(&DDRB, spi_slave, &PORTB);
    spiImpl->interface.selectSlave(&PORTB, spi_slave);
    spiImpl->interface.deselectSlave(&PORTB, spi_slave);
    TEST_ASSERT_BIT_HIGH(spi_slave, *(spiImpl->port));
}
