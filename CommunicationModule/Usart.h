#ifndef EDGEDEVICE_USART_H
#define EDGEDEVICE_USART_H

#include "Peripheral/PeripheralInterface.h"

typedef struct UsartConfig UsartConfig;

/*
 * Setup the UsartConfig struct to match the corresponding
 * values of your avr platform. Hand it over to the
 * PeripheralInterfaceUsartImpl_createNew()
 * function. It will be copied so you are free to delete it afterwards.
 * To write to a uart output initialize a UsartPeripheral struct using the
 * corresponding enum types.
 * These represent the values currently support by the implementation.
 * Every unsupported setup will result in an exception being thrown on the call
 * to PeripheralInterface_selectPeripheral().
 * You can use PeripheralInterfaceUsartImpl_getDefaultPeripheral to retrieve
 * a default setup for a UsartPeripheral.
 */
struct UsartConfig
{
  volatile uint8_t *control_and_status_register_a;
  volatile uint8_t *control_and_status_register_b;
  volatile uint8_t *control_and_status_register_c;
  volatile uint8_t *data_register;
  volatile uint8_t *baud_rate_register_high;
  volatile uint8_t *baud_rate_register_low;
  uint32_t cpu_frequency;
};

typedef enum UsartPeripheralBaudRate
{
  USART_BAUD_RATE_2400,
  USART_BAUD_RATE_9600,
  USART_BAUD_RATE_115200,
} UsartPeripheralBaudRate;

typedef enum UsartPeripheralParity
{
  USART_NO_PARITY,
} UsartPeripheralParity;

typedef enum UsartPeripheralDataBits
{
  USART_DATA_BITS_8,
} UsartPeripheralDataBits;

typedef enum UsartPeripheralStopBits
{
  USART_STOP_BIT_1,
} UsartPeripheralStopBits;

typedef enum UsartPeripheralMode
{
  USART_MODE_ASYNCHRONOUS_NORMAL,
  USART_MODE_ASYNCHRONOUS_DOUBLE_SPEED,
  USART_MODE_SYNCHRONOUS_MASTER,
} UsartPeripheralMode;

typedef struct UsartPeripheral
{
  uint8_t baud_rate;
  uint8_t data_bits;
  uint8_t stop_bits;
  uint8_t mode;
  uint8_t parity;
} UsartPeripheral;

void
PeripheralInterfaceUsartImpl_createNew(PeripheralInterface *self,
                                       const UsartConfig *config);

size_t
PeripheralInterfaceUsartImpl_getADTSize(void);

/*
  Returns a default UsartPeripheral struct. Initialized as follows:
  ```
   UsartPeripheral peripheral = {
    .baud_rate = USART_BAUD_RATE_9600,
    .data_bits = USART_DATA_BITS_8,
    .stop_bit  = USART_STOP_BIT_1,
    .mode      = USART_MODE_ASYNCHRONOUS_NORMAL,
    .parity    = USART_NO_PARITY,
  };
  ```
  */
UsartPeripheral
PeripheralInterfaceUsartImpl_getDefaultPeripheral(void);

typedef struct PeripheralInterfaceUsartImpl PeripheralInterfaceUsartImpl;

struct PeripheralInterfaceUsartImpl
{
  PeripheralInterface interface;
  UsartConfig config;
};

#endif
