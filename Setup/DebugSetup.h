#ifndef COMMUNICATIONMODULE_DEBUGSETUP_H
#define COMMUNICATIONMODULE_DEBUGSETUP_H

#define setUpDebugging() do \
  {\
    if (DEBUG) \
      setUpPrint();\
  }\
  while(0)

void
setUpPrint();

#endif //COMMUNICATIONMODULE_DEBUGSETUP_H
