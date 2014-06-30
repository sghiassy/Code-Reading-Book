/* -*- C++ -*- */
// UPIPE_Acceptor.i,v 4.2 1998/05/25 18:15:51 irfan Exp

// UPIPE_Acceptor.i

ASYS_INLINE int
ACE_UPIPE_Acceptor::remove (void)
{
  ACE_TRACE ("ACE_UPIPE_Acceptor::remove");
  return this->ACE_SPIPE_Acceptor::remove ();
}
