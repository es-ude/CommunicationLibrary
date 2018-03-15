#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "lib/include/DEPRACATED/SPIMessage.h"
#include "lib/include/Peripheral/PeripheralInterface.h"

/**
 * DEPRACATED
 */



typedef struct SPISlave SPISlave;
typedef struct SPIMaster SPIMaster;

/*
 * The representation of a serial peripheral interface
 * provided by the host platform.
 */
struct SPIMaster {
    /**
     * Initialize the SPIMaster
     * @param self
     */
    void (*init)(SPIMaster *self);
    void (*transferSync)(const SPISlave *self, const SPIMessage *message);
	void (*transferAsync)(const SPISlave *self, const SPIMessage *message);
	void (*transferAsyncWithCompletionCallback) (const SPISlave *self,
                                               const SPIMessage *message,
                                               void (*callback) (void));
	void (*destroy) (PeripheralInterface *self);
    void (*setInterruptHandler)();
};

/*
 * An SPISlave represents an external device that you
 * want to communicate with using an peripheralInterface. Typically you would want to create
 * one SPISlave instance for every external device. Set completion_callback
 * either to NULL, or a function that shall be called as soon as all bytes
 * have been transferred. The callback has to be non blocking and should
 * have fast execution time since it'peripheral going to be executed from inside
 * an interrupt service routine.
 *
 */
struct SPISlave {
	SPIMaster *messageModule;
    void (*completion_callback) (void);
    volatile uint8_t *slave_select_register;
    uint8_t slave_select_pin;
};


/**
 * The transfer functions write outgoing_data contained inside message
 * to the spi device and store the received data to the memory incoming_data
 * points to. Both functions might throw a SPI_BUSY_EXCEPTION.
 */

static void SPI_transferSync(const SPISlave *self, const SPIMessage *data){
  self->messageModule->transferSync(self, data);
}

static void SPI_transferAsync(const SPISlave *self, const SPIMessage *data) {
  if (self->completion_callback == 0) {
    self->messageModule->transferAsync(self, data);
  }
  else {
    self->messageModule->transferAsyncWithCompletionCallback(self, data,
                                                            self->completion_callback);
  }
}



#endif /* end of include guard */
