// MCast_Server_i.cpp,v 1.2 2001/09/20 18:25:27 irfan Exp

#include "MCast_Server_i.h"

MCast_Server_i::MCast_Server_i (void)
{
  //Constructor.
}

CORBA::Boolean
MCast_Server_i::connect_server (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  return 0;
}
