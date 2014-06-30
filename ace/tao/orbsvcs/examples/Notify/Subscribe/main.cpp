// -*- C++ -*-
// main.cpp,v 1.1 2000/03/05 01:38:23 pradeep Exp

#include "Subscribe.h"

int
main (int argc, char *argv [])
{
  Subscribe client;

  ACE_TRY_NEW_ENV
    {
      client.init (argc, argv,
                   ACE_TRY_ENV); //Init the Client
      ACE_TRY_CHECK;

      client.run (ACE_TRY_ENV);
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
                           "Filter system error: ");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
