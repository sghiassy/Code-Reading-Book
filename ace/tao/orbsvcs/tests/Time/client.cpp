// client.cpp,v 1.1 1998/12/30 22:21:03 vishal Exp

#include "Client_i.h"

ACE_RCSID(Time, client, "client.cpp,v 1.1 1998/12/30 22:21:03 vishal Exp")

// This function runs the Time Service client test.

int
main (int argc, char *argv[])
{
  Client_i client;

  ACE_DEBUG ((LM_DEBUG,
              "[CLIENT] Process/Thread Id : (%P/%t) Time Service Client\n"));

  if (client.init (argc, argv) == -1)
    return -1;
  else
    return client.run ();
}
