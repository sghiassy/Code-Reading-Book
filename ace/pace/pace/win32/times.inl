/* times.inl,v 1.7 2000/07/18 20:19:06 luther Exp -*- C -*-

 * =============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/win32/times.inl
 *
 * = AUTHOR
 *    Luther Baker
 *
 * ============================================================================= */

#if (PACE_HAS_POSIX_MP_UOF)
PACE_INLINE
pace_clock_t
pace_times (struct tms * buffer)
{
  PACE_UNUSED_ARG (buffer);
  PACE_ERRNO_NO_SUPPORT_RETURN (-1);
}
#endif /* PACE_HAS_POSIX_MP_UOF */
