// -*- C++ -*-
// Acceptor_Registry.i,v 1.4 1999/12/12 22:00:44 othman Exp

ACE_INLINE TAO_AcceptorSetIterator
TAO_Acceptor_Registry::begin (void)
{
  return this->acceptors_;
}

ACE_INLINE TAO_AcceptorSetIterator
TAO_Acceptor_Registry::end (void)
{
  return this->acceptors_ + this->size_;
}
