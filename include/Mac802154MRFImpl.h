#ifndef NETWORK_HARDWARE_PERIPHERAL_IMPL_H
#define NETWORK_HARDWARE_PERIPHERAL_IMPL_H

#include "include/Mac802154.h"
#include "include/RuntimeLibraryInterface.h"

size_t Mac802154MRF_requiredSize(void);
void Mac802154MRF_create(uint8_t *memory, DelayFunction delay_microseconds);
#endif /* end of include guard */
