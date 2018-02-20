#include <string.h>
#include "test/Mocks/MockSPIImpl.h"
#include "CException.h"
#include "lib/include/Exception.h"

static void transfer(const SPISlave *device, const SPIMessage *message);
static void transferSync(const SPISlave *device, const SPIMessage *message);
static void transferAsync(const SPISlave *device, const SPIMessage *message);
static void init(SPI *device);
static void copyOutgoingDataToInternalBuffer(const SPIDeviceMockImpl *device, const SPIMessage *message);
static uint8_t getTotalNumberOfMessages(const SPIDeviceMockImpl *device);
static SPIMessage *getLastMessage(const SPIDeviceMockImpl *self);
static void rebuildMessageInternally(SPIDeviceMockImpl *self, const SPIMessage *message);
static void updateInternalBufferPosition(SPIDeviceMockImpl *self, const SPIMessage *message);
static void setLastMessagesNextField(const SPIDeviceMockImpl *self, const SPIMessage *message);

void SPIDeviceMockImpl_init(SPIDeviceMockImpl *device) {
  device->device.transferSync = transferSync;
  device->device.transferAsync = transferAsync;
  device->device.init = init;
  device->number_of_sync_transfer_calls = 0;
  device->number_of_async_transfer_calls = 0;
  device->current_buffer_position = 0;
  device->isBusy = false;
}

bool SPIDeviceMockImpl_messageWasTransferred(SPIDeviceMockImpl *self, SPIMessage *message) {
  for (uint16_t index = 0; index < getTotalNumberOfMessages(self); index++) {
    if (SPIMessage_equal(self->message_buffer + index, message)){
      return true;
    }
  }
  return false;
}


void init(SPI *device) {}

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

void transfer(const SPISlave *device, const SPIMessage *message) {
  SPIDeviceMockImpl *self = (SPIDeviceMockImpl *) device->hw_interface;

  if (self->isBusy) {
    Throw (SPI_BUSY_EXCEPTION);
  }
  rebuildMessageInternally(self, message);
  updateInternalBufferPosition(self, message);
}

void rebuildMessageInternally(SPIDeviceMockImpl *self, const SPIMessage *message) {
  SPIMessage *last_message = getLastMessage(self);
  SPIMessage_init(last_message);
  last_message->length = message->length;
  setLastMessagesNextField(self, message);
  copyOutgoingDataToInternalBuffer(self, message);
}

void copyOutgoingDataToInternalBuffer(const SPIDeviceMockImpl *self, const SPIMessage *message) {
  if (message->outgoing_data != NULL) {
    SPIMessage *last_message = getLastMessage(self);
    last_message->outgoing_data = self->output_buffer + self->current_buffer_position;
    memcpy(last_message->outgoing_data,
           message->outgoing_data,
           message->length);
  }
}

void setLastMessagesNextField(const SPIDeviceMockImpl *self, const SPIMessage *message) {
  SPIMessage *last_message = getLastMessage(self);
  if (message->next != NULL) {
    last_message->next = last_message + 1;
  } else {
    last_message->next = NULL;
  }
}

void updateInternalBufferPosition(SPIDeviceMockImpl *self, const SPIMessage *message) {
  self->current_buffer_position += message->length;
}

SPIMessage *getLastMessage(const SPIDeviceMockImpl *self) {
  return self->message_buffer + getTotalNumberOfMessages(self) - 1;
}

uint8_t getTotalNumberOfMessages(const SPIDeviceMockImpl *self) {
  return self->number_of_sync_transfer_calls + self->number_of_async_transfer_calls;
}