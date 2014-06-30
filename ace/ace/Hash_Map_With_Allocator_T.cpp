// Hash_Map_With_Allocator_T.cpp
// Hash_Map_With_Allocator_T.cpp,v 4.1 1999/06/26 19:54:04 marina Exp

#ifndef ACE_HASH_MAP_WITH_ALLOCATOR_T_CPP
#define ACE_HASH_MAP_WITH_ALLOCATOR_T_CPP

#include "ace/Hash_Map_With_Allocator_T.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (__ACE_INLINE__)
#include "ace/Hash_Map_With_Allocator_T.i"
#endif /* __ACE_INLINE__ */

template <class EXT_ID, class INT_ID>
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::ACE_Hash_Map_With_Allocator (ACE_Allocator *alloc)
  : ACE_Hash_Map_Manager<EXT_ID, INT_ID, ACE_Null_Mutex> (alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::ACE_Hash_Map_With_Allocator");
}

template <class EXT_ID, class INT_ID>
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::ACE_Hash_Map_With_Allocator (size_t size,
                                                                          ACE_Allocator *alloc)
  : ACE_Hash_Map_Manager<EXT_ID, INT_ID, ACE_Null_Mutex> (size, alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::ACE_Hash_Map_With_Allocator");
}

#endif /* ACE_HASH_MAP_WITH_ALLOCATOR_T_CPP */
