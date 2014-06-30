// ATM_Params.cpp,v 4.4 2000/04/19 02:49:33 brunsch Exp

#include "ace/ATM_Params.h"

ACE_RCSID(ace, ATM_Params, "ATM_Params.cpp,v 4.4 2000/04/19 02:49:33 brunsch Exp")

#if defined (ACE_HAS_ATM)

#if !defined (__ACE_INLINE__)
#include "ace/ATM_Params.i"
#endif /* __ACE_INLINE__ */

ACE_ALLOC_HOOK_DEFINE(ACE_ATM_Params)

void
ACE_ATM_Params::dump (void) const
{
  ACE_TRACE ("ACE_ATM_Params::dump");
}

#endif /* ACE_HAS_ATM */

