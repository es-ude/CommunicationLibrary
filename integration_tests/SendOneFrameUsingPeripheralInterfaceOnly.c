#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Setup/HardwareSetup.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"

#include "Setup/DebugSetup.h"
#include "Util/Debug.h"
#include "PeripheralInterface/PeripheralInterface.h"
#include "integration_tests/HelpersForUsageWithoutCommModule.h"

/*
 * This test sends one frame containing the payload 'aa'
 * to an XBee with the address 0013A2004175A89D
 * Setup of the MRF Chip is performed as shown in it's data sheet.
 * The control bytes of the frame header are set as follows:
 * +---+-----------------------------+
 * | 0 | Reserved                    |
 * +---+-----------------------------+
 * | 1 | Pan Id Compression          |
 * +---+-----------------------------+
 * | 0 | Acknowledgement Request     |
 * +---+-----------------------------+
 * | 0 | Frame Pending               |
 * +---+-----------------------------+
 * | 0 | Security Enabled            |
 * +---+-----------------------------+
 * | 0 |                             |
 * | 0 | Frame Type - Data Frame     |
 * | 1 |                             |
 * +---+-----------------------------+
 * | 1 | Source Addressing Mode      |
 * | 0 | 16 bit                      |
 * +---+-----------------------------+
 * | 1 | Frame Version               |
 * | 0 | 2015                        |
 * +---+-----------------------------+
 * | 1 | Destination Addressing      |
 * | 0 | Mode 64bit                  |
 * +---+-----------------------------+
 * | 0 | Information Element Present |
 * +---+-----------------------------+
 * | 0 | Sequence Number Suppression | // 2015 version feature, not supported by XbeeS2C
 * +---+-----------------------------+
 *
 * Note that the bit order shown is reversed from the
 * order one can see in the ieee specs of 802.15.4
 * We need to work with the least significant bit in
 * last position here.
 * The Pan Id, Destination and Source Addresses are
 * represented in little endian.
 * */
static uint8_t frame[] = {
        // frame header length, frame length
        0x09, 0x0B,
        // frame header control section
        0b01000001, 0b10101000,
        // sequence number
        0x00,
        // 16 bit source address
        0x34, 0x12,
        // 16 bit destination address
        0xBB, 0xBB,
        // destination pan id
        0x34, 0x12,
        // payload; just the string 'aa'
        0x61, 0x61,
};

void sendToCoordinator(void) {
  writeBurstToLongAddress(mrf_tx_fifo_start, frame, frame[1]);
  setRegister(mrf_register_tx_normal_fifo_control, 1);
}

int main(void) {
  setUpDebugging();

  setUpPeripheral();
  initMrf();


  while(1) {
    _delay_ms(1000);
    debug(String, "sending\n");
    sendToCoordinator();
    uint8_t text[8];
    uint8_t value = readByteFromShortAddress(0x24);
    sprintf(text, "0x%02x\n", value);
    debug(String, text);
  }
}