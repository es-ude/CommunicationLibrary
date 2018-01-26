#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

typedef struct Message Message;

struct Message {
  uint8_t address[2];
  uint8_t *payload;
};

#endif /* MESSAGE_H */
