#include "CException.h"
#include "src/Peripheral/UsartIntern.h"

/**
 * Regarding the current implementation:
 * In case speed for selecting the usart peripheral
 * turns out to be an issue, we can work with predefined
 * cpu frequencies like we did for the baud rates above.
 * The result would be that the compiler can calculate
 * the values and place them into the code. This way
 * we wouldn't have to handle those large numbers during
 * runtime.
 */

static void
writeByte(PeripheralInterface *self, uint8_t byte);

static void
waitForEmptyTransmitBuffer(PeripheralInterfaceUsartImpl *self);

static void
waitForEndOfTransmission(PeripheralInterfaceUsartImpl *self);

static uint8_t
readByteBlocking(PeripheralInterface *self);

static void
setInterfacePointers(PeripheralInterface *self);

static void
selectUsartPeripheral(PeripheralInterface *self, Peripheral *peripheral);

static void
deselectPeripheral(PeripheralInterface *self,
                                   Peripheral *peripheral);

void
PeripheralInterfaceUsartImpl_createNew(PeripheralInterface *self,
                                       const UsartConfig *const config)
{
  PeripheralInterfaceUsartImpl *impl = (PeripheralInterfaceUsartImpl *)self;
  impl->config                       = *config;
  setInterfacePointers(self);
}

UsartPeripheral
PeripheralInterfaceUsartImpl_getDefaultPeripheral(void)
{
  UsartPeripheral peripheral = {
    .baud_rate = USART_BAUD_RATE_9600,
    .data_bits = USART_DATA_BITS_8,
    .stop_bits = USART_STOP_BIT_1,
    .mode      = USART_MODE_ASYNCHRONOUS_NORMAL,
    .parity    = USART_NO_PARITY,
  };
  return peripheral;
}

static void
setInterfacePointers(PeripheralInterface *self)
{
  self->writeByteBlocking  = writeByte;
  self->selectPeripheral   = selectUsartPeripheral;
  self->readByteBlocking   = readByteBlocking;
  self->deselectPeripheral = deselectPeripheral;
}

size_t
PeripheralInterfaceUsartImpl_getADTSize(void)
{
  return sizeof(struct PeripheralInterfaceUsartImpl);
}

void
writeByte(PeripheralInterface *self, uint8_t byte)
{
  PeripheralInterfaceUsartImpl *const impl =
    (PeripheralInterfaceUsartImpl *)self;
  waitForEmptyTransmitBuffer(impl);
  *impl->config.data_register = byte;
}

static void
waitForEndOfReception(PeripheralInterfaceUsartImpl *self)
{
  while (!(*self->config.control_and_status_register_a &
           (1 << usart_reception_complete_bit)))
    {
    }
}

static void
waitForEndOfTransmission(PeripheralInterfaceUsartImpl *self)
{
  while (!(*self->config.control_and_status_register_a &
        (1 << usart_transmit_complete_bit)))
  {
  }
  *self->config.control_and_status_register_a = (1 << usart_transmit_complete_bit);
}

uint8_t
readByteBlocking(PeripheralInterface *self)
{
  PeripheralInterfaceUsartImpl *const impl =
    (PeripheralInterfaceUsartImpl *)self;
  return *impl->config.data_register;
}

void
waitForEmptyTransmitBuffer(PeripheralInterfaceUsartImpl *self)
{
  while (!(*self->config.control_and_status_register_a &
           (1 << usart_data_register_empty_bit)))
    {
    }
}

static void
setBaudRate(PeripheralInterfaceUsartImpl *impl,
            UsartPeripheralBaudRate baud_rate)
{
  uint32_t baud_rate_numeric = baud_rate_9600;
  switch (baud_rate)
    {
      case USART_BAUD_RATE_2400:
        baud_rate_numeric = baud_rate_2400;
        break;
      case USART_BAUD_RATE_115200:
        baud_rate_numeric = baud_rate_115200;
        break;
      case USART_BAUD_RATE_9600:
        baud_rate_numeric = baud_rate_9600;
        break;
      default:
        Throw(PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION);
    }
  uint32_t baud_rate_register_value =
    impl->config.cpu_frequency / 16 / baud_rate_numeric - 1;
  *impl->config.baud_rate_register_high = baud_rate_register_value >> 8;
  *impl->config.baud_rate_register_low  = (uint8_t)baud_rate_register_value;
}

static void
setDataBits(PeripheralInterfaceUsartImpl *self,
            UsartPeripheralDataBits number_of_data_bits)
{
  switch (number_of_data_bits)
    {
      case USART_DATA_BITS_8:
        *self->config.control_and_status_register_c =
          (1 << usart_character_size_0_bit) |
          (1 << usart_character_size_1_bit);
        break;
      default:
        Throw(PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION);
    }
}

static void
setStopBits(PeripheralInterfaceUsartImpl *self,
            UsartPeripheralStopBits stop_bits)
{
  switch (stop_bits)
    {
      case USART_STOP_BIT_1:
        *self->config.control_and_status_register_c &= ~(1 << usart_stop_bit_select_bit);
        break;
      default:
        Throw(PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION);
    }
}

static void
setParity(PeripheralInterfaceUsartImpl *self, UsartPeripheralParity parity)
{
  switch (parity)
    {
      case USART_NO_PARITY:
        break;
      default:
        Throw(PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION);
    }
}

void
selectUsartPeripheral(PeripheralInterface *self, Peripheral *peripheral)
{
  PeripheralInterfaceUsartImpl *impl = (PeripheralInterfaceUsartImpl *)self;
  UsartPeripheral *usart_peripheral  = (UsartPeripheral *)peripheral;
  setBaudRate(impl, usart_peripheral->baud_rate);
  setDataBits(impl, usart_peripheral->data_bits);
  setStopBits(impl, usart_peripheral->stop_bits);
  setParity(impl, usart_peripheral->parity);
  *impl->config.control_and_status_register_b =
    (1 << usart_transmitter_enable_bit) | (1 << usart_receiver_enable_bit);
}

void
deselectPeripheral(PeripheralInterface *self,
                                   Peripheral *peripheral)
{
  PeripheralInterfaceUsartImpl *impl = (PeripheralInterfaceUsartImpl *) self;
  waitForEndOfTransmission(impl);
}
