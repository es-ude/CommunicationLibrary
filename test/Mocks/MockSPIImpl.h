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
	SPI device;
	uint8_t *output_buffer;
  uint8_t *input_buffer;
  uint8_t current_buffer_position;
	uint8_t number_of_async_transfer_calls;
	uint8_t number_of_sync_transfer_calls;
	bool isBusy;
};

void SPIDeviceMockImpl_init(SPIDeviceMockImpl *device);

#endif /* end of include guard */

