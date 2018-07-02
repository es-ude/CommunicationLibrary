#include "Helpers.h"

/*
 * We use the LUFA library to send debugging messages
 * to a virtual serial port over usb (CDC-ACM).
 * This is taken from the VirtualSerial demo
 * that ships with the LUFA library.
 * You can find it under
 * LUFA/Demos/Device/ClassDriver/VirtualSerial/VirtualSerial.c
 */

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
        {
                .Config =
                        {
                                .ControlInterfaceNumber         = 0,
                                .DataINEndpoint                 =
                                        {
                                                .Address                = CDC_TX_EPADDR,
                                                .Size                   = CDC_TXRX_EPSIZE,
                                                .Banks                  = 1,
                                        },
                                .DataOUTEndpoint                =
                                        {
                                                .Address                = CDC_RX_EPADDR,
                                                .Size                   = CDC_TXRX_EPSIZE,
                                                .Banks                  = 1,
                                        },
                                .NotificationEndpoint           =
                                        {
                                                .Address                = CDC_NOTIFICATION_EPADDR,
                                                .Size                   = CDC_NOTIFICATION_EPSIZE,
                                                .Banks                  = 1,
                                        },
                        },
        };

void setUpUsbSerial(void) {
  clock_prescale_set(clock_div_1);
  USB_Init();
}

void usbWriteByte(uint8_t byte) {
  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, byte);
}

void usbWriteString(const uint8_t *data) {
  CDC_Device_SendString(&VirtualSerial_CDC_Interface, (const char* const)data);
}