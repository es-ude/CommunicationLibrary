#ifndef COMMUNICATIONMODULE_MRF_H
#define COMMUNICATIONMODULE_MRF_H

#include "src/Mac802154/Mac802154.h"
#include "include/RuntimeLibraryInterface.h"
#include "src/Mac802154/MRF/MrfIo.h"
#include "src/Mac802154/MRF/MRFState.h"


typedef struct Mrf Mrf;
struct Mrf {
  struct Mac802154 mac;
  MrfIo io;
  void (*deallocate)(void *);
  void (*delay_microseconds)(double);
  MrfState state;
};


#endif //COMMUNICATIONMODULE_MRF_H
