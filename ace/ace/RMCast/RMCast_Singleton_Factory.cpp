// RMCast_Singleton_Factory.cpp,v 1.1 2000/10/10 00:06:30 coryan Exp

#include "RMCast_Singleton_Factory.h"

#if !defined (__ACE_INLINE__)
# include "RMCast_Singleton_Factory.i"
#endif /* ! __ACE_INLINE__ */

ACE_RCSID(ace, RMCast_Singleton_Factory, "RMCast_Singleton_Factory.cpp,v 1.1 2000/10/10 00:06:30 coryan Exp")

ACE_RMCast_Singleton_Factory::~ACE_RMCast_Singleton_Factory (void)
{
}

ACE_RMCast_Module*
ACE_RMCast_Singleton_Factory::create (void)
{
  return this->singleton_;
}

void
ACE_RMCast_Singleton_Factory::destroy (ACE_RMCast_Module *)
{
}
