#include <string.h>
#include "test/Mocks/MockSPIImpl.h"

static void transfer(const SPI *device, const SPIMessage *message, volatile uint8_t *slave_select_line);
static void transferSync(const SPI *device, const SPIMessage *message, volatile uint8_t *slave_select_line);
static void transferAsync(const SPI *device, const SPIMessage *message, volatile uint8_t *slave_select_line);
static void init(SPI *device);
static void destroy(SPI *device);

void SPIDeviceMockImpl_init(SPIDeviceMockImpl *device) {
  device->device.transferSync = transferSync;
  device->device.transferAsync = transferAsync;
  device->device.init = init;
  device->number_of_sync_transfer_calls = 0;
  device->number_of_async_transfer_calls = 0;
  device->current_buffer_position = 0;
}

void init(SPI *device) {}

void transfer(const SPI *device, const SPIMessage *message, volatile uint8_t *slave_select_line) {
  SPIDeviceMockImpl *self = (SPIDeviceMockImpl *) device;
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

void transferSync(const SPI *device, const SPIMessage *message, volatile uint8_t *slave_select_line) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device;
  mock->number_of_sync_transfer_calls++;
  transfer(device, message, NULL);
}

void transferAsync(const SPI *device, const SPIMessage *message, volatile uint8_t *slave_select_line) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device;
  mock->number_of_async_transfer_calls++;
  transfer(device, message, NULL);
}

void destroy(SPI *device) {}