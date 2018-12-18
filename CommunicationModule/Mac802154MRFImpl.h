#ifndef NETWORK_HARDWARE_PERIPHERAL_IMPL_H
#define NETWORK_HARDWARE_PERIPHERAL_IMPL_H

#include "CommunicationModule/Mac802154.h"

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
typedef void (*DelayFunction)(uint16_t amount);
typedef struct MRFConfig MRFConfig;

size_t
Mac802154MRF_getADTSize(void);

void
Mac802154MRF_create(Mac802154 memory,
                    MRFConfig *config);

typedef struct GPIOPin {
                    volatile uint8_t *data_direction_register;
                    volatile uint8_t *data_register;
                    uint8_t pin_number;
} GPIOPin;

struct MRFConfig
{
                    uint8_t transmitter_power;
                    GPIOPin reset_line;
                    DelayFunction delay_microseconds;
                    PeripheralInterface *interface;
                    Peripheral *device;
};
#endif /* end of CommunicationModule guard */
