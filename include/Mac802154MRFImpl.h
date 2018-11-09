#ifndef NETWORK_HARDWARE_PERIPHERAL_IMPL_H
#define NETWORK_HARDWARE_PERIPHERAL_IMPL_H

#include "include/Mac802154.h"

/*!
 * \file Mac802154MRFImpl.h
 *
 * This header is used to provide the function for initializing
 * an ADT implementing the Mac802154 interface for the MRF24J40MA.
 * The only relevant thing that needs to be mentioned here
 * is the DelayFunction. There are cases where the interaction
 * with the MRF Chip has to be paused for some microseconds.
 * The delay function handed to the Mac802154_create function
 * should therefore block for at least the specified amount of
 * microseconds.
 *
 * The PeripheralInterface is assumed to be in an initialized state.
 *
 */
typedef void (*DelayFunction) (uint16_t amount);

size_t Mac802154MRF_getADTSize (void);

void Mac802154MRF_create(Mac802154 memory,
                         DelayFunction delay_microseconds,
                         PeripheralInterface interface,
                         Peripheral *device);
#endif /* end of include guard */
