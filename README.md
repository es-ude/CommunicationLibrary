
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
-   **Protocol** (802.15.4 vs ZigBee vs Bluetooth)
-   **HWModuleInterface** (MRF vs XBee)
-   **PeripheralInterface** (Usart vs SPI)

The goal here is to create as much data as possible on the stack by defining a
tree of needed data structures that is build up once at the start
of the program. This allows us to build that "configuration tree" any way
we need depending on the used hardware. This should enable us to use
multiple hardware setups in parallel.

The Tree could be created like so:

    PeripheralInterface p_interface;
    initPeripheralInterfaceSPI(&p_interface);
    
    HWModuleInterface hw_interface;
    initHwModuleInterfaceMRF(&hw_interface, &p_interface);
    
    Protocol protocol;
    initProtocol802154(&protocol, &hw_interface);
    
    UserAPI comm;
    initUserAPI(&comm, &protocol);
    
    runYourUserCode(&comm);

By using pointers the user stays in complete control of memory.
Second option is to allocate everything on the heap and creat all
modules as abstract data types. Though this option seems better because it
prevents data members from direct access, we might run into problems in case
heap space is artificially limited by runtime (is this actually a thing?).
Besides that we'd be screwed on systems without dynamic allocation.


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

## PeripheralInterfaces

To enable sharing of PeripheralInterfaces across different parts of an application
access to them has to be organized. Maybe do it like so:

    typedef enum {
        BUSY = 1,
        INITIALIZED = (1 << 1),
    } PeriphalInterfaceState;
    
    typedef struct PeripheralInterface {
        PeripheralInterfaceState state;
        union {
            PeripheralSPI spi;
            PeripheralUART uart;
    } PeripheralInterface;
    
    bool
    isBusy_PeripheralInterface(PeripheralInterface *interface)
    {
        // of course this has to be atomic because of calls from ISRs
        return BUSY && interface->state;
    }
    
    void
    claim_PeripheralInterface(PeripheralInterface *interface)
    {
        // atomic as well
        interface->state |= BUSY;
    }

One could of course also implement a Mutex mechanism for the Peripheral e.g. like so

    typedef struct Mutex {
        void *key;
    } Mutex;
    
    void
    lockMutexWithKey(Mutex *mutex, void *key)
    {
        // atomically
        mutex->key = key;
    }
    
    bool
    mutexLocked(Mutex *mutex)
    {
        //atomically
        return mutex->key != NULL;
    }
    
    bool
    unlockMutexWithKey(Mutex *mutex, void *key)
    {
        //atomically
        if(mutex->key == key)
        {
            mutex->key = NULL;
            return true;
        }
        else return false;
    }

This way it would be more difficult to accidentally use a busy interface and I don't
think it's expensive enough not to use it.


<a id="org408041e"></a>

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
    PeripheralInterface spi_interface;
    initPeripheralInterfaceSPI(&spi_interface, &spi_conf);

