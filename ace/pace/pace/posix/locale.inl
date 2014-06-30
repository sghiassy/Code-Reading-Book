/* locale.inl,v 1.4 2000/07/14 21:09:07 joeh Exp -*- C -*-

 * ============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/posix/locale.inl
 *
 * = AUTHOR
 *    Joe Hoffert
 *
 * =========================================================================== */

#include <locale.h>

#if (PACE_HAS_POSIX_NONUOF_FUNCS)
PACE_INLINE
pace_lconv *
pace_localeconv (void)
{
  return localeconv ();
}
#endif /* PACE_HAS_POSIX_NONUOF_FUNCS */

#if (PACE_HAS_POSIX_MP_UOF)
PACE_INLINE
char *
pace_setlocale (int category, const char * locale)
{
  return setlocale (category, locale);
}
#endif /* PACE_HAS_POSIX_MP_UOF */
