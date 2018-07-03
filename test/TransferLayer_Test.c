//
// Created by Peter Zdankin on 01.04.18.
//

#include <string.h>
#include "lib/include/Peripheral.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "test/Mocks/RuntimeLibraryImplMock.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "unity.h"
#include "lib/include/TransferLayer/InterruptData.h"
#include "CException.h"
#include "lib/include/Exception.h"

static PeripheralInterface interface;
static MemoryManagement *dynamic_memory;

struct InterruptData{
    uint8_t *buffer;
    uint16_t length;
    uint16_t index;
    bool busy;
};

typedef struct PeripheralInterfaceImpl {
    struct PeripheralInterface interface;
    PeripheralCallback readCallback;
    PeripheralCallback writeCallback;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    bool clearWriteCallback;
    bool clearReadCallback;
    uint8_t f_osc;
    void (*handleInterrupt)(void);
    InterruptData interruptData;
} PeripheralInterfaceImpl;

typedef struct SPIPeripheral{
    volatile uint8_t *DDR;
    uint8_t PIN;
    volatile  uint8_t *PORT;
} SPIPeripheral;

volatile uint8_t mySPCR = 0;
volatile uint8_t mySPDR = 0;
volatile uint8_t mySPSR = 0;
volatile uint8_t myDDR = 0;
volatile uint8_t myPORTB = 0;

static uint8_t buffer[] = "Hello World";
static uint8_t *receiveBuffer;
static uint16_t length = sizeof(buffer) / sizeof(uint8_t);



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
    SPIConfig spiConfig = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc};
    TransferLayerConfig transferConfig = {dynamic_memory->allocate, dynamic_memory->deallocate};
    interface = PeripheralInterface_create(transferConfig, spiConfig);
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    impl->interruptData.busy = false;
    impl->interruptData.index = 0;
    impl->interruptData.buffer = NULL;
    impl->interruptData.length = 0;
    impl->readCallback.function = NULL;
    impl->readCallback.argument = NULL;
    impl->writeCallback.function = NULL;
    impl->writeCallback.argument = NULL;

    receiveBuffer = dynamic_memory->allocate(sizeof(uint8_t) * (11 + 1));
}

void tearDown(){
    interface->destroy(interface);
    dynamic_memory->deallocate(buffer);
    dynamic_memory->deallocate(receiveBuffer);
    TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_numberOfAllocatedObjects(dynamic_memory));
}


void readCallback(uint16_t *thingy){
    *thingy = 1337;
}

void writeCallback(uint16_t *thingy){
    *thingy = 123;
}


void stub_callback_function(void * arg){

}
void *stub_callback_argument = (void*)2;
PeripheralCallback stub_callback = {.argument = (void*) 2, .function = stub_callback_function};

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

void test_setReadCallbackNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->setReadCallback);
}
void test_setWriteCallbackNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->setWriteCallback);
}

void test_writeBlockingNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->writeBlocking);
}

void test_readBlockingNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->readBlocking);
}

void test_writeNonBlockingNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->writeNonBlocking);
}

void test_readNonBlockingNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->readNonBlocking);
}

void test_clearCallbackNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->setCallbackClearFlags);
}

void test_isBusyNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->isBusy);
}

//SPI


void test_ddr(void){
    TEST_ASSERT_EQUAL(0, DDRB);
}

void test_spcr(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    TEST_ASSERT_EQUAL_INT(0, *(impl->spcr));
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
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);

    TEST_ASSERT_BIT_HIGH(spi_mosi_pin, *(impl->ddr));
    TEST_ASSERT_BIT_HIGH(spi_sck_pin, *(impl->ddr));
}

void test_spiSPCR(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    TEST_ASSERT_BIT_HIGH(spi_enable, *(impl->spcr));
    TEST_ASSERT_BIT_HIGH(spi_master_slave_select, *(impl->spcr));
    TEST_ASSERT_BIT_HIGH(spi_ss_pin, *(impl->ddr));
    uint8_t tempSPCR = *(impl->spcr);
    tempSPCR &= 0b00000011;
    TEST_ASSERT_EQUAL(f_osc, tempSPCR);
}




void test_setReadCallbackChangesMethod(void){
    interface->setReadCallback(interface, stub_callback);
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    TEST_ASSERT_EQUAL_PTR(stub_callback_function, impl->readCallback.function);
}

void test_setReadCallbackParameter(void){
    interface->setReadCallback(interface, stub_callback);
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    TEST_ASSERT_EQUAL_PTR(stub_callback.argument, impl->readCallback.argument);
}

void test_setWriteCallbackChangesMethod(void){
    interface->setWriteCallback(interface, stub_callback);
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    TEST_ASSERT_EQUAL_PTR(stub_callback.function, impl->writeCallback.function);
}

void test_setWriteCallbackParameter(void){
    int data = 2;
    interface->setWriteCallback(interface, stub_callback);
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    TEST_ASSERT_EQUAL_PTR(stub_callback.argument, impl->writeCallback.argument);
}

void test_configurePeripheralNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->configurePeripheral);
}

void test_configurePeripheral(void){
    uint8_t spi_slave = 1;
    SPIPeripheral device = {&DDRB, spi_slave, &PORTB};
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->configurePeripheral(interface, &device);
    TEST_ASSERT_BIT_HIGH(spi_slave, DDRB);
    TEST_ASSERT_BIT_HIGH(spi_slave, PORTB);
    TEST_ASSERT_BIT_HIGH(spi_slave,*device.DDR);
    TEST_ASSERT_BIT_HIGH(spi_slave,*device.PORT);
    TEST_ASSERT_BIT_HIGH(spi_slave,*impl->ddr);
    TEST_ASSERT_BIT_HIGH(spi_slave,*impl->port);
}

void test_selectPeripheralNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->selectPeripheral);
}

void test_selectPeripheral(void){
    uint8_t spi_slave = 1;
    SPIPeripheral device = {&DDRB, spi_slave, &PORTB};
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->configurePeripheral(interface, &device);
    interface->selectPeripheral(interface, &device);
    TEST_ASSERT_BIT_LOW(spi_slave, PORTB);
    TEST_ASSERT_BIT_LOW(spi_slave,*device.PORT);
    TEST_ASSERT_BIT_LOW(spi_slave,*impl->port);
}

void test_deselectPeripheralNotNull(void){
    TEST_ASSERT_NOT_NULL(interface->deselectPeripheral);
}

void test_deselectPeripheral(void){
    uint8_t spi_slave = 1;
    SPIPeripheral device = {&DDRB, spi_slave, &PORTB};
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->configurePeripheral(interface, &device);
    interface->selectPeripheral(interface, &device);
    interface->deselectPeripheral(interface, &device);
    TEST_ASSERT_BIT_HIGH(spi_slave, PORTB);
    TEST_ASSERT_BIT_HIGH(spi_slave,*device.PORT);
    TEST_ASSERT_BIT_HIGH(spi_slave,*impl->port);
}



void test_isNotBusyAtBegin(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    TEST_ASSERT_FALSE(impl->interruptData.busy);
}

void test_isBusyAfterStartNonBlockingWrite(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    TEST_ASSERT_TRUE(impl->interruptData.busy);
}

void test_handleInterruptNotNullAfterNonBlockingWrite(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    TEST_ASSERT_NOT_NULL(impl->handleInterrupt);
}

void test_NotBusyAfterNonBlockingWrite(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    for(uint16_t i = 0; i <= length; ++i){
        impl->handleInterrupt();
    }
    TEST_ASSERT_FALSE(impl->interruptData.busy);
}


void test_interruptsEnabledAfterNonBlockingWrite(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    TEST_ASSERT_BIT_HIGH(spi_interrupt_enable, *impl->spcr);
}


void test_interruptsDisabledAfterNonBlockingWrite(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    for(uint16_t i = 0; i < length; ++i){
        impl->handleInterrupt();
    }
    TEST_ASSERT_BIT_LOW(spi_interrupt_enable, *impl->spcr);
}

void test_indexAfterNonBlockingWriteIs1(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    TEST_ASSERT_EQUAL_UINT16(1,impl->interruptData.index); //Because first byte is transmitted
}

void test_spdrContainsMessageAfterEachWrite(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    for(uint16_t i = 0; i < length; ++i){
        TEST_ASSERT_EQUAL_UINT8(buffer[i], *impl->spdr);
        impl->handleInterrupt();
    }
}

void test_setReadAndWriteCallback(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    TEST_ASSERT_FALSE(impl->clearReadCallback);
    TEST_ASSERT_FALSE(impl->clearWriteCallback);
    interface->setCallbackClearFlags(interface, true, false);
    TEST_ASSERT_TRUE(impl->clearReadCallback);
    TEST_ASSERT_FALSE(impl->clearWriteCallback);
    interface->setCallbackClearFlags(interface, false, true);
    TEST_ASSERT_FALSE(impl->clearReadCallback);
    TEST_ASSERT_TRUE(impl->clearWriteCallback);
    interface->setCallbackClearFlags(interface, true, true);
    TEST_ASSERT_TRUE(impl->clearReadCallback);
    TEST_ASSERT_TRUE(impl->clearWriteCallback);
}


void test_readCallbackClearedAfterCallback(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->setCallbackClearFlags(interface,true, false);
    uint16_t value = 0;
    uint16_t *valuePTR = &value;
    PeripheralCallback callback = {.function = (void (*)(void*)) readCallback, .argument = &value};
    interface->setReadCallback(interface, callback);
    interface->readNonBlocking(interface, buffer, length);
    for(uint16_t i = 0; i < length; ++i){
        *impl->spdr = buffer[i];
        impl->handleInterrupt();
    }
    TEST_ASSERT_EQUAL_UINT16(1337, *valuePTR);
    TEST_ASSERT_NULL(impl->readCallback.function);
    TEST_ASSERT_NULL(impl->readCallback.argument);
}

void test_writeCallback(){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    uint16_t value = 0;
    PeripheralCallback callback = {
            .function = (void (*)(void *)) writeCallback,
            .argument = &value,
    };
    interface->setWriteCallback(interface, callback);
    interface->writeNonBlocking(interface, buffer, length);
    for(uint16_t i = 0; i < length; ++i){
        impl->handleInterrupt();
    }
    TEST_ASSERT_EQUAL_UINT16(123, value);
}


void test_interruptsEnabledAfterNonBlockingRead(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->readNonBlocking(interface, receiveBuffer, length);
    TEST_ASSERT_BIT_HIGH(spi_interrupt_enable, *impl->spcr);
}
void test_isBusyAfterStartNonBlockingRead(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->readNonBlocking(interface, receiveBuffer, length);
    TEST_ASSERT_TRUE(impl->interruptData.busy);
    TEST_ASSERT_TRUE(interface->isBusy(interface));
}

void test_NotBusyAfterNonBlockingRead(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->readNonBlocking(interface, receiveBuffer, length);
    for(uint16_t i = 0; i < length; ++i){
        *impl->spdr = buffer[i];
        impl->handleInterrupt();
    }
    TEST_ASSERT_FALSE(impl->interruptData.busy);
    TEST_ASSERT_FALSE(interface->isBusy(interface));
}

void test_InterruptsDisabledAfterNonBlockingRead(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->readNonBlocking(interface, receiveBuffer, length);
    for(uint16_t i = 0; i < length; ++i){
        *impl->spdr = buffer[i];
        impl->handleInterrupt();
    }
    TEST_ASSERT_BIT_LOW(spi_interrupt_enable, *impl->spcr);
}

void test_nonBlockingReadSPDR(void){
  // This test is ignored since it is currently not clear
  // how to correctly unit test that behaviour.
  TEST_IGNORE();
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->readNonBlocking(interface, receiveBuffer, length);
    for(uint16_t i = 0; i < length; ++i){
        *impl->spdr = buffer[i];
        impl->handleInterrupt();
        TEST_ASSERT_EQUAL_UINT8(buffer[i], *impl->spdr);
    }
}

void test_nonBlockingWriteInitializesAllValues(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->writeNonBlocking(interface, buffer, length);
    TEST_ASSERT_NOT_NULL(impl->interruptData.buffer);
    TEST_ASSERT_NOT_NULL(impl->handleInterrupt);
    TEST_ASSERT_NOT_NULL(interface->isBusy);
    TEST_ASSERT_EQUAL_UINT16(1, impl->interruptData.index);
    impl->handleInterrupt();
    TEST_ASSERT_EQUAL_UINT16(2, impl->interruptData.index);

}

void test_nonBlockingReadReceiveBuffer(void){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    interface->readNonBlocking(interface, receiveBuffer, length);
    for(uint16_t i = 0; i < length; ++i){
        *impl->spdr = buffer[i];
        impl->handleInterrupt();
        TEST_ASSERT_EQUAL_UINT8(buffer[i], receiveBuffer[i]);
    }
}

void test_writeNonBlockingTwiceGivesException(void){
    interface->init(interface);
    CEXCEPTION_T ex;
    interface->writeNonBlocking(interface, buffer, length);
    Try{
                interface->writeNonBlocking(interface, buffer, length);
            }Catch(ex){

    }
    TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, ex);
}

void test_readNonBlockingTwiceGivesException(void){
    interface->init(interface);
    CEXCEPTION_T ex;
    interface->readNonBlocking(interface, buffer, length);
    Try{
                interface->readNonBlocking(interface, buffer, length);
            }Catch(ex){

    }
    TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, ex);
}

void test_readAndWriteNonBlockingGivesException(void){
    interface->init(interface);
    CEXCEPTION_T ex;
    interface->readNonBlocking(interface, buffer, length);
    Try{
                interface->writeNonBlocking(interface, buffer, length);
            }Catch(ex){

    }
    TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, ex);
}

void test_writeAndreadNonBlockingGivesException(void){
    interface->init(interface);
    CEXCEPTION_T ex;
    interface->writeNonBlocking(interface, buffer, length);
    Try{
                interface->readNonBlocking(interface, buffer, length);
            }Catch(ex){

    }
    TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, ex);
}



void test_readCallback(){
    PeripheralInterfaceImpl *impl = (PeripheralInterfaceImpl *)interface;
    interface->init(interface);
    uint16_t value = 0;
    PeripheralCallback callback = {
            .function = (void (*) (void *)) readCallback,
            .argument = &value
    };
    interface->setReadCallback(interface, callback);
    interface->readNonBlocking(interface, buffer, length);
    for(uint16_t i = 0; i < length; ++i){
        *impl->spdr = buffer[i];
        impl->handleInterrupt();
    }
    TEST_ASSERT_EQUAL_UINT16(1337, value);
}

//These tests can only be run on SPI
void test_readCallbackBlocking(){
    //interface->init(interface);
    //uint16_t value = 0;
    //uint16_t *valuePTR = &value;
    //interface->setReadCallback(interface, (void *) readCallback, valuePTR);
    //interface->readBlocking(interface, buffer, length);
    //TEST_ASSERT_EQUAL_UINT16(1337, *valuePTR);
}

void test_writeCallbackBlocking(){
    //interface->init(interface);
    //uint16_t value = 0;
    //uint16_t *valuePTR = &value;
    //interface->setWriteCallback(interface, (void *) writeCallback, valuePTR);
    //interface->writeBlocking(interface, buffer, length);
    //TEST_ASSERT_EQUAL_UINT16(123, *valuePTR);
}