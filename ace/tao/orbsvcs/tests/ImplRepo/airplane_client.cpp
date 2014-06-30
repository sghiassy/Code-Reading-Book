// airplane_client.cpp,v 1.3 2001/02/13 17:24:34 coryan Exp

#include "airplane_client_i.h"

ACE_RCSID(ImplRepo, airplane_client, "airplane_client.cpp,v 1.3 2001/02/13 17:24:34 coryan Exp")

// This function runs the test.

int
main (int argc, char **argv)
{
  Airplane_Client_i client;

  ACE_DEBUG ((LM_DEBUG, "\n\tPaper Airplane Client\n\n"));

  if (client.init (argc, argv) == -1)
    return 1;
  else
    return client.run ();
}
