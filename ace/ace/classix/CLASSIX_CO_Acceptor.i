/* -*- C++ -*- */
// CLASSIX_CO_Acceptor.i,v 1.2 2001/03/04 00:55:32 brunsch Exp

// Do nothing routine for constructor. 
ACE_INLINE
ACE_CLASSIX_CO_Acceptor::ACE_CLASSIX_CO_Acceptor (ACE_Reactor* theReactor)
    : port_    (0),
      reactor_ (theReactor)
{
  ACE_TRACE ("ACE_CLASSIX_CO_Acceptor::ACE_CLASSIX_CO_Acceptor");
}

ACE_INLINE
ACE_CLASSIX_CO_Acceptor::~ACE_CLASSIX_CO_Acceptor(void)
{
  delete this->port_;
}

ACE_INLINE
void
ACE_CLASSIX_CO_Acceptor::reactor(ACE_Reactor* theReactor)
{
  this->reactor_ = theReactor;
}

ACE_INLINE
ACE_HANDLE
ACE_CLASSIX_CO_Acceptor::get_handle(void) const
{
  return this->addr_.get_handle();
}
