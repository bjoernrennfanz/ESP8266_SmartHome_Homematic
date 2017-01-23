#include <stdio.h>
#include <stddef.h>

#include "contiki-core.h"

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data)
{
  PROCESS_BEGIN();

  while(1) {
    PROCESS_WAIT_EVENT();
    printf("Got event number %d\n", ev);
  }

  PROCESS_END();
}
