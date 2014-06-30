/* mqueue.c,v 1.4 2000/07/12 06:27:32 jwh1 Exp

 * =============================================================================
 *
 * = LIBRARY
 *    pace (posix ace)
 *
 * = FILENAME
 *    pace/posix/mqueue.c
 *
 * = AUTHOR
 *    Luther Baker
 *
 * ============================================================================= */

#include "pace/mqueue.h"

#if PACE_LINUX
# include "pace/emulation/mqueue.c"
#endif /* PACE_LINUX */

#if !defined (PACE_HAS_INLINE)
# include "pace/posix/mqueue.inl"
#endif /* ! PACE_HAS_INLINE */
