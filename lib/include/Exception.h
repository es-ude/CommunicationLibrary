#ifndef COMMUNICATIONMODULE_EXCEPTION_H
#define COMMUNICATIONMODULE_EXCEPTION_H

#include "CException.h"

enum ExceptionValues {
  NO_EXCEPTION = 0x00,
  PERIPHERAL_INTERFACE_BUSY_EXCEPTION,
  NETWORK_HARDWARE_IS_BUSY_EXCEPTION,

  /*
 * Thrown when the requested action would lead to an invalid header.
 * E.g. setting source and destination address to short format without previously
 * setting a pan id.
 */
          FRAMEHEADER802154_INVALID_EXCEPTION,
};

#endif //COMMUNICATIONMODULE_EXCEPTION_H
