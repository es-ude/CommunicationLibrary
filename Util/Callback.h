#ifndef COMMUNICATIONMODULE_CALLBACK_H
#define COMMUNICATIONMODULE_CALLBACK_H

/*! Contains the function and argument needed to do a callback
 *
 * This struct is used throughout the library for representing callbacks.
 */
typedef struct GenericCallback {
  void (*function) (void *);
  void *argument;
} GenericCallback;

#endif //COMMUNICATIONMODULE_CALLBACK_H
