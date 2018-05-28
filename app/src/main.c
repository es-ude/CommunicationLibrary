#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lib/include/TransferLayer/PeripheralInterface.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "lib/include/platform/io.h"
#include "CException.h"
#include "lib/include/Exception.h"
#include "lib/include/TransferLayer/InterruptData.h"
#include "lib/include/usart/USART.h"

static uint8_t f_osc = f_osc_16;
PeripheralInterface *interface;

struct Peripheral{
    volatile uint8_t *DDR;
    uint8_t PIN;
    volatile  uint8_t *PORT;
};





int main() {
    uint8_t spi_slave = 1;
    Peripheral device = {&DDRB, spi_slave, &PORTB};
    SPIConfig spiConfig = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc};


    TransferLayerConfig transferConfig = {malloc, free};
    interface = PeripheralInterface_create(transferConfig, spiConfig);


    interface->init(interface);
    interface->configurePeripheral(interface, &device);
    uint16_t baud_rate = 9600;
    USART_init(baud_rate);
    uint8_t buff[] = "Start\r\n";

    USART_writeN(buff, 5);
    sei();

    uint8_t buffer[] = "Writing Blocking";
    uint16_t length = sizeof(buffer) / sizeof(uint8_t);

    uint8_t wb[] = "\r\nWriting Blocking\r\n";
    uint16_t lengthwb = sizeof(wb) / sizeof(uint8_t);
    uint8_t rb[] = "\r\nReading Blocking\r\n";
    uint16_t lengthrb = sizeof(rb) / sizeof(uint8_t);
    uint8_t wnb[] = "\r\nWriting Nonblocking\r\n";
    uint16_t lengthwnb = sizeof(wnb) / sizeof(uint8_t);
    uint8_t rnb[] = "\r\nReading Nonblocking\r\n";
    uint16_t lengthrnb = sizeof(rnb) / sizeof(uint8_t);


    //Test writing
    USART_writeN(wb, lengthwb);
    interface->selectPeripheral(interface, &device);
    interface->writeBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);

    //Test reading
    USART_writeN(rb, lengthrb);
    interface->selectPeripheral(interface, &device);
    interface->readBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);
    USART_writeN(buffer, length);

    //Test NonBlocking Writing
    USART_writeN(wnb, lengthwnb);
    interface->selectPeripheral(interface, &device);
    interface->writeNonBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);
     while(interface->isBusy(interface)){

    }


    //Test NonBlocking Reading
    USART_writeN(rnb, lengthrnb);
    interface->selectPeripheral(interface, &device);
    interface->readNonBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);
    while(interface->isBusy(interface)){

    }

    USART_writeN(buffer, length);

}

ISR(SPI_STC_vect){
    interface->handleInterrupt();
}

