
# Table of Contents

1.  [Communication Module](#org55606bb)
    1.  [Software Layers of the Communication Module](#orgd4c4682)
    2.  [UserAPI](#orgeec3702)
    3.  [PeripheralInterfaces](#orgdb9bb52)
        1.  [Example of an SPI Interface](#org408041e)



<a id="org55606bb"></a>

# Communication Module


<a id="orgd4c4682"></a>

## Software Layers of the Communication Module

I propose the following Layers:

-   **UserAPI** (send, receive, initialize necessary data structures)
-   **NetworkHardware** (MRF vs XBee vs BluetoothAdapter)
-   **Peripheral** (Usart vs SPI)

For now we expose the interfaces using abstract datatypes.
This allows us to build a "configuration tree" any way
we need depending on the used hardware, which also enables us to use
multiple hardware setups in parallel.

The Tree could be created like so:

    Peripheral *peripheral = initPeripheralInterfaceSPI(&p_interface);
    
    NetworkHardware *hardware = NetworkHardware_createMRF(peripheral);
    
    UserAPI comm;
    initUserAPI(&comm, hardware);
    
    runYourUserCode(&comm);


<a id="orgeec3702"></a>

## UserAPI

After the above intialization using the library should then be as easy as:

    // sending a message
    Message message;
    message.destination.address._16bit = 42;
    message.payload = "this is payload";
    message.payload_length = strlen(message.payload);
    comm->sendMessage(&message); // runs non-blocking
    
    // receive a message
    Message message;
    uint8_t buffer[256];
    message.payload = buffer;
    message.payload_length = 256;
    comm->receiveMessage(&message); // runs non-blocking

Isn't everything else (e.g. setting a pan id) initialization and would 
go into the init functions as well?


<a id="orgdb9bb52"></a>

### Example of an SPI Interface

For setup of the SPI Interface let's do something like this

    typedef struct SPIConfig
    {
        PortRegister port;
        DDRRegister ddr;
        DDRRegister chip_select_ddr;
        PortRegister chip_Select_port;
    } SPIConfig;

Then for initialization of SPI do something like

    SPIConfig spi_conf;
    spi_conf.port = PORTD;
    spi_conf.ddr = DDRD;
    spi_conf.chip_select_ddr = DDRB;
    spi_conf.chip_select_port = PORTB;
    Peripheral spi_interface;
    Peripheral_createSPI(&spi_interface, &spi_conf);

