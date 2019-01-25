#include "integration_tests/src/Setup/MrfHardwareSetup.h"
#include <util/delay.h>

void delay_microseconds(uint16_t microseconds) {
    while (microseconds > 0){
        _delay_us(1);
        microseconds--;
    }
}

