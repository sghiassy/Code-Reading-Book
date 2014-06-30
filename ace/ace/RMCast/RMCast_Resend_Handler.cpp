// RMCast_Resend_Handler.cpp,v 1.2 2000/10/11 00:57:08 coryan Exp

#include "RMCast_Resend_Handler.h"
#include "RMCast_Retransmission.h"

#if !defined (__ACE_INLINE__)
# include "RMCast_Resend_Handler.i"
#endif /* ! __ACE_INLINE__ */

ACE_RCSID(ace, RMCast_Resend_Handler, "RMCast_Resend_Handler.cpp,v 1.2 2000/10/11 00:57:08 coryan Exp")

ACE_RMCast_Resend_Handler::~ACE_RMCast_Resend_Handler (void)
{
}

int
ACE_RMCast_Resend_Handler::handle_timeout (const ACE_Time_Value &,
                                           const void *)
{
  (void) this->retransmission_->resend_all ();
  return 0;
}
