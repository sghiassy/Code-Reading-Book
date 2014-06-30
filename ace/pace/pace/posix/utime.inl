/* utime.inl,v 1.4 2000/07/14 21:09:10 joeh Exp -*- C -*-

 * =============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/posix/utime.inl
 *
 * = AUTHOR
 *    Luther Baker
 *
 * ============================================================================= */

#if (PACE_HAS_POSIX_FS_UOF)
PACE_INLINE
int
pace_utime (const char * path,
            const pace_utimbuf * times)
{
  return utime (path, times);
}
#endif /* PACE_HAS_POSIX_FS_UOF */
