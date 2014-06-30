// persistent_client.cpp,v 1.2 1999/07/09 17:34:21 schmidt Exp

# include "Persistent_Client.h"

// The "persistent client" program for the application.

int
main (int argc, char *argv[])
{
  Persistent_Client_i client;
  

  ACE_DEBUG ((LM_DEBUG,
              "\nGrid client\n\n"));

  if (client.run ("grid", argc, argv) == -1)
    return -1; 
  else
    return 0;
}

                                 
