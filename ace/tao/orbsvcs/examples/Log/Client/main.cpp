// -*- C++ -*-
// main.cpp,v 1.1 1999/10/10 03:15:54 pradeep Exp

#include "TLS_Client.h"

int
main (int argc, char *argv [])
{
  TLS_Client client; // Telecom Log Service Client

  ACE_TRY_NEW_ENV
    {
      client.init (argc, argv,
                   ACE_TRY_ENV); //Init the Client
      ACE_TRY_CHECK;

      client.run_tests (ACE_TRY_ENV); // Run a few tests
      ACE_TRY_CHECK;
    }
  ACE_CATCH (CORBA::UserException, ue)
    {
      ACE_PRINT_EXCEPTION (ue,
                           "TLS_Client user error: ");
      return 1;
    }
  ACE_CATCH (CORBA::SystemException, se)
    {
      ACE_PRINT_EXCEPTION (se,
                           "TLS_Client system error: ");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
