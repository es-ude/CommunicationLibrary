#include <string.h>
#include "test/Mocks/MockSPIImpl.h"
#include "CException.h"
#include "lib/include/Exception.h"

static void transfer(const SPISlave *device, const SPIMessage *message);
static void transferSync(const SPISlave *device, const SPIMessage *message);
static void transferAsync(const SPISlave *device, const SPIMessage *message);
static void init(SPI *device);
static void destroy(SPI *device);
static uint8_t getTotalNumberOfMessages(const SPIDeviceMockImpl *device);
static bool spiMessagesAreEqual(const SPIMessage *left, const SPIMessage *right);
static bool byteArraysAreEqual(const uint8_t *left, const uint8_t *right, uint8_t size);

void SPIDeviceMockImpl_init(SPIDeviceMockImpl *device) {
  device->device.transferSync = transferSync;
  device->device.transferAsync = transferAsync;
  device->device.init = init;
  device->number_of_sync_transfer_calls = 0;
  device->number_of_async_transfer_calls = 0;
  device->current_buffer_position = 0;
  device->isBusy = false;
}

void init(SPI *device) {}

void transfer(const SPISlave *device, const SPIMessage *message) {
  SPIDeviceMockImpl *self = (SPIDeviceMockImpl *) device->hw_interface;

  if (self->isBusy) {
    Throw (SPI_BUSY_EXCEPTION);
  }
  SPIMessage *last_message = self->message_buffer + getTotalNumberOfMessages(self)-1;
  last_message->length = message->length;
  last_message->incoming_data = NULL;
  last_message->outgoing_data = NULL;
  if (message->incoming_data != NULL) {
    last_message->incoming_data = self->input_buffer + self->current_buffer_position;
    memcpy(message->incoming_data,
           last_message->incoming_data,
           message->length);
  }
  if (message->outgoing_data != NULL) {
    last_message->outgoing_data = self->output_buffer + self->current_buffer_position;
    memcpy(last_message->outgoing_data,
           message->outgoing_data,
           message->length);
  }
  self->current_buffer_position += message->length;
}

void transferSync(const SPISlave *device, const SPIMessage *message) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device->hw_interface;
  mock->number_of_sync_transfer_calls++;
  transfer(device, message);
  if (message->next != NULL) {
    transferSync(device, message->next);
  }
}

void transferAsync(const SPISlave *device, const SPIMessage *message) {
  SPIDeviceMockImpl *mock = (SPIDeviceMockImpl *) device->hw_interface;
  mock->number_of_async_transfer_calls++;
  transfer(device, message);
  if (message->next != NULL) {
    transferAsync(device, message->next);
  }
}

void destroy(SPI *device) {}


bool SPIDeviceMockImpl_messageWasTransferred(SPIDeviceMockImpl *self, SPIMessage *message) {
  for (uint16_t index = 0; index < getTotalNumberOfMessages(self); index++) {
    if (spiMessagesAreEqual(self->message_buffer + index, message)){
      return true;
    }
  }
  return false;
}


static bool byteArraysAreEqual(const uint8_t *left, const uint8_t *right, uint8_t size) {
  if (left == right) return true;
  if ((left == NULL && right != NULL) || (left != NULL && right == NULL)) return false;
  for (uint8_t i=0; i <size ; i++) {
    if(left[i] != right[i]) return false;
  }
  return true;
}

static bool spiMessagesAreEqual(const SPIMessage *left, const SPIMessage *right) {
  if (left == right) return true;
  if (left->length == right->length
      && left->outgoing_data == right->outgoing_data
      && left->incoming_data == right->incoming_data) return true;
  if (left->length == right->length
      && left->outgoing_data != NULL
      && right->outgoing_data != NULL) {
    return byteArraysAreEqual(left->outgoing_data, right->outgoing_data, right->length);
  }
  return false;
}


uint8_t getTotalNumberOfMessages(const SPIDeviceMockImpl *self) {
  return self->number_of_sync_transfer_calls + self->number_of_async_transfer_calls;
}