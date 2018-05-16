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
    uint8_t buff[] = "Start";

    USART_writeN(buff, 5);
    sei();

    uint8_t buffer[] = "Writing Blocking";
    uint16_t length = sizeof(buffer) / sizeof(uint8_t);

    //Test writing
/*
    interface->selectPeripheral(interface, &device);
    interface->writeBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);
*/

    //Test writing
    /*
    interface->selectPeripheral(interface, &device);
    interface->readBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);
    UART_transmitStrN(buffer, length);
    UART_transmitStr("Continuing\r\n");
    */

    //Test NonBlocking Writing
    interface->selectPeripheral(interface, &device);
    interface->writeNonBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);
    while(1){

    }

    //Test NonBlocking Reading
    /*
    interface->selectPeripheral(interface, &device);
    interface->readNonBlocking(interface, buffer, length);
    interface->deselectPeripheral(interface, &device);
    USART_writeN(buffer, length);
     */

}

