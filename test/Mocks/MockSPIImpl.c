#include <string.h>
#include "test/Mocks/MockSPIImpl.h"
#include "CException.h"
#include "lib/include/Exception.h"

static void transfer(const SPISlave *device, const SPIMessage *message);
static void transferSync(const SPISlave *device, const SPIMessage *message);
static void transferAsync(const SPISlave *device, const SPIMessage *message);
static void init(SPIMessageLayer *device);
static void destroy(SPIMessageLayer *device);

void SPIDeviceMockImpl_init(SPIDeviceMockImpl *device) {
  device->device.transferSync = transferSync;
  device->device.transferAsync = transferAsync;
  device->device.init = init;
  device->number_of_sync_transfer_calls = 0;
  device->number_of_async_transfer_calls = 0;
  device->current_buffer_position = 0;
  device->isBusy = false;
}

void init(SPIMessageLayer *device) {}

void transfer(const SPISlave *device, const SPIMessage *message) {
  SPIDeviceMockImpl *self = (SPIDeviceMockImpl *) device->messageModule;
  if (self->isBusy) {
    Throw (SPI_BUSY_EXCEPTION);
  }
  if (message->incoming_data != NULL) {
    memcpy(message->incoming_data,
           self->input_buffer + self->current_buffer_position,
           message->length);
  }
  if (message->outgoing_data != NULL) {
    memcpy(self->output_buffer + self->current_buffer_position,
           message->outgoing_data,
           message->length);
  }
  self->current_buffer_position += message->length;
}

void transferSync(const SPISlave *device, const SPIMessage *message) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device->messageModule;
  mock->number_of_sync_transfer_calls++;
  transfer(device, message);
  if (message->next != NULL) {
    transferSync(device, message->next);
  }
}

void transferAsync(const SPISlave *device, const SPIMessage *message) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device->messageModule;
  mock->number_of_async_transfer_calls++;
  transfer(device, message);
  if (message->next != NULL) {
    transferAsync(device, message->next);
  }
}

void destroy(SPIMessageLayer *device) {}