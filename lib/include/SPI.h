/*
 * SPI.h
 *
 *  Created on: 24.01.2018
 *      Author: peterzdankin
 */

#ifndef SPI_H_
#define SPI_H_

#include<stdint.h>

typedef volatile uint8_t *PortRegister;
typedef volatile uint8_t *DDRRegister;


typedef struct SPIConfig
{
    PortRegister port;
    DDRRegister ddr;
    DDRRegister chip_select_ddr;
    PortRegister chip_select_port;
} SPIConfig;

void spi_init(void);
uint8_t spi_transfer(uint8_t data);


#endif /* SPI_H_ */
