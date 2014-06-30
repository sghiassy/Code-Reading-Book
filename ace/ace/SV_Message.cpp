// SV_Message.cpp
// SV_Message.cpp,v 4.2 2000/04/19 02:49:39 brunsch Exp

#include "ace/SV_Message.h"

#if !defined (__ACE_INLINE__)
#include "ace/SV_Message.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ace, SV_Message, "SV_Message.cpp,v 4.2 2000/04/19 02:49:39 brunsch Exp")

ACE_ALLOC_HOOK_DEFINE(ACE_SV_Message)

void
ACE_SV_Message::dump (void) const
{
  ACE_TRACE ("ACE_SV_Message::dump");
}

