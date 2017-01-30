#include <stdio.h>
#include <stddef.h>

#include <ets_sys.h>
#include <os_type.h>
#include <osapi.h>

#include "contiki-core.h"

PROCESS(first_process, "First");
PROCESS(second_process, "Second");
AUTOSTART_PROCESSES(&first_process, &second_process);

PROCESS_THREAD(first_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
	  PROCESS_WAIT_EVENT();
	  // os_printf("First: Got event number %d\n", ev);
  }

  PROCESS_END();
}


PROCESS_THREAD(second_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
	  PROCESS_WAIT_EVENT();
	  // os_printf("Second: Got event number %d\n", ev);
  }

  PROCESS_END();
}
