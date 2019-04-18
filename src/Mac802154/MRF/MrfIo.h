#ifndef COMMUNICATIONMODULE_MRFIO_H_H
#define COMMUNICATIONMODULE_MRFIO_H_H

#include <stdint.h>
#include "CommunicationModule/Peripheral.h"

/**
 * IO Module for the MRF Network Chip
 */

typedef struct MrfIo MrfIo;
typedef struct MrfIoCallback MrfIoCallback;
typedef struct  MrfIo_NonBlockingWriteContext MrfIo_NonBlockingWriteContext;


void MrfIo_writeBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint16_t address);
void MrfIo_writeBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint8_t address);
void MrfIo_writeNonBlockingToLongAddress(MrfIo *mrf, MrfIo_NonBlockingWriteContext context);
void MrfIo_writeNonBlockingToShortAddress(MrfIo *mrf, MrfIo_NonBlockingWriteContext context);

/**
 * Evaluates the register address to determine if it belongs to the short or long address space of
 * the mrf chip. Then synchronously writes that value to the address.
 * @param mrf
 * @param register_address
 * @param value
 */
void MrfIo_setControlRegister(MrfIo *mrf, uint16_t register_address, uint8_t value);
uint8_t MrfIo_readControlRegister(MrfIo *mrf, uint16_t register_address);
void MrfIo_readBlockingFromLongAddress(MrfIo *mrf, uint16_t register_address, uint8_t *buffer, uint8_t size);
void MrfIo_readNonBlockingFromLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MrfIo_readBlockingFromShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);
void MrfIo_readNonBlockingFromShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size);

struct MrfIoCallback {
  void (*function) (void *arg);
  void *argument;
};

struct MrfIo_NonBlockingWriteContext
{
  MrfIoCallback callback;
  const uint8_t *output_buffer;
  uint8_t length;
  uint16_t address;
};

struct MrfIo {
  Peripheral *device;
  PeripheralInterface *interface;
  uint8_t command[2];
  uint8_t command_size;
  uint8_t length;
  const uint8_t *output_buffer;
  MrfIoCallback callback;
};

#endif //COMMUNICATIONMODULE_MRFIO_H_H
