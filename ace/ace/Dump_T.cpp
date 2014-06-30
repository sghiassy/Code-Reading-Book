// Dump_T.cpp
// Dump_T.cpp,v 4.3 1998/11/18 21:52:54 levine Exp

#ifndef ACE_DUMP_T_C
#define ACE_DUMP_T_C

#include "ace/Dump_T.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

ACE_RCSID(ace, Dump_T, "Dump_T.cpp,v 4.3 1998/11/18 21:52:54 levine Exp")

template <class Concrete>
ACE_Dumpable_Adapter<Concrete>::~ACE_Dumpable_Adapter (void)
{
  ACE_TRACE ("ACE_Dumpable_Adapter<Concrete>::~ACE_Dumpable_Adapter");
}

template <class Concrete>
ACE_Dumpable_Adapter<Concrete>::ACE_Dumpable_Adapter (const Concrete *t)
  : ACE_Dumpable ((const void *) t), this_ (t)
{
  ACE_TRACE ("ACE_Dumpable_Adapter<Concrete>::ACE_Dumpable_Adapter");
}

template <class Concrete> Concrete *
ACE_Dumpable_Adapter<Concrete>::operator->() const
{
  return (Concrete *) this->this_;
}

template <class Concrete> void
ACE_Dumpable_Adapter<Concrete>::dump (void) const
{
  ACE_TRACE ("ACE_Dumpable_Adapter<Concrete>::dump");
  this->this_->dump ();
}

#endif /* ACE_DUMP_T_C */
