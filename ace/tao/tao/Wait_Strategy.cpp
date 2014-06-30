// Wait_Strategy.cpp,v 1.31 2001/08/01 23:39:56 coryan Exp

#include "tao/Wait_Strategy.h"

ACE_RCSID(tao, Wait_Strategy, "Wait_Strategy.cpp,v 1.31 2001/08/01 23:39:56 coryan Exp")

#if !defined (__ACE_INLINE__)
# include "Wait_Strategy.inl"
#endif /* __ACE_INLINE__ */

// Constructor.
TAO_Wait_Strategy::TAO_Wait_Strategy (TAO_Transport *transport)
  : transport_ (transport),
    is_registered_ (0)

{
}

// Destructor.
TAO_Wait_Strategy::~TAO_Wait_Strategy (void)
{
}

int
TAO_Wait_Strategy::sending_request (TAO_ORB_Core * /* orb_core */,
                                    int            /* two_way */)
{
  return 0;
}
