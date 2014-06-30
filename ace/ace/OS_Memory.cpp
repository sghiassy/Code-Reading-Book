// OS_Memory.cpp,v 4.1 2000/06/04 04:27:41 brunsch Exp

#include "ace/OS_Memory.h"

ACE_RCSID(ace, OS_Memory, "OS_Memory.cpp,v 4.1 2000/06/04 04:27:41 brunsch Exp")

#if !defined (ACE_HAS_INLINED_OSCALLS)
# include "ace/OS_Memory.inl"
#else /* !ACE_HAS_INLINED_OSCALLS */
#if defined (__hpux) && !defined (__GNUG__)
static int shut_up_aCC = 0;
#endif /* HPUX && !g++ */
#endif /* !ACE_HAS_INLINED_OS_CALLS */
