#include "Util/Atomic.h"
#include <util/atomic.h>

void
executeAtomically(GenericCallback callback)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    callback.function(callback.argument);
  }
}
