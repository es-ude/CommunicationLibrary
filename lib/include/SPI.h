#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef struct SPIMessage SPIMessage;
typedef struct SPIDevice SPIDevice;
typedef struct SPI SPI;

struct SPIMessage {
	uint8_t length;
	uint8_t* outgoing_data;
	uint8_t* incoming_data;
};

struct SPI {
	void (*transferSync) (const SPIDevice *self, 
			const SPIMessage *message,
			uint8_t *slave_select_line);
	void (*transferAsync) (const SPIDevice *self, 
			const SPIMessage *message,
			uint8_t *slave_select_line);
	void (*init) (SPIDevice *self);
};

struct SPIDevice {
	SPI *hw_interface;
	uint8_t *slave_select_line;
};


/**
 * The transfer functions write outgoing_data contained inside message
 * to the spi device and store the received data to the memory incoming_data
 * points to.
 */
static void SPI_transferSync(const SPIDevice *self, const SPIMessage *data){
	self->interface->transferSync(self, data, self->slave_select_line);
}

static void SPI_transferAsync(const SPIDevice *self, const SPIMessage *data) {
	self->interface->transferAsync(self, data, self->slave_select_line);
}


#endif /* end of include guard */
