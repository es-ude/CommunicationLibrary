//
// Created by Peter Zdankin on 02.03.18.
//

#include <stdlib.h>
#include "unity.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/include/Peripheral/SPIImpl.h"
#include "lib/include/platform/io.h"
#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/communicationLayer/CommunicationLayerImpl.h"

static CommunicationLayer *cl;
static PeripheralInterface *spi;
static MemoryManagement *dynamic_memory;


typedef struct CommunicationLayerImpl{
    CommunicationLayer communicationLayer;
    PeripheralInterface *peripheralInterface;
} CommunicationLayerImpl;

typedef struct SPIImpl {
    PeripheralInterface interface;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    uint8_t f_osc;
} SPIImpl;

struct InterruptData{
    Message *m;
    PeripheralInterface *peripheral;
    bool busy;
};


volatile uint8_t mySPCR = 0;
volatile uint8_t mySPDR = 0;
volatile uint8_t mySPSR = 0;
volatile uint8_t myDDRB = 0;
volatile uint8_t myPORTB = 0;

static sck_rate f_osc = f_osc_16;


#define SPCR (*(volatile uint8_t *)&mySPCR)

#define SPDR (*(volatile uint8_t *)&mySPDR)

#define SPSR (*(volatile uint8_t *)&mySPSR)

#define DDRB (*(volatile uint8_t *)&myDDRB)

#define PORTB (*(volatile uint8_t *)&myPORTB)

static void setPorts(){
    SPCR = 0;
    SPDR = 0;
    DDRB = 0;
    PORTB = 0;
}

void setUp(void){
    setPorts();
    dynamic_memory = MemoryManagement_createMockImpl();
    InterruptData *interruptData = dynamic_memory->allocate(sizeof(InterruptData));
    SPIConfig spiConfig = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc, dynamic_memory->allocate, dynamic_memory->deallocate, interruptData};
    spi = SPI_createSPI(spiConfig);
    CommunicationLayerConfig commConfig = {spi, dynamic_memory->allocate, dynamic_memory->deallocate};
    cl = CL_createCommunicationLayer(commConfig);
}

void tearDown(void){
    spi->destroy(spi);
    cl->destroy(cl);
    TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_numberOfAllocatedObjects(dynamic_memory));
}

void test_SPIAndCLNotNull(void){
    TEST_ASSERT_NOT_NULL(spi);
    TEST_ASSERT_NOT_NULL(cl);
}

void test_InitNotNull(void){
    TEST_ASSERT_NOT_NULL(cl->init);
}

void test_isBusyNotNull(void){
    TEST_ASSERT_NOT_NULL(cl->isBusy);
}

void test_transferAsyncNotNull(void){
    TEST_ASSERT_NOT_NULL(cl->transferAsync);
}

void test_transferSyncNotNull(void){
    TEST_ASSERT_NOT_NULL(cl->transferSync);
}

void test_destroyNotNull(void){
    TEST_ASSERT_NOT_NULL(cl->destroy);
}

void test_spiNotNull(void){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *)cl;
    TEST_ASSERT_NOT_NULL(impl->peripheralInterface);
}

void test_transferAsyncMakesLayerBusy(void){
    cl->init(cl, spi);
    uint16_t length = 2;
    uint8_t input[length];
    uint8_t output[length];
    uint16_t index = 0;
    Message m = {
           input, output, index, length
    };
    cl->transferAsync(cl, &m);
    TEST_ASSERT_TRUE(cl->isBusy(cl));
}

void test_notBusyAtStart(void){
    cl->init(cl, spi);
    TEST_ASSERT_FALSE(cl->isBusy(cl));
}

void test_setInterruptHandlerNotNull(void){
    TEST_ASSERT_NOT_NULL(cl->setInterruptHandler);
}



void test_notBusyAfterTransmissionFinished(void){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *)cl;

    cl->init(cl, spi);
    uint16_t length = 2;
    uint8_t input[length];
    uint8_t output[length];
    uint16_t index = 0;
    Message m = {
            input, output, index, length
    };
    cl->transferAsync(cl, &m);
    uint16_t i;
    for(i = 0; i<=length; i++){
        TEST_ASSERT_TRUE(cl->isBusy(cl));
        impl->peripheralInterface->handleInterrupt(impl->peripheralInterface);
    }
    TEST_ASSERT_FALSE(cl->isBusy(cl));

}

void test_transmissionTransmitsCorrectData(void){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *)cl;

    cl->init(cl, spi);
    uint16_t length = 2;
    uint8_t input[length];
    input[0] = 3;
    input[1] = 4;
    uint8_t output[length];
    output[0] = 1;
    output[1] = 2;
    uint16_t index = 0;
    Message m = {
            input, output, index, length
    };
    cl->transferAsync(cl, &m);
    uint16_t i;
    for(i = 0; i<=length; i++){
        impl->peripheralInterface->handleInterrupt(impl->peripheralInterface);
    }
    TEST_ASSERT_EQUAL_UINT(1, input[0]);
    TEST_ASSERT_EQUAL_UINT(2, input[1]);
    TEST_ASSERT_EQUAL_UINT(1, output[0]);
    TEST_ASSERT_EQUAL_UINT(2, output[1]);
}

void test_transmissionTransmitsCorrectDataWhenHandledTheSamePointer(void){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *)cl;

    cl->init(cl, spi);
    uint16_t length = 2;
    uint8_t input[length];
    input[0] = 3;
    input[1] = 4;
    uint16_t index = 0;
    Message m = {
            input, input, index, length
    };
    cl->transferAsync(cl, &m);
    uint16_t i;
    for(i = 0; i<=length; i++){
        impl->peripheralInterface->handleInterrupt(impl->peripheralInterface);
    }
    TEST_ASSERT_EQUAL_UINT(3, input[0]);
    TEST_ASSERT_EQUAL_UINT(4, input[1]);
}

void test_transmissionTransmitsCorrectDataChangeSPDR(void){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *)cl;
    SPIImpl *spiImpl = (SPIImpl *)impl->peripheralInterface;

    cl->init(cl, spi);
    uint16_t length = 2;
    uint8_t input[length];
    input[0] = 3;
    input[1] = 4;
    uint8_t output[length];
    output[0] = 1;
    output[1] = 2;
    uint16_t index = 0;
    Message m = {
            input, output, index, length
    };

    uint8_t slave_out[length];
    slave_out[0] = 11;
    slave_out[1] = 12;
    cl->transferAsync(cl, &m);
    uint16_t i;
    for(i = 0; i<=length; i++){
        /*
         * Here we fake communication by setting the SPDR manually
         * In real life we would have MISO and MOSI, so that the slave could be able
         * to read the received stuff
         */
        *(spiImpl->spdr) = slave_out[i];
        impl->peripheralInterface->handleInterrupt(impl->peripheralInterface);
    }
    TEST_ASSERT_EQUAL_UINT(11, input[0]);
    TEST_ASSERT_EQUAL_UINT(12, input[1]);
}

static void handleInterrupt(PeripheralInterface *self);


void test_setInterruptHandler(void){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *)cl;

    cl->init(cl, spi);
    cl->setInterruptHandler(cl, handleInterrupt);

    uint16_t length = 2;
    uint8_t input[length];
    uint8_t output[length];
    uint16_t index = 0;
    Message m = {
            input, output, index, length
    };
    cl->transferAsync(cl, &m);
    uint16_t i;
    for(i = 0; i<=length; i++){
        TEST_ASSERT_TRUE(cl->isBusy(cl));
        impl->peripheralInterface->handleInterrupt(impl->peripheralInterface);
    }
    TEST_ASSERT_FALSE(cl->isBusy(cl));

}

void handleInterrupt(PeripheralInterface *self){


    Message *m = self->interruptData->m;
    PeripheralInterface *peripheral = self->interruptData->peripheral;
    m->inputBuffer[m->index] = peripheral->read(peripheral);
    if(m->index < m->length){
        peripheral->write(peripheral,m->outputBuffer[++(m->index)]);
    }
    else{
        self->interruptData->busy = false;
    }


}