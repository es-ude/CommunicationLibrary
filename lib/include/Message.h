#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

/**
 *  DEPRACATED
 */

typedef struct Message Message;

struct Message {
  uint8_t address[2];
  uint8_t *payload;
};

#endif /* MESSAGE_H */
