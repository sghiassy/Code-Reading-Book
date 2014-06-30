// -*- C++ -*-
//
// Cleanup_Func_Registry.inl,v 1.1 2000/11/15 09:50:58 othman Exp

ACE_INLINE size_t
TAO_Cleanup_Func_Registry::size (void) const
{
  return this->cleanup_funcs_.size ();
}
