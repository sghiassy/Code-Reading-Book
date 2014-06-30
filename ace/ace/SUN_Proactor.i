/* -*- C++ -*- */
// SUN_Proactor.i,v 4.1 2000/07/01 21:21:56 schmidt Exp

ACE_INLINE 
ACE_POSIX_Proactor::Proactor_Type ACE_SUN_Proactor::get_impl_type (void)
{
  return PROACTOR_SUN;
} 
