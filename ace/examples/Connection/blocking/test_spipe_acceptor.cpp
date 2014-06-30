// test_spipe_acceptor.cpp,v 4.3 1998/07/31 22:55:10 gonzo Exp

// ACE_SPIPE Server.

#include "SPIPE-acceptor.h"

ACE_RCSID(blocking, test_spipe_acceptor, "test_spipe_acceptor.cpp,v 4.3 1998/07/31 22:55:10 gonzo Exp")

int 
main (int argc, char *argv[])
{                                                                
  // Perform Service_Config initializations
  ACE_Service_Config daemon (argv[0]);

  IPC_Server peer_acceptor;
  
  if (peer_acceptor.init (argc, argv) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "init"), -1);

  return peer_acceptor.svc ();
}

