// Logging_Test.cpp,v 1.1 1999/06/24 22:56:10 mjb2 Exp

#include "Logging_Test_i.h"

ACE_RCSID(Logger, Logging_Test, "Logging_Test.cpp,v 1.1 1999/06/24 22:56:10 mjb2 Exp")

// This function runs the Logging service test.

int
main (int argc, char **argv)
{
  Logger_Client logger_client;

  ACE_DEBUG ((LM_DEBUG,
              "\n \t IDL_Logger: client \n\n"));

  if (logger_client.init (argc, argv) == -1)
    return 1;

  return logger_client.run ();

}
