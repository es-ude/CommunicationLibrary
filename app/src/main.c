#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "lib/include/TransferLayer/PeripheralInterface.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "lib/include/platform/io.h"
#include "lib/include/communicationLayer/InterruptData.h"
#include "CException.h"
#include "lib/include/Exception.h"

static uint8_t f_osc = f_osc_16;

int main(){
  DDRB |= (1<<PB5);
  SPIConfig spiConfig = {&DDRB, &PORTB, &SPCR, &SPDR, &SPSR, f_osc};
  TransferLayerConfig transferConfig = {malloc, free};
  PeripheralInterface *interface = PeripheralInterface_create(transferConfig, spiConfig);
  interface->init(interface);
    PORTB |= (1<<PB5);
}
