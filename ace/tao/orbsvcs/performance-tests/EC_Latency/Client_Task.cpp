/**
 * @file Client_Task.cpp
 *
 * Client_Task.cpp,v 1.1 2001/08/24 21:46:48 coryan Exp
 *
 * @author Carlos O'Ryan <coryan@uci.edu>
 */

#include "Client_Task.h"

ACE_RCSID(EC_Latency, Client_Task, "Client_Task.cpp,v 1.1 2001/08/24 21:46:48 coryan Exp")

ECL_Client_Task::ECL_Client_Task (CORBA::ORB_ptr orb)
  :  orb_ (CORBA::ORB::_duplicate (orb))
{
}

int
ECL_Client_Task::svc (void)
{
  ACE_DECLARE_NEW_CORBA_ENV;
  ACE_TRY
    {
      this->orb_->run (ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      return -1;
    }
  ACE_ENDTRY;
  return 0;
}
