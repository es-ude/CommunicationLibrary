#ifndef COMMUNICATIONMODULE_MRFIO_H_H
#define COMMUNICATIONMODULE_MRFIO_H_H

#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"

/**
 * IO Module for the MRF Network Chip
 */

typedef struct MrfIo MrfIo;

void MrfIo_writeBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint16_t address);
void MrfIo_writeBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint8_t address);
void MrfIo_writeNonBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MrfIo_setCallback(MrfIo *mrf, MRFCallback callback);
void MrfIo_writeNonBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MrfIo_readBlockingFromLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MrfIo_readNonBlockingFromLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MrfIo_readBlockingFromShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MrfIo_readNonBlockingFromShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);


struct MrfIo {
  Peripheral *device;
  PeripheralInterface *interface;
  uint8_t command[2];
  uint8_t command_size;
  uint8_t length;
  const uint8_t *output_buffer;
  uint8_t *input_buffer;
  MRFCallback callback;
};

#endif //COMMUNICATIONMODULE_MRFIO_H_H
