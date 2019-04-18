#ifndef COMMUNICATIONMODULE_CALLBACK_H
#define COMMUNICATIONMODULE_CALLBACK_H

/*! Contains the function and argument needed to do a callback
 *
 * This struct is used throughout the library for representing callbacks.
 */
typedef struct CommunicationModule_Callback {
  void (*function) (void *);
  void *argument;
} CommunicationModule_Callback;

#endif //COMMUNICATIONMODULE_CALLBACK_H
