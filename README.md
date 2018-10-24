# CommunicationModule
The CommunicationModule is a library aimed at 8bit-avr microcontrollers.
The intent is to offer software support for several 802.15.4 based network chips like [MRF24J40MA](https://www.microchip.com/wwwproducts/en/MRF24J40MA)
or the [XBee](https://www.digi.com/products/xbee-rf-solutions/2-4-ghz-modules/xbee-802-15-4#productsupport).
To allow interaction with these chips a drivers for the respective peripheral interface (USART, SPI) are included.

## Dependencies
The library comes with very few but essential dependencies.
To use the precompiled library you'll need
1. avr-gcc
2. [CException](https://github.com/ThrowTheSwitch/CException)

To build our library you'll have to install [Bazel](https://bazel.build) and
create the CROSSTOOLS file to enable usage of avr-gcc. For more information on this
see this [confluence article](http://confluence.es.uni-due.de:8090/pages/viewpage.action?pageId=23953429) .

## How to use the library
The library follows a strict separation of interfaces and implementation.
Several different implementations of each interface may be in use at the same time
(however keep in mind not to use the same physical ressource more than once).
All implementation is hidden behind abstract data types. To start using a module
you have to create the corresponding structs. To give users as much control over their memory usage as possible,
every implementation offers two functions

1. size_t InterfaceNameImplementationName_getADTSize(void);
2. InterfaceName InterfaceName_create(InterfaceName ptr_to_memory, OptionalConfigParameters parameters);

The create function usually also initializes the implementation, so that after
calling it you can start using the implementation.
For details about functions offered by the interfaces see their doxygen documentation
or take a look at their header files.

### Exceptions
Instead of passing and handling error codes in long if-else statements, we use
the CException library. However currently it is only partially used.