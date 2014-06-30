/* signal.c,v 1.4 2000/07/14 05:32:51 jwh1 Exp

 * =============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/posix/signal.c
 *
 * = AUTHOR
 *    Luther Baker
 *
 * ============================================================================= */

#include "pace/signal.h"

#if (PACE_HAS_CPLUSPLUS)
extern "C" {
#endif

#if !defined (PACE_HAS_INLINE)
# include "pace/posix/signal.inl"
#endif /* ! PACE_HAS_INLINE */

#if (PACE_HAS_CPLUSPLUS)
}
#endif
