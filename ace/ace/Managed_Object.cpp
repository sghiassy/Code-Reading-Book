// Managed_Object.cpp,v 4.12 2000/04/19 02:49:34 brunsch Exp

#ifndef ACE_MANAGED_OBJECT_CPP
#define ACE_MANAGED_OBJECT_CPP

#include "ace/Managed_Object.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (__ACE_INLINE__)
#include "ace/Managed_Object.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ace, Managed_Object, "Managed_Object.cpp,v 4.12 2000/04/19 02:49:34 brunsch Exp")

template <class TYPE>
ACE_Cleanup_Adapter<TYPE>::~ACE_Cleanup_Adapter (void)
{
}

#endif /* ACE_MANAGED_OBJECT_CPP */
