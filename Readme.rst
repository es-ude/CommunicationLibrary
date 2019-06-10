=====================
CommunicationModule
=====================

.. image:: https://readthedocs.org/projects/communicationlibrary/badge/?version=latest
    :target: https://communicationlibrary.readthedocs.io/en/latest/?badge=latest
    :alt: Documentation Status

The CommunicationModule is a library aimed at 8bit-avr microcontrollers.
The intent is to offer software support for several 802.15.4 based network chips like MRF24J40MA_
or the XBee_. 
To allow interaction with these chips a drivers for the respective peripheral interface (USART, SPI) are included.

.. _MRF24J40MA: https://www.microchip.com/wwwproducts/en/MRF24J40MA
.. _XBee: https://docs.bazel.build/versions/0.26.0/install.html

Dependencies
------------
The library comes with very few but essential dependencies.
To use the precompiled library you'll need

1. avr-gcc
2. CException_
3. Mutex_
4. PeripheralInterface_

.. _CException: https://github.com/ThrowTheSwitch/CException
.. _Mutex: https://github.com/es-ude/EmbeddedUtil
.. _PeripheralInterface: https://github.com/es-ude/PeripheralInterface

To use our build scripts you will have to install the Bazel build tool.
You can find install instructions at https://docs.bazel.build/versions/0.26.0/install.html.

How to use the library
----------------------
The library follows a strict separation of interfaces and implementation.
Several different implementations of each interface may be in use at the same time
(however keep in mind not to use the same physical ressource more than once).
All implementation is hidden behind abstract data types. To start using a module
you have to create the corresponding structs. To give users as much control over their memory usage as possible,
every implementation offers two functions

1. ``size_t InterfaceNameImplementationName_getADTSize(void);``
2. ``InterfaceName InterfaceName_create(InterfaceName ptr_to_memory, OptionalConfigParameters parameters);``

The create function usually also initializes the implementation, so that after
calling it you can start using the implementation.
For details about functions offered by the interfaces see their doxygen documentation
or take a look at their header files.

IMPORTANT: To use the library you will have to implement the ``executeAtomically()`` function.
This is necessary to make the Mutex lib work. The corresponding header file
can be found in the EmbeddedUtilities repo under ``Util/Atomic.h``.

Predefined setups for the department's platforms
------------------------------------------------
A ready to use setup for the Motherboard and ElasticNode hardware
platforms is available. To compile these use the following command::

    bazel build integration_tests:HardwareSetup --platforms @AvrToolchain//platforms:Motherboard
 
or::
    
    bazel build integration_tests:HardwareSetup --platforms @AvrToolchain//platforms:ElasticNode

respectively.

How to use the setups in your own bazel project
-----------------------------------------------
Integrate e.g. the ``MotherboardSetup`` lib like so::

    default_embedded_binary(
        name = "MyApp",
        srcrs = ["src/MyApp.c"],
        deps =  ["@CommunicationModule//Setup:MotherboardSetup"],
    )

Exceptions
----------

Instead of passing and handling error codes in long if-else statements, we use
the CException library. However currently it is only partially used.

Known Issues
------------
* non blocking functions are in development
* enabling promiscuous mode seems to prevent back to back reception of packages

