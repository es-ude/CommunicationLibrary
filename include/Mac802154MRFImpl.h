#ifndef NETWORK_HARDWARE_PERIPHERAL_IMPL_H
#define NETWORK_HARDWARE_PERIPHERAL_IMPL_H

#include "include/Mac802154.h"

typedef void (*DelayFunction) (uint16_t amount);

size_t Mac802154MRF_getADTSize (void);
void Mac802154MRF_create(uint8_t *memory, DelayFunction delay_microseconds);
#endif /* end of include guard */
