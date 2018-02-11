#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef struct SPIMessage SPIMessage;
typedef struct SPISlave SPISlave;
typedef struct SPI SPI;

/*
 * When creating a new SPIMessage, the memory
 * areas provided through outgoing_data and incoming_data,
 * must have enough space for "length" number of bytes.
 * E.g.:
 * ```
 * uint8_t out[i];
 * uint8_t int[j];
 * SPIMessage m = { .length = minimum(i,j),
 *                  .outgoing_data = out,
 *                  .incoming_data = in };
 * ```
 *
 */
struct SPIMessage {
	uint8_t length;
	uint8_t* outgoing_data;
	uint8_t* incoming_data;
};

/*
 * The representation of a serial peripheral interface
 * provided by the host platform.
 */
struct SPI {
	void (*transferSync)(const SPI *self,
                       const SPIMessage *message,
                       volatile uint8_t *slave_select_line);
	void (*transferAsync)(const SPI *self,
                        const SPIMessage *message,
                        volatile uint8_t *slave_select_line);
	void (*init)(SPI *self);
	void (*destroy) (SPI *self);
};

/*
 * An SPISlave represents an external device that you
 * want to communicate with using an spi. Typically you would want to create
 * one SPISlave instance for every external device.
 *
 */
struct SPISlave {
	SPI *hw_interface;
	volatile uint8_t *slave_select_line;
};


/**
 * The transfer functions write outgoing_data contained inside message
 * to the spi device and store the received data to the memory incoming_data
 * points to. Both functions might throw a SPI_BUSY_EXCEPTION.
 */
static void SPI_transferSync(const SPISlave *self, const SPIMessage *data){
	self->hw_interface->transferSync(self->hw_interface,
                                   data,
                                   self->slave_select_line);
}

static void SPI_transferAsync(const SPISlave *self, const SPIMessage *data) {
	self->hw_interface->transferAsync(self->hw_interface,
                                    data,
                                    self->slave_select_line);
}

#endif /* end of include guard */
