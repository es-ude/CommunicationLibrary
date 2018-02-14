
#ifndef COMMUNICATIONMODULE_SPIMESSAGE_H
#define COMMUNICATIONMODULE_SPIMESSAGE_H


#include <stdint.h>

typedef struct SPIMessage SPIMessage;

/*
 * When creating a new SPIMessage, the memory
 * areas provided through outgoing_data and incoming_data,
 * must have enough space for "length" number of bytes.
 * E.g.:
 * ```
 * uint8_t out[i];
 * uint8_t int[j];
 * SPIMessage m = { .length = minimum(i,j),
 *                  .outgoing_data = out,
 *                  .incoming_data = in };
 * ```
 *
 */
struct SPIMessage {
  uint8_t length;
  uint8_t* outgoing_data;
  uint8_t* incoming_data;
  SPIMessage *next;
};

static inline void SPIMessage_append(SPIMessage *head, SPIMessage *tail) {
  head->next = tail;
}

static inline void SPIMessage_init(SPIMessage *self) {
  self->length = 0;
  self->incoming_data = NULL;
  self->outgoing_data = NULL;
  self->next = NULL;
}

#endif //COMMUNICATIONMODULE_SPIMESSAGE_H
