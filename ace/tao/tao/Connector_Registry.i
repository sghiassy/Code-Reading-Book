// -*- C++ -*-
// Connector_Registry.i,v 1.2 2000/03/16 01:24:37 bala Exp

ACE_INLINE TAO_ConnectorSetIterator
TAO_Connector_Registry::begin (void)
{
  return this->connectors_;
}

ACE_INLINE TAO_ConnectorSetIterator
TAO_Connector_Registry::end (void)
{
  return this->connectors_ + this->size_;
}
