/* limits.h,v 1.4 2000/06/20 20:18:13 luther Exp

 * ============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/limits.h
 *
 * = AUTHOR
 *    Joe Hoffert
 *
 * ============================================================================ */

#ifndef PACE_LIMITS_H_INDIRECT
#define PACE_LIMITS_H_INDIRECT

#include "pace/config/defines.h"

#if defined (PACE_HAS_POSIX)
# include "pace/posix/limits.h"
#elif defined (PACE_VXWORKS)
# include "pace/vxworks/limits.h"
#elif defined (PACE_WIN32)
# include "pace/win32/limits.h"
#endif

#endif /* PACE_LIMITS_H_INDIRECT */
