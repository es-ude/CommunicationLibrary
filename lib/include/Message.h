#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct Message Message;

struct Message {
  uint8_t address[2];
  uint8_t *payload;
};

#endif /* MESSAGE_H */
