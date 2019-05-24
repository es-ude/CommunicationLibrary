#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Setup/HardwareSetup.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "Setup/LUFAHelpers.h"
#include "Setup/DebugSetup.h"
#include "Util/Debug.h"
#include "PeripheralInterface/PeripheralInterface.h"
#include "integration_tests/HelpersForUsageWithoutCommModule.h"


int
main(void)
{
  setUpPeripheral();
  setUpDebugging();
  _delay_ms(1000);
  debug(String, "start\n");
  initMrf();

  enablePromiscuous();
  while (1)
  {
    _delay_ms(1000);
    while (!gotNewMessage())
    {
    }
    uint8_t packet_size = getReceivedMessageSize();
    uint8_t packet[packet_size];
    readBurstFromLongAddress(mrf_rx_fifo_start, packet, packet_size);
    for(int i=0;i<packet_size;i++)
    {
      debugPrintHex(packet[i]);
    }
    debug(String, "\n");
    debugSized(packet+10, packet_size-12);
    debug(String,"\n");
  }
}
