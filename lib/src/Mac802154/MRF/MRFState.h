#ifndef COMMUNICATIONMODULE_MRFSTATE_H
#define COMMUNICATIONMODULE_MRFSTATE_H

#include "lib/include/Mac802154.h"
#include "lib/src/Mac802154/FrameHeader802154.h"


typedef struct MRFSendContext MRFSendContext;

typedef PeripheralCallback MRFWriteCallback;

typedef union MRFHeader {
  struct {
    uint8_t header_length;
    uint8_t frame_length;
    FrameHeader802154 *header;
  } fields;
} MRFHeader;
/**
 * This struct holds all data needed for asynchronously writing a
 * 80154 frame to the mrf chip.
 */
typedef struct MRFSendContext {
  MRFHeader header;
  const uint8_t *payload;
  uint8_t current_command[2];
  PeripheralInterface *hw_interface;
  Peripheral *device;
  uint8_t state; //<!-- represents which fields need to be updated (i.e. what function to set for the write_callback)
  MRFWriteCallback write_callback;
} MRFSendContext;

void MRFSendContext_setExtendedAddress(MRFSendContext *self, uint64_t address);
void MRFSendContext_setShortAddress(MRFSendContext *self, uint16_t address);
void MRFSendContext_setPayload(MRFSendContext *self, const uint8_t *payload, uint16_t size);
void MRFSendContext_setPeripheral(MRFSendContext *self, Peripheral *device);
void MRFSendContext_setPeripheralInterface(MRFSendContext *self, PeripheralInterface *interface);
void MRFSendContext_setPanId(MRFSendContext *self, uint16_t pan_id);
void MRFSendContext_runSendSequence(MRFSendContext *self);
#endif //COMMUNICATIONMODULE_MRFSTATE_H
