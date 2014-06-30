// -*- C++ -*-
//
// DLL_ORB.inl,v 1.4 2000/09/22 19:15:54 othman Exp


ACE_INLINE CORBA::ORB_ptr
TAO_DLL_ORB::orb (void)
{
  return CORBA::ORB::_duplicate (this->orb_.in());
}
