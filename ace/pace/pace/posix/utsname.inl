/* utsname.inl,v 1.4 2000/07/14 21:09:10 joeh Exp -*- C -*-

 * =============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/posix/utsname.inl
 *
 * = AUTHOR
 *    Luther Baker
 *
 * ============================================================================= */

#if (PACE_HAS_POSIX_SP_UOF)
PACE_INLINE
int
pace_uname (pace_utsname * name)
{
  return uname (name);
}
#endif /* PACE_HAS_POSIX_SP_UOF */
