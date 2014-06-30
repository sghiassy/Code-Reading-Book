//client.cpp,v 1.1 1999/05/06 18:24:28 coryan Exp

# include "Client_i.h"

// The client program for the application.

int
main (int argc, char **argv)
{
  Checkpoint_Client_i client;


  ACE_DEBUG ((LM_DEBUG,
              "\nEvent_Types client\n\n"));

  if (client.run ("Event_Types_Checkpoint",
                  argc,
                  argv) == -1)
    return -1;
  else
    return 0;

}
