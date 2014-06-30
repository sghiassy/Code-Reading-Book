/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000curs.h,v 3.3 1996/12/23 06:40:39 dawes Exp $ */





/* $XConsortium: p9000curs.h /main/4 1996/02/21 17:32:18 kaleb $ */

extern Bool p9000BlockCursor;
extern Bool p9000ReloadCursor;

#define BLOCK_CURSOR    p9000BlockCursor = TRUE;

#define UNBLOCK_CURSOR  { \
			    if (p9000ReloadCursor) \
			       p9000RestoreCursor(p9000savepScreen); \
			    p9000BlockCursor = FALSE; \
                        }
