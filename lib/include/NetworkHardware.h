#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lib/include/Message.h"

typedef struct NetworkHardware NetworkHardware;
typedef struct NetworkHardwareConfig NetworkHardwareConfig;

struct NetworkHardware {
  void (*setShortDestinationAddress) (NetworkHardware *self, uint16_t address);
  void (*setExtendedDestinationAddress) (NetworkHardware *self, uint8_t* address);
  void (*setPayload) (NetworkHardware *self, uint8_t* buffer, size_t size);

  void (*associate) (NetworkHardware *self);
  void (*receive) (NetworkHardware *self, Message *received_msg);
  void (*send) (NetworkHardware *self);
  void (*init) (NetworkHardware *self, const NetworkHardwareConfig *config);
  void (*destroy) (NetworkHardware *self);
  bool (*isAssociated) (NetworkHardware *self);
  uint16_t (*getAddress16Bit) (NetworkHardware *self);
  const uint8_t *(*getAddress64Bit) (NetworkHardware *self);
  const uint8_t *(*getPayload) (NetworkHardware *self);
  uint16_t (*getPayloadSize) (NetworkHardware *self);
};

struct NetworkHardwareConfig {
  uint16_t short_source_address;
  uint8_t extended_source_address[8];
  uint16_t pan_id;
};

static inline void NetworkHardware_init(NetworkHardware *hardware,
                                        const NetworkHardwareConfig *config) {
  hardware->init(hardware, config);
}


#endif //COMMUNICATIONMODULE_NETWORKHARDWARE_H
