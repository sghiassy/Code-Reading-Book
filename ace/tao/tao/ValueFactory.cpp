// ValueFactory.cpp,v 1.1 1999/05/06 18:24:46 coryan Exp

// ============================================================================
//
// = LIBRARY
//     TAO
//
// = FILENAME
//     ValueFactory.cpp
//
// = AUTHOR
//     Torsten Kuepper  <kuepper2@lfa.uni-wuppertal.de>
//
// ============================================================================


#include "tao/ValueFactory.h"

#ifdef TAO_HAS_VALUETYPE

#if !defined (__ACE_INLINE__)
# include "tao/ValueFactory.i"
#endif /* ! __ACE_INLINE__ */

ACE_RCSID(tao, ValueFactory, "ValueFactory.cpp,v 1.1 1999/05/06 18:24:46 coryan Exp")

CORBA_ValueFactoryBase::~CORBA_ValueFactoryBase ()
{
}

#endif /* TAO_HAS_VALUETYPE */
