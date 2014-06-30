/* -*- C++ -*- */
// Dynamic_Service.i,v 4.2 2000/11/19 18:42:08 coryan Exp

template <class TYPE> ACE_INLINE TYPE *
ACE_Dynamic_Service<TYPE>::instance (const ACE_TCHAR *name)
{
  return  ACE_reinterpret_cast(TYPE*,ACE_Dynamic_Service_Base::instance (name));
}
