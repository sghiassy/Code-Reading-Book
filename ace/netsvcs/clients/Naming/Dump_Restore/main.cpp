// main.cpp,v 4.5 2000/05/29 00:31:52 othman Exp

// Test the client-side of the ACE Name Server...

#include "ace/Service_Config.h"
#include "ace/Log_Msg.h"
#include "Dump_Restore.h"

ACE_RCSID(Dump_Restore, main, "main.cpp,v 4.5 2000/05/29 00:31:52 othman Exp")

int 
main (int argc, char *argv[])
{
  ACE_Service_Config daemon (argv[0]);

  ACE_DEBUG ((LM_DEBUG, "entering main\n"));

  // Get a handler
  Dump_Restore client_handler (argc, argv);

  ACE_Reactor::run_event_loop ();
  
  /* NOTREACHED */
  ACE_DEBUG ((LM_DEBUG, "leaving main\n"));
  return 0;
}
