#ifndef COMMUNICATIONMODULE_ATOMIC_H
#define COMMUNICATIONMODULE_ATOMIC_H

#include "Util/Callback.h"

/**
 * This function has to be implemented
 * by the user of the library.
 * The function provided as parameter
 * is assumed to be executed without
 * being interrupted. Enabling and
 * disabling interrupts usually depends
 * on the platform the application runs on.
 * To avoid introducing this dependency into
 * the communication module we rely on the
 * user providing this function during
 * link time. For most atmega
 * platforms an implementation
 * like the following should be sufficient:
 *
 * ```c
 * #include <util/atomic.h>
 *
 * void
 * executeAtomically(CommunicationModule_Callback callback)
 * {
 *   ATOMIC_BLOCK(ATOMIC_STATERESTORE)
 *   {
 *     callback.function(callback.argument);
 *   }
 * }
 *
 */
void
executeAtomically(GenericCallback callback);

#endif //COMMUNICATIONMODULE_ATOMIC_H
