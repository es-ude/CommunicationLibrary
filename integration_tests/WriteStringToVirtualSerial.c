#include "Setup/DebugSetup.h"
#include "Util/Debug.h"
#include <util/delay.h>

int main(void) {
  setUpDebugging();

  int a = 0;
  char hello[] = "Ahello world\n";

  for (;;)
  {

    if (a ==0){
      hello[0]++;
      debug(String, hello);
      if (hello[0] == 'Z'){
        hello[0] = 'A'-1;
      }

    }

  }
}