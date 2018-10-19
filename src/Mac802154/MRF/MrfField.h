#ifndef COMMUNICATIONMODULE_MRFFIELD_H
#define COMMUNICATIONMODULE_MRFFIELD_H

#include <stdint.h>
#include <stddef.h>

typedef struct MrfField MrfField;

struct MrfField
{
  uint16_t address;
  const uint8_t *data;
  size_t length;
};

#endif //COMMUNICATIONMODULE_MRFFIELD_H
