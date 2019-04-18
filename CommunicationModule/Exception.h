#ifndef COMMUNICATIONMODULE_EXCEPTION_H
#define COMMUNICATIONMODULE_EXCEPTION_H

#include "CException.h"

enum ExceptionValues {
  NO_EXCEPTION = 0x00,
  PERIPHERAL_INTERFACE_BUSY_EXCEPTION,
  NETWORK_HARDWARE_IS_BUSY_EXCEPTION,
  PERIPHERAL_SELECT_EXCEPTION,
  PERIPHERAL_INTERFACE_DESELECTED_WRONG_PERIPHERAL_EXCEPTION,
  PERIPHERAL_CONFIGURATION_EXCEPTION,
};

#endif //COMMUNICATIONMODULE_EXCEPTION_H
