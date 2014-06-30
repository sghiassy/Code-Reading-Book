//
// RMCast_Resend_Worker.cpp,v 1.1 2000/10/11 00:57:08 coryan Exp
//

#include "RMCast_Resend_Worker.h"
#include "RMCast_Module.h"

#if !defined (__ACE_INLINE__)
# include "RMCast_Resend_Worker.i"
#endif /* ! __ACE_INLINE__ */

ACE_RCSID(ace, RMCast_Resend_Worker, "RMCast_Resend_Worker.cpp,v 1.1 2000/10/11 00:57:08 coryan Exp")

int
ACE_RMCast_Resend_Worker::work (ACE_UINT32 const & key,
                                ACE_RMCast::Data const &item)
{
  if (key > this->max_sequence_number_)
    return 0;
  //ACE_DEBUG ((LM_DEBUG,
  //            "RMCast_Resend_Worker::work - message %d resent\n",
  //            key));

  ACE_RMCast::Data data = item;
  int r = this->next_->data (data);
  if (r != 0)
    return r;
  this->n++;

  return 1; // @@ Stop after the first message...
}
