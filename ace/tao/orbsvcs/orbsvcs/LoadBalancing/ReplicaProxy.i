// -*- C++ -*-
// ReplicaProxy.i,v 1.2 2000/06/08 22:14:05 othman Exp

ACE_INLINE
TAO_LB_ReplicaProxy::~TAO_LB_ReplicaProxy (void)
{
  // Nada
}

ACE_INLINE CORBA::Float
TAO_LB_ReplicaProxy::current_load (void) const
{
  return this->current_load_;
}

ACE_INLINE CORBA::Object_ptr
TAO_LB_ReplicaProxy::replica (void)
{
  return CORBA::Object::_duplicate (this->replica_.in ());
}
