// -*- C++ -*-
//
// ORB_Table.inl,v 1.2 2001/02/02 02:32:03 fhunleth Exp

ACE_INLINE TAO_ORB_Table *
TAO_ORB_Table::instance (void)
{
  return TAO_Singleton<TAO_ORB_Table, TAO_SYNCH_MUTEX>::instance ();
}

ACE_INLINE TAO_ORB_Core *
TAO_ORB_Table::first_orb (void)
{
  return this->first_orb_;
}
