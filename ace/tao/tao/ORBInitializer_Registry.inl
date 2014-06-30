// -*- C++ -*-
//
// ORBInitializer_Registry.inl,v 1.2 2001/02/02 02:32:02 fhunleth Exp

ACE_INLINE TAO_ORBInitializer_Registry *
TAO_ORBInitializer_Registry::instance (void)
{
  return
    TAO_Singleton<TAO_ORBInitializer_Registry, TAO_SYNCH_MUTEX>::instance ();
}
