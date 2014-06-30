/* setjmp.c,v 1.1 2001/06/01 20:56:51 joeh Exp

 * =============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/vxworks/setjmp.c
 *
 * = AUTHOR
 *    Joe Hoffert
 *
 * ============================================================================= */

#include "pace/setjmp.h"

#if !defined (PACE_HAS_INLINE)
# include "pace/vxworks/setjmp.inl"
#endif /* ! PACE_HAS_INLINE */
