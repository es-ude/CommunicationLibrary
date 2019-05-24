#include <stdint.h>
#include "Setup/HardwareSetup.h"
#include <util/delay.h>

void delay_microseconds(uint16_t microseconds) {
    while (microseconds > 0){
        _delay_us(1);
        microseconds--;
    }
}

