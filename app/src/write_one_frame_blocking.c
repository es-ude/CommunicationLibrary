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


static uint8_t f_osc = f_osc_16;
PeripheralInterface *interface;

struct SPIPeripheral{
  volatile uint8_t *DDR;
  uint8_t PIN;
  volatile  uint8_t *PORT;
};

void readFrame(struct SPIPeripheral *peripheral);

void delay(double ms) {
  while (ms > 0) {
    _delay_ms(1);
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

  MemoryManagement memoryManagement = {
          .allocate = malloc,
          .deallocate = free,
  };
  Mac802154 *mac = Mac802154_createMRF(&memoryManagement, delay);
  Mac802154Config config = {
          .interface = interface,
          .device = &device,
          .pan_id = 0xffff,
          .short_source_address = 1234,
          .channel = 11,
          .extended_source_address = 1234,
  };
  Mac802154_init(mac, &config);
  Mac802154_setPayload(mac, buff, 5);
  Mac802154_setExtendedDestinationAddress(mac, 0xffffffffffffffff);
  Mac802154_sendBlocking(mac);
  readFrame(&device);
  while(1) {
    _delay_ms(2000);
    USART_writeN("sending\r\n", 9);
    Mac802154_sendBlocking(mac);
  }
}

void readFrame(struct SPIPeripheral *device) {

  PeripheralInterface_selectPeripheral(interface, device);
  uint8_t command[2] = {(uint8_t)(MRF_readLongCommand(0) >> 8), (uint8_t) MRF_readLongCommand(0)};
  PeripheralInterface_writeBlocking(interface, command, 2);
  uint8_t buffer[40];
  PeripheralInterface_readBlocking(interface, buffer, 40);
  PeripheralInterface_deselectPeripheral(interface, device);
  USART_writeN(buffer, 40);

}

ISR(SPI_STC_vect){
  interface->handleInterrupt();
}

