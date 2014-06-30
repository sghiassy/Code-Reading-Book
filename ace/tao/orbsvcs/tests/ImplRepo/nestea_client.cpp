// nestea_client.cpp,v 1.2 1999/04/21 01:18:35 brunsch Exp

#include "nestea_client_i.h"

ACE_RCSID(ImplRepo, nestea_client, "nestea_client.cpp,v 1.2 1999/04/21 01:18:35 brunsch Exp")

// This function runs the test.

int
main (int argc, char **argv)
{
  Nestea_Client_i client;

  ACE_DEBUG ((LM_DEBUG, "\n\tNestea Bookshelf Client\n\n"));

  if (client.init (argc, argv) == -1)
    return 1;
  else
    return client.run ();
}

