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
    SPIConfig config = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc, dynamic_memory->allocate, dynamic_memory->deallocate, interruptData};
    spi = SPI_createSPI(config);
    cl = CL_createCommunicationLayer(spi, malloc);
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