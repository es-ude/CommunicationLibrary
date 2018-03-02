//
// Created by Peter Zdankin on 02.03.18.
//

#ifndef COMMUNICATIONMODULE_MESSAGE_H
#define COMMUNICATIONMODULE_MESSAGE_H

#include <stdint.h>

typedef struct Message Message;

struct Message{
    /**
     * The message will be used to send a message and store arriving messages
     */
    uint8_t *message;
    /**
     * The currently transmitted index
     */
    uint16_t index;
    /**
     * The total length of the message
     */
    uint16_t length;

    /**
     * A message is sent to a SPI Slave
     * To address it, we need the DDR of the SS Pin and
     * the pin itself
     */
    volatile uint8_t *slaveSelect_DDR;
    uint8_t SS_Pin;
};

#endif //COMMUNICATIONMODULE_MESSAGE_H
