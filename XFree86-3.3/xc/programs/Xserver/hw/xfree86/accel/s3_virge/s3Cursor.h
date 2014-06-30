/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3Cursor.h,v 3.2 1996/12/27 07:02:19 dawes Exp $ */





/* $XConsortium: s3Cursor.h /main/1 1996/10/24 07:12:19 kaleb $ */


extern Bool s3BlockCursor;
extern Bool s3ReloadCursor;

#define BLOCK_CURSOR	s3BlockCursor = TRUE;

/* BL 0815150096:
 * UNBLOCK_CURSOR is called at the end of every graphics function, write_mem_barrier()
 * guarantees update of regs and memory (on e.g. Alpha)
 */
#define UNBLOCK_CURSOR	{  write_mem_barrier(); \
			   WaitIdleEmpty(); \
			   if (s3ReloadCursor) \
			      s3RestoreCursor(s3savepScreen); \
			   s3BlockCursor = FALSE; \
			}
