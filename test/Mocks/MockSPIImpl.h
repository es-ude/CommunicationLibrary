#ifndef MOCK_SPI_H
#define MOCK_SPI_H

#include "lib/include/SPI.h"
#include <stdbool.h>

typedef struct SPIDeviceMockImpl SPIDeviceMockImpl;

/**
 * The message_copy field contains a copy of the
 * SPIMessage that the caller of the transfer functions
 * owns.
 */
struct SPIDeviceMockImpl {
	SPIDevice device;
	uint8_t *output_buffer;
  uint8_t *input_buffer;
  uint8_t current_buffer_position;
	bool transfer_async_called;
	bool transfer_sync_called;
};

void SPIDeviceMockImpl_init(SPIDeviceMockImpl *device);

#endif /* end of include guard */

