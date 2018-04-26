#ifndef COMMUNICATIONMODULE_MRFIO_H_H
#define COMMUNICATIONMODULE_MRFIO_H_H

#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"

/**
 * IO Module for the MRF Network Chip
 */

typedef struct MrfIo MrfIo;

void MRFIO_writeBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MRFIO_writeNonBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MRFIO_setCallback(MrfIo *mrf, MRFCallback callback);
void MRFIO_writeBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MRFIO_writeNonBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MRFIO_readBlockingFromLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MRFIO_readNonBlockingFromLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MRFIO_readBlockingFromShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MRFIO_readNonBlockingFromShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);


struct MrfIo {
  Peripheral *device;
  PeripheralInterface *interface;
  uint8_t command[2];
  uint8_t length;
  const uint8_t *output_buffer;
  uint8_t *input_buffer;
  MRFCallback callback;
};

#endif //COMMUNICATIONMODULE_MRFIO_H_H
