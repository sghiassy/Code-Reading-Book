/* utsname.inl,v 1.9 2001/04/21 22:43:10 coryan Exp -*- C -*-

 * =============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/win32/utsname.inl
 *
 * = AUTHOR
 *    Luther Baker
 *
 * ============================================================================= */

#include "pace/string.h"

#if (PACE_HAS_POSIX_SP_UOF)
PACE_INLINE
int
pace_uname (pace_utsname * name)
{
  PACE_UNUSED_ARG (name);
  PACE_ERRNO_NO_SUPPORT_RETURN (-1);
}
#endif /* PACE_HAS_POSIX_SP_UOF */
