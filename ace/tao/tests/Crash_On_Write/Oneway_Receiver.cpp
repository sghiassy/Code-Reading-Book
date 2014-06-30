//
// Oneway_Receiver.cpp,v 1.1 2001/04/08 03:58:43 coryan Exp
//
#include "Oneway_Receiver.h"

ACE_RCSID(Crash_On_Write, Oneway_Receiver, "Oneway_Receiver.cpp,v 1.1 2001/04/08 03:58:43 coryan Exp")

Oneway_Receiver::Oneway_Receiver (void)
{
}

void
Oneway_Receiver::receive_oneway (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
}
