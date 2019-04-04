#ifndef FRAMEHEADER802154_STRUCT_H
#define FRAMEHEADER802154_STRUCT_H

#define MAXIMUM_HEADER_SIZE 21

typedef struct FrameHeader802154 FrameHeader802154;

struct FrameHeader802154 {
    uint8_t data[MAXIMUM_HEADER_SIZE];
};

#endif