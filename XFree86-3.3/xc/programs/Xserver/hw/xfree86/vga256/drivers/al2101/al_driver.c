/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/al2101/al_driver.c,v 3.16.2.2 1997/05/09 07:15:25 hohndel Exp $ */
/*
 * Copyright 1994 by Paolo Severini, Italy.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Paolo Severini not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Paolo Severini makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * PAOLO SEVERINI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL PAOLO SEVERINI BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Paolo Severini, lendl@dist.dist.unige.it
 *
 */
/* $XConsortium: al_driver.c /main/12 1996/10/27 11:06:44 kaleb $ */


#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "vga.h"

#ifdef XFreeXDGA
#include "X.h" 
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

typedef struct {
  vgaHWRec std;               /* good old IBM VGA */
  unsigned char ExtFuncReg1;
  unsigned char ExtCRTCreg1;
  unsigned char AbilReadBank;
  unsigned char ReadBank;         
  unsigned char WriteBank;
  } vgaAL2101Rec, *vgaAL2101Ptr;

static Bool     AL2101Probe();
static char *   AL2101Ident();
static Bool     AL2101ClockSelect();
static void     AL2101EnterLeave();
static Bool     AL2101Init();
static int      AL2101ValidMode();
static void *   AL2101Save();
static void     AL2101Restore();
static void     AL2101Adjust();
extern void     AL2101SetRead();
extern void     AL2101SetWrite();
extern void     AL2101SetReadWrite();

vgaVideoChipRec AL2101 = {
  AL2101Probe,
  AL2101Ident,
  AL2101EnterLeave,
  AL2101Init,
  AL2101ValidMode,
  AL2101Save,
  AL2101Restore,
  AL2101Adjust,
  vgaHWSaveScreen,
  (void (*)())NoopDDA,
  (void (*)())NoopDDA,
  AL2101SetRead,
  AL2101SetWrite,
  AL2101SetReadWrite,
  0x10000,
  0x10000,
  16,
  0xFFFF,
  0x00000, 0x10000,
  0x00000, 0x10000,
  TRUE,                               /* Uses 2 banks */
  VGA_NO_DIVIDE_VERT,
  {0,},
  8,
  FALSE,
  0,
  0,
  FALSE,
  FALSE,
  FALSE,
  NULL,
  1,
  1
};


#define new ((vgaAL2101Ptr)vgaNewVideoState)

static unsigned AL2101_IOPorts[] = {
	0x3D6, 0x3D7
};

/*
 * AL2101ClockSelect --
 *      select one of the possible clocks ...
 */

static Bool
AL2101ClockSelect(no)
     int no;
{
  static unsigned char save1, save2;
  unsigned char temp;

  switch(no)
  {
    case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(0x3CE, 0x0C); save2 = inb(0x3CF);
      break;
    case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outw(0x3CE, 0x0C | (save2 << 8));
      break;
    default:
      temp = inb(0x3CC);
      outb(0x3C2, ( temp & 0xf3) | ((no << 2) & 0x0C));
      outb(0x3CE, 0x0C); temp = inb(0x3CF);
      outw(0x3CE, 0x0C | (( (temp & 0xDF) | ((no & 0x04)<<3)) <<8) );
  }
  return(TRUE);
}


/*
 * AL2101Ident --
 */

static char *
AL2101Ident(n)
     int n;
{
  static char *chipsets[] = {"al2101"};

  if (n + 1 > sizeof(chipsets) / sizeof(char *))
    return(NULL);
  else
    return(chipsets[n]);
}


/*
 * AL2101Probe --
 *      check up whether a Al2101 based board is installed
 */

static Bool
AL2101Probe()
{
  unsigned char tmp, tmp2, tmp3;

  /*
   * Set up I/O ports to be used by this card
   */  
  xf86ClearIOPortList(vga256InfoRec.scrnIndex);
  xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
  xf86AddIOPorts(vga256InfoRec.scrnIndex, sizeof(AL2101_IOPorts) /
      sizeof(AL2101_IOPorts[0]), AL2101_IOPorts);

  if (vga256InfoRec.chipset)
    {
      if (StrCaseCmp(vga256InfoRec.chipset, AL2101Ident(0)))
	return (FALSE);
      else
	AL2101EnterLeave(ENTER);
    }
  else
    {
      AL2101EnterLeave(ENTER);
      outb(vgaIOBase + 4, 0x1A);
      tmp= inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, tmp & 0xE0); /* disable extensions */
      outb(vgaIOBase + 4, 0x19);
      tmp2=inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, tmp2^0xFF);
      tmp3=inb(vgaIOBase + 5);
      if (tmp3 != (tmp2 ^ 0xFF))
	{
	  outw(vgaIOBase + 4, ((tmp|0x10)<<8) | 0x1A);
          outb(vgaIOBase + 4, 0x19);
          tmp2=inb(vgaIOBase + 5);
          outb(vgaIOBase + 5, tmp2^0xFF);
          tmp3=inb(vgaIOBase + 5);
	  outb(vgaIOBase + 5, tmp2);
          if (tmp3 == (tmp2 ^ 0xFF))
	    {
              outb(vgaIOBase + 4, 0x1A);
              tmp2=inb(vgaIOBase + 5);
              outb(vgaIOBase + 5, tmp2^0x3F);
              tmp3=inb(vgaIOBase + 5);
	      outb(vgaIOBase + 5, tmp2);
              if (tmp3 == (tmp2 ^ 0x3F))
		{
 		  if (!vga256InfoRec.videoRam) vga256InfoRec.videoRam = 1024;
 		  if (!vga256InfoRec.clocks) vgaGetClocks(8, AL2101ClockSelect);

 		  vga256InfoRec.chipset = AL2101Ident(0);
  		  vga256InfoRec.bankedMono = FALSE;
 		  return(TRUE);
		}
	    }
	}
      outw(vgaIOBase + 4, (tmp<<8) | 0x1A);
      AL2101EnterLeave(LEAVE);	
      return(FALSE);
    }   
  if (!vga256InfoRec.videoRam) vga256InfoRec.videoRam = 1024;
  if (!vga256InfoRec.clocks) vgaGetClocks(8, AL2101ClockSelect);

  vga256InfoRec.chipset = AL2101Ident(0);
  vga256InfoRec.bankedMono = FALSE;
#ifndef MONOVGA
#ifdef XFreeXDGA
  vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

  return(TRUE);
}



/*
 * AL2101EnterLeave --
 *      enable/disable io-mapping
 */

static void 
AL2101EnterLeave(enter)
     Bool enter;
{
  unsigned char temp;

#ifndef MONOVGA
#ifdef XFreeXDGA
  if (vga256InfoRec.directMode&XF86DGADirectGraphics && !enter)
    return;
#endif
#endif

  if (enter)
    {
      xf86EnableIOPorts(vga256InfoRec.scrnIndex);

      vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
      outb(vgaIOBase + 4, 0x1A); temp = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, temp | 0x10);
    }
  else
    {
      xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    }
}


/*
 * AL2101Restore --
 *      restore a video mode
 */

static void 
AL2101Restore(restore)
  vgaAL2101Ptr restore;
{
  vgaProtect(TRUE);

  outb(0x3D6,0x00);outb(0x3D7,0x00);

  outw(0x3CE, (restore->AbilReadBank  << 8) | 0x0F);
  outb(0x3D6, restore->ReadBank);
  outb(0x3D7, restore->WriteBank);
  outw(0x3CE, (restore->ExtFuncReg1 << 8) | 0x0C);
  outw(vgaIOBase + 4, (restore->ExtCRTCreg1 << 8) | 0x19);

  vgaHWRestore((vgaHWPtr)restore);

  outw(0x3C4, 0x0300); /* now reenable the timing sequencer */

  vgaProtect(FALSE);
}



/*
 * AL2101Save --
 *      save the current video mode
 */

static void *
AL2101Save(save)
     vgaAL2101Ptr save;
{
  unsigned char    tmp1, tmp2, tmp3;

  tmp1 = inb(0x3D6);
  tmp2 = inb(0x3D7);
  outb(0x3CE,0x0F);  tmp3=inb(0x3CF);

  save = (vgaAL2101Ptr)vgaHWSave((vgaHWPtr)save, sizeof(vgaAL2101Rec));

  save->ReadBank     = tmp1;
  save->WriteBank    = tmp2;
  save->AbilReadBank = tmp3;
  outb(0x3D6,0x00);outb(0x3D7,0x00); /*segment select*/

  outb(vgaIOBase + 4, 0x19); save->ExtCRTCreg1  = inb(vgaIOBase + 5);
  outb(0x3CE,0x0C);          save->ExtFuncReg1  = inb(0x3CF);

  return ((void *) save);
}



/*
 * AL2101Init --
 *      Handle the initialization, etc. of a screen.
 */

static Bool
AL2101Init(mode)
     DisplayModePtr mode;
{
  if (!vgaHWInit(mode,sizeof(vgaAL2101Rec)))
    return(FALSE);
  new->ExtFuncReg1   = 0x10; 
  new->ExtCRTCreg1   = 0x12; 
  new->AbilReadBank  = 0x04;
  new->ReadBank      = 0x00;
  new->WriteBank     = 0x00;
  new->std.CRTC[5] |= 0x60;	/* Hsync skew */
  new->std.CRTC[19]  = vga256InfoRec.displayWidth >> 4;

  return(TRUE);
}


/*
 * AL2101Adjust --
 *      adjust the current video frame to display the mousecursor
 */

static void 
AL2101Adjust(x, y)
     int x, y;
{
  int Base = (y * vga256InfoRec.displayWidth + x) >> 3;

  outw(vgaIOBase + 4, (Base & 0x00FF00)        | 0x0C);
  outw(vgaIOBase + 4, ((Base & 0x00FF) << 8)   | 0x0D);
  outw(vgaIOBase + 4, ((Base & 0x070000) >> 8) | 0x20);
#ifdef XFreeXDGA
  if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
    /* Wait until vertical retrace is in progress. */
    while (inb(vgaIOBase + 0xA) & 0x08);
    while (!(inb(vgaIOBase + 0xA) & 0x08));
  }
#endif
}

/*
 * AL2101ValidMode --
 *
 */
static int
AL2101ValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}
