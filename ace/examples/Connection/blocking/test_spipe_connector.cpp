// test_spipe_connector.cpp,v 4.4 1998/07/31 22:55:10 gonzo Exp

// ACE_SPIPE Client.

#include "SPIPE-connector.h"

ACE_RCSID(blocking, test_spipe_connector, "test_spipe_connector.cpp,v 4.4 1998/07/31 22:55:10 gonzo Exp")

int 
main (int argc, char *argv[])                       
{                                                       
  // Perform Service_Config initializations
  ACE_Service_Config daemon (argv[0]);

  IPC_Client peer_connector;

  if (peer_connector.init (argc, argv) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "init"), -1);
  
  return peer_connector.svc ();
}                                                       
