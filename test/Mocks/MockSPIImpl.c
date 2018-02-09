#include <string.h>
#include "test/Mocks/MockSPIImpl.h"

static void transfer(const SPIDevice *device, const SPIMessage *message);
static void transferSync(const SPIDevice *device, const SPIMessage *message);
static void transferAsync(const SPIDevice *device, const SPIMessage *message);
static void init(SPIDevice *device);

void SPIDeviceMockImpl_init(SPIDeviceMockImpl *device) {
  device->device.transferSync = transferSync;
  device->device.transferAsync = transferAsync;
  device->device.init = init;
  device->number_of_sync_transfer_calls = 0;
  device->number_of_async_transfer_calls = 0;
}

void init(SPIDevice *device) {}

void transfer(const SPIDevice *device, const SPIMessage *message) {
  SPIDeviceMockImpl *self = (SPIDeviceMockImpl *) device;
  self->current_buffer_position += message->length;
  if (message->incoming_data != NULL) {
    memcpy(message->incoming_data, self->input_buffer, message->length);
  }
  if (message->outgoing_data != NULL) {
    memcpy(self->output_buffer, message->outgoing_data, message->length);
  }
}

void transferSync(const SPIDevice *device, const SPIMessage *message) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device;
  mock->number_of_sync_transfer_calls++;
  transfer(device, message);
}

void transferAsync(const SPIDevice *device, const SPIMessage *message) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device;
  mock->number_of_async_transfer_calls++;
  transfer(device, message);
}
