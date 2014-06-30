/* times.inl,v 1.4 2000/07/14 21:09:10 joeh Exp -*- C -*-

 * =============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/posix/times.inl
 *
 * = AUTHOR
 *    Luther Baker
 *
 * ============================================================================= */

#if (PACE_HAS_POSIX_MP_UOF)
PACE_INLINE
pace_clock_t
pace_times (pace_tms * buffer)
{
  return times (buffer);
}
#endif /* PACE_HAS_POSIX_MP_UOF */
