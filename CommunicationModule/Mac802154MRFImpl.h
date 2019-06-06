#ifndef NETWORK_HARDWARE_PERIPHERAL_IMPL_H
#define NETWORK_HARDWARE_PERIPHERAL_IMPL_H

#include "CommunicationModule/Mac802154.h"
#include "CommunicationModule/FrameHeader802154Struct.h"

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

size_t
Mac802154MRF_getADTSize(void);

void
Mac802154MRF_create(Mac802154 *memory,
                    MRFConfig *config);


/**
 * ATTENTION:
 * Do not use any of the structs below directly,
 * they are just defined here publicly to allow
 * for static memory allocation!
 */
typedef struct Mrf Mrf;
typedef struct MrfIo MrfIo;
typedef struct MrfState MrfState;
typedef struct MrfHeader MrfHeader;
typedef struct MrfIoCallback MrfIoCallback;
typedef struct MrfIo_NonBlockingWriteContext MrfIo_NonBlockingWriteContext;

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

struct MrfHeader {
    uint8_t frame_header_length;
    uint8_t frame_length;
    FrameHeader802154 frame_header;
};

struct MrfState {
    uint8_t state;
    MrfHeader header;
    const uint8_t *payload;
};

struct Mrf {
    Mac802154 mac;
    MrfIo io;
    void (*delay_microseconds)(uint16_t);
    MrfState state;
    Mac802154Config config;
};




#endif /* end of CommunicationModule guard */
