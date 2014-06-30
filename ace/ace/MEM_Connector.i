/* -*- C++ -*- */
// MEM_Connector.i,v 4.2 2001/04/03 05:43:08 nanbor Exp

// MEM_Connector.i

// Establish a connection.

ASYS_INLINE ACE_MEM_IO::Signal_Strategy
ACE_MEM_Connector::preferred_strategy (void) const
{
  return this->preferred_strategy_;
}

ASYS_INLINE void
ACE_MEM_Connector::preferred_strategy (ACE_MEM_IO::Signal_Strategy strategy)
{
  this->preferred_strategy_ = strategy;
}

ASYS_INLINE ACE_MEM_SAP::MALLOC_OPTIONS &
ACE_MEM_Connector::malloc_options (void)
{
  return this->malloc_options_;
}
