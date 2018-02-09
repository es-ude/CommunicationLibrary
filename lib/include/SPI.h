#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef struct SPIMessage SPIMessage;
typedef struct SPIDevice SPIDevice;

/**
 * The transfer functions write outgoing_data contained inside message
 * to the spi device and store the received data to the memory incoming_data
 * points to.
 */
static void SPI_transferSync(const SPIDevice *self, const SPIMessage *data){
	self->transferSync(self, data);
}

static void SPI_transferAsync(const SPIDevice *self, const SPIMessage *data) {
	self->transferAsync(self, data);
}

struct SPIMessage {
	uint8_t length;
	uint8_t* outgoing_data;
	uint8_t* incoming_data;
};

struct SPIDevice {
	void (*transferSync) (const SPI *self, const SPIMessage *message);
	void (*transferAsync) (const SPI *self, const SPIMessage *message)
};

#endif /* end of include guard */
