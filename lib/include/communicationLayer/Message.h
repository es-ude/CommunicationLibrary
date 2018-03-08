//
// Created by Peter Zdankin on 02.03.18.
//

#ifndef COMMUNICATIONMODULE_MESSAGE_H
#define COMMUNICATIONMODULE_MESSAGE_H

#include <stdint.h>

typedef struct Message Message;

/**
 * If inputBuffer and outputBuffer point to the same memory location,
 * The outputbuffer will be used to store arriving messages
 */
struct Message{
    /**
     * Where to store MISO Bytes
     */
    uint8_t *inputBuffer;
    /**
     * Which bytes to send over MOSI
     */
    uint8_t *outputBuffer;
    /**
     * The currently transmitted index
     */
    uint16_t index;
    /**
     * The total length of the message
     */
    uint16_t length;

};

#endif //COMMUNICATIONMODULE_MESSAGE_H
