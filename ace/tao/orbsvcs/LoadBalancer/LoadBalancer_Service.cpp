// -*- C++ -*-

// LoadBalancer_Service.cpp,v 1.2 2000/06/08 22:14:04 othman Exp

#include "LoadBalancer.h"

ACE_RCSID(LoadBalancer, LoadBalancer_Service, "LoadBalancer_Service.cpp,v 1.2 2000/06/08 22:14:04 othman Exp")

int
main (int argc, char *argv[])
{
  // @@ Ossama: i'm not sure what the LoadBalancer class buys you.
  ACE_DECLARE_NEW_CORBA_ENV;
  ACE_TRY
    {
      TAO_LoadBalancer balancer;

      balancer.init (argc, argv, ACE_TRY_ENV);
      ACE_TRY_CHECK;

      balancer.run (ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "TAO Load Balancer");
      return -1;
    }
  ACE_ENDTRY;


  return 0;
}
