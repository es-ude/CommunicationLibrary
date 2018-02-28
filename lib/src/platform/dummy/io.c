#include <stdint.h>
#include "lib/include/platform/io.h"

uint8_t spi_control_register = 0;
uint8_t * const spi_control_register_ptr = &spi_control_register;
