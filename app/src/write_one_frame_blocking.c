#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/Peripheral.h"
#include "lib/include/usart/USART.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "lib/include/Mac802154.h"
#include "lib/include/Mac802154MRFImpl.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"
#include "lib/src/Mac802154/MRF/MRFState.h"
#include "lib/src/Mac802154/MRF/Mrf.h"


static uint8_t f_osc = f_osc_16;
PeripheralInterface *interface;

typedef struct SPIPeripheral{
  volatile uint8_t *DDR;
  uint8_t PIN;
  volatile  uint8_t *PORT;
} SPIPeripheral;

void printFramePayloadFromMrf(SPIPeripheral *device, uint8_t payload_length);
void writeToTXMemory(Peripheral *device, const uint8_t *payload, uint8_t payload_length);

void delay_ten_times(double ms) {
  while (ms > 0) {
    _delay_ms(10);
    ms--;
  }
}


int main() {
  uint8_t spi_slave = 1;
  struct SPIPeripheral device = {&DDRB, spi_slave, &PORTB};
  SPIConfig spiConfig = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc};


  TransferLayerConfig transferConfig = {malloc, free};
  interface = PeripheralInterface_create(transferConfig, spiConfig);


  interface->init(interface);
  interface->configurePeripheral(interface, &device);
  uint16_t baud_rate = 9600;
  USART_init(baud_rate);
  uint8_t buff[] = "Start\r\n";

  USART_writeN(buff,7);

  uint8_t *buffer = "Hello, World";
  while(1) {
    _delay_ms(500);
    buffer[0]++;
    USART_writeN("sending\r\n", 9);
    writeToTXMemory(&device, "Hello, World!", 13);
    _delay_ms(500);
    printFramePayloadFromMrf(&device, 13);
  }
}

void printFramePayloadFromMrf(SPIPeripheral *device, uint8_t payload_length) {

  uint8_t buffer[payload_length];
  uint8_t command[2] = {0x80, 0};
  PeripheralInterface_selectPeripheral(interface, device);
  PeripheralInterface_writeBlocking(interface, command, 2);
  PeripheralInterface_readBlocking(interface, buffer, payload_length);
  PeripheralInterface_deselectPeripheral(interface, device);
  USART_writeN("payload: ", 9);

  USART_writeN(buffer, payload_length);
  USART_write('\n');
  USART_write('\r');
}

void writeToTXMemory(Peripheral *device, const uint8_t *payload, uint8_t payload_length){
  uint8_t write_command[] = {0x80, 0x10};
  PeripheralInterface_selectPeripheral(interface, device);
  PeripheralInterface_writeBlocking(interface, write_command, 2);
  PeripheralInterface_writeBlocking(interface, payload, payload_length);
  PeripheralInterface_deselectPeripheral(interface, device);
}

ISR(SPI_STC_vect){
  interface->handleInterrupt();
}

