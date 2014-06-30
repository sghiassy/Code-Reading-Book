// Reply_Dispatcher.cpp,v 1.37 2000/11/13 20:38:58 parsons Exp

#include "tao/Reply_Dispatcher.h"

ACE_RCSID(tao, Reply_Dispatcher, "Reply_Dispatcher.cpp,v 1.37 2000/11/13 20:38:58 parsons Exp")

#if !defined (__ACE_INLINE__)
#include "tao/Reply_Dispatcher.i"
#endif /* __ACE_INLINE__ */

// Constructor.
TAO_Reply_Dispatcher::TAO_Reply_Dispatcher (void)
  : reply_status_ (100) // Just an invalid reply status.
{
}

// Destructor.
TAO_Reply_Dispatcher::~TAO_Reply_Dispatcher (void)
{
}

