// -*- C++ -*-
// Transport_Timer.cpp,v 1.2 2001/04/24 08:02:53 coryan Exp

#include "Transport_Timer.h"
#include "Transport.h"

ACE_RCSID(tao, Transport_Timer, "Transport_Timer.cpp,v 1.2 2001/04/24 08:02:53 coryan Exp")

TAO_Transport_Timer::TAO_Transport_Timer (TAO_Transport *transport)
  : transport_ (transport)
{
}

int
TAO_Transport_Timer::handle_timeout (const ACE_Time_Value &current_time,
                                     const void *act)
{
  return this->transport_->handle_timeout (current_time, act);
}
