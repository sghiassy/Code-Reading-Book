// -*- C++ -*-
// DIOP_Acceptor.i,v 1.1 2001/07/03 23:53:23 fhunleth Exp

ACE_INLINE const ACE_INET_Addr&
TAO_DIOP_Acceptor::address (void) const
{
  ACE_ASSERT (this->addrs_ != 0);

  // @@ This is busted.
  //    The Implementation Repository will have to start supporting
  //    IORs with multiple profiles.  For now, we just return the
  //    first addr.
  //                    -Ossama
  return this->addrs_[0];
}

ACE_INLINE const ACE_INET_Addr *
TAO_DIOP_Acceptor::endpoints (void)
{
  ACE_ASSERT (this->addrs_ != 0);

  return this->addrs_;
}
