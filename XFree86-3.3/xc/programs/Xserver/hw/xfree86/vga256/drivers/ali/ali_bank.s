/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/ali/ali_bank.s,v 3.3 1996/12/23 06:55:51 dawes Exp $ */
/*
 * These are here the very lowlevel VGA bankswitching routines.
 * The segment to switch to is passed via %eax. Only %eax and %edx my be used
 * without saving the original contents.
 *
 * WHY ASSEMBLY LANGUAGE ???
 *
 * These routines must be callable by other assembly routines. But I don't
 * want to have the overhead of pushing and poping the normal stack-frame.
 */
/* $XConsortium: ali_bank.s /main/3 1996/02/21 18:01:43 kaleb $ */

/*
 * first we have here a mirror for the segment register. That's because a
 * I/O read costs so much more time, that is better to keep the value of it
 * in memory.
 */

#include "assyntax.h"

        FILE("ali_bank.s")

        AS_BEGIN
        SEG_DATA
Segment:
        D_BYTE 0

        SEG_TEXT

        ALIGNTEXT4
        GLOBL   GLNAME(ALISetRead)
GLNAME(ALISetRead):
/*      MOV_B   (AL,CONTENT(Segment)) */
        MOV_L   (CONST(0x3D6),EDX)
        OUT_B
        RET

        ALIGNTEXT4
        GLOBL   GLNAME(ALISetWrite)
GLNAME(ALISetWrite):
/*      MOV_B   (AL,CONTENT(Segment)) */
        MOV_L   (CONST(0x3D7),EDX)
        OUT_B
        RET

        ALIGNTEXT4
        GLOBL   GLNAME(ALISetReadWrite)
GLNAME(ALISetReadWrite):
        MOV_B   (AL,AH)
/*      MOV_B   (AL,CONTENT(Segment)) */
        MOV_L   (CONST(0x3D6),EDX)
        OUT_W
        RET

