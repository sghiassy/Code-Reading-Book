/*
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaBankc.c,v 3.0 1996/12/09 11:54:41 dawes Exp $ */

/*
 * Because a modern VGA has more than 128kBytes (which are mappable into the
 * 386' memory some logic is required. The VGA's memory (logical VGA
 * address space) is devided into smaller parts (called logical segments). 
 * These segments are mapped to logical areas.
 *
 * There are there different logical mapping areas:
 *
 * Read:       an area which can be read from
 * Write:      an area which can be written to
 * ReadWrite:  here is both read an write possible
 *
 * It is permissable to use simultaneously a Read and a Write, but you can use
 * ReadWrite only as a single. 
 * For example the bitblitting code uses a Read area as source and a Write
 * area as destination. Most other routines use only a ReadWrite.
 *
 * A logical mapping area is described by some parameters (here I will for
 * example describe a Read area:
 *
 * ReadBottom     lowest accessable byte relative to the beginning of the
 *                VGA boards mapped memory.
 * 
 * ReadTop        highes accessable byte plus one.
 *
 * SegmentSize    size of such an mapped area (common for all three)
 *
 * SegmentShift   log2(SegmentSize) (used to compute the logical segment)
 *
 * SegmentMask    SegmentSize - 1 (used to mask the offset inter an area)
 *
 * 
 * All that the following routines are doing is computing for a given offset
 * into the logical VGA adress space the offset into such an logical area
 * and the logical segment number. By the way they call also the VGA board's
 * driver to set up the VGA's physical memory mapping according to the logical
 * that was requested by the calliie.
 *
 * For shake of simplicity Write and ReadWrite share the same Bottom & Top.
 * NOTE: Read & Write may have differnt starting addresses, or even common.
 *
 * There are multible routines present for the same effect. This was made
 * for effectivly interface lowlevel assembly language best.
 */

/*
 * BUGALERT: this should be gotten from vga.h. But since there some C lang.
 *           things, too ...
 */
#include "X.h"
#include "misc.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"

#include "vga.h"

unsigned long readseg, writeseg, saveseg;

/*
 *-----------------------------------------------------------------------
 * vgaSetReadWrite ---
 *     select a memory bank of the VGA board for read & write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
pointer
vgaSetReadWrite(p)
    register pointer p;
{
#ifdef XF86VGA16
  writeseg = ((unsigned long)p - (unsigned long)vgaBase) >> vgaSegmentShift;
  (vgaSetReadWriteFunc)(writeseg);
  return (vgaWriteBottom + (((unsigned long)p - (unsigned long)vgaBase) & vgaSegmentMask));
#else
  writeseg = ((unsigned long)p - VGABASE) >> vgaSegmentShift;
  (vgaSetReadWriteFunc)(writeseg);
  return (pointer)
    ((unsigned long)vgaWriteBottom +
     (((unsigned long)p - VGABASE) & vgaSegmentMask));
#endif
}

/*
 *-----------------------------------------------------------------------
 * vgaReadWriteNext ---
 *     switch to next memory bank of the VGA board for read & write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */

pointer
vgaReadWriteNext(p)
     register pointer p;
{
  (vgaSetReadWriteFunc)(++writeseg);
  return (pointer)((unsigned long)p - vgaSegmentSize);
}

/*
 *-----------------------------------------------------------------------
 * vgaReadWritePrev ---
 *     switch to previous memory bank of the VGA board for read & write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */

pointer
vgaReadWritePrev(p)
     register pointer p;
{
  (vgaSetReadWriteFunc)(--writeseg); 
  return (pointer)((unsigned long)p + vgaSegmentSize);
}

/*
 *-----------------------------------------------------------------------
 * vgaSetRead ---
 *     select a memory bank of the VGA board for read access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
pointer
vgaSetRead(p)
    register pointer p;
{
#ifdef XF86VGA16
  readseg = ((unsigned long)p - (unsigned long)vgaBase) >> vgaSegmentShift;
  (vgaSetReadFunc)(readseg);
  return (vgaReadBottom + (((unsigned long)p - (unsigned long)vgaBase) & vgaSegmentMask));
#else
  readseg = ((unsigned long)p - VGABASE) >> vgaSegmentShift;
  (vgaSetReadFunc)(readseg);
  return (pointer)
    ((unsigned long)vgaReadBottom +
     (((unsigned long)p - VGABASE) & vgaSegmentMask));
#endif
}

/*
 *-----------------------------------------------------------------------
 * vgaReadNext ---
 *     switch to next memory bank of the VGA board for read access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */

pointer
vgaReadNext(p)
     register pointer p;
{
  (vgaSetReadFunc)(++readseg);
  return (pointer)((unsigned long)p - vgaSegmentSize);
}

/*
 *-----------------------------------------------------------------------
 * vgaReadPrev ---
 *     switch to previous memory bank of the VGA board for read access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
pointer
vgaReadPrev(p)
     register pointer p;
{
  (vgaSetReadFunc)(--readseg); 
  return (pointer)((unsigned long)p + vgaSegmentSize);
}

/*
 *-----------------------------------------------------------------------
 * vgaSetWrite ---
 *     select a memory bank of the VGA board for write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
pointer
vgaSetWrite(p)
    register pointer p;
{
#ifdef XF86VGA16
  writeseg = ((unsigned long)p - (unsigned long)vgaBase) >> vgaSegmentShift;
  (vgaSetWriteFunc)(writeseg);
  return (vgaWriteBottom + (((unsigned long)p - (unsigned long)vgaBase) & vgaSegmentMask));
#else
  writeseg = ((unsigned long)p - VGABASE) >> vgaSegmentShift;
  (vgaSetWriteFunc)(writeseg);
  return (pointer)
    ((unsigned long)vgaWriteBottom +
     (((unsigned long)p - VGABASE) & vgaSegmentMask));
#endif
}

/*
 *-----------------------------------------------------------------------
 * vgaWriteNext ---
 *     switch to next memory bank of the VGA board for write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
pointer
vgaWriteNext(p)
     register pointer p;
{
  (vgaSetWriteFunc)(++writeseg);
  return (pointer)((unsigned long)p - vgaSegmentSize);
}

/*
 *-----------------------------------------------------------------------
 * vgaWritePrev ---
 *     switch to previous memory bank of the VGA board for write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
pointer
vgaWritePrev(p)
     register pointer p;
{
  (vgaSetWriteFunc)(--writeseg); 
  return (pointer)((unsigned long)p + vgaSegmentSize);
}

/*
 *-----------------------------------------------------------------------
 * vgaSaveBank --
 *     save Banking-state
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
void
vgaSaveBank()
{
  saveseg = writeseg;
}

/*
 *-----------------------------------------------------------------------
 * vgaRestoreBank --
 *     restore the banking after vgaSaveBank was called
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
void
vgaRestoreBank()
{
  (vgaSetWriteFunc)(saveseg);
  (vgaSetReadFunc)(saveseg);
}

/*
 *-----------------------------------------------------------------------
 * vgaPushRead ---
 *     make the write-bank also readable. no acces to the former read bank !
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
void
vgaPushRead(void)
{
  (vgaSetReadWriteFunc)(writeseg);
}

/*
 *-----------------------------------------------------------------------
 * vgaPopRead ---
 *     restore the banking after vgaPushRead was called
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
void
vgaPopRead(void)
{
  (vgaSetWriteFunc)(writeseg);
  (vgaSetReadFunc)(readseg);
}

/*
 *-----------------------------------------------------------------------
 * vgaSetVidPage ---
 *     select a memory bank of the VGA board for read & write access
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 */
void
vgaSetVidPage(page)
     register int page;
{
  (vgaSetReadWriteFunc)(page);
  return;
}
