// Transport_Mux_Strategy.cpp,v 1.22 2001/08/01 23:39:56 coryan Exp

#include "tao/Transport_Mux_Strategy.h"
#include "tao/Reply_Dispatcher.h"

ACE_RCSID(tao, Transport_Mut_Strategy, "Transport_Mux_Strategy.cpp,v 1.22 2001/08/01 23:39:56 coryan Exp")

TAO_Transport_Mux_Strategy::TAO_Transport_Mux_Strategy (TAO_Transport *transport)
  : transport_ (transport)
{
}

TAO_Transport_Mux_Strategy::~TAO_Transport_Mux_Strategy (void)
{
}
