/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaHW.c,v 3.50.2.1 1997/05/10 07:03:00 hohndel Exp $
 *
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 */
/* $XConsortium: vgaHW.c /main/19 1996/10/28 04:55:33 kaleb $ */

#ifdef ISC202
#include <sys/types.h>
#define WIFEXITED(a)  ((a & 0x00ff) == 0)  /* LSB will be 0 */
#define WEXITSTATUS(a) ((a & 0xff00) >> 8)
#define WIFSIGNALED(a) ((a & 0xff00) == 0) /* MSB will be 0 */
#define WTERMSIG(a) (a & 0x00ff)
#else
#if defined(ISC) && !defined(_POSIX_SOURCE)
#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#undef _POSIX_SOURCE
#else
#if defined(MINIX) || defined(AMOEBA) || (defined(ISC) && defined(_POSIX_SOURCE)) || defined(Lynx)
#include <sys/types.h>
#endif
#include <sys/wait.h>
#endif
#endif

#if !defined(AMOEBA) && !defined(MINIX)
#define _NEED_SYSI86
#endif

#include "X.h"
#include "misc.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#if 0
#include <errno.h>
#else
#include "xf86_OSlib.h"
#endif
#include "xf86_HWlib.h"
#include "vga.h"
#ifdef PC98_EGC
/* I/O port address define for extended EGC */
#define		EGC_READ	0x4a2	/* EGC FGC,EGC,Read Plane  */
#define		EGC_MASK	0x4a8	/* EGC Mask register       */
#define		EGC_ADD		0x4ac	/* EGC Dest/Source address */
#define		EGC_LENGTH	0x4ae	/* EGC Bit length          */
#endif

#if !defined(PC98_NEC480) && !defined(PC98_EGC)
#if !defined(MONOVGA) && !defined(SCO)
#ifndef SAVE_FONT1
#define SAVE_FONT1
#endif
#endif

#if defined(Lynx) || defined(CSRG_BASED) || defined(MACH386) || defined(linux) || defined(AMOEBA) || defined(MINIX)
#ifndef NEED_SAVED_CMAP
#define NEED_SAVED_CMAP
#endif
#ifndef MONOVGA
#ifndef SAVE_TEXT
#define SAVE_TEXT
#endif
#endif
#ifndef SAVE_FONT2
#define SAVE_FONT2
#endif
#endif

/* bytes per plane to save for text */
#if defined(Lynx) || defined(linux) || defined(MINIX)
#define TEXT_AMOUNT 16384
#else
#define TEXT_AMOUNT 4096
#endif

/* bytes per plane to save for font data */
#define FONT_AMOUNT 8192
#endif /* !defined(PC98_NEC480) && !defined(PC98_EGC) */

#if defined(CSRG_BASED) || defined(MACH386)
#include <sys/time.h>
#endif

#ifdef MACH386
#define WEXITSTATUS(x) (x.w_retcode)
#define WTERMSIG(x) (x.w_termsig)
#define WSTOPSIG(x) (x.w_stopsig)
#endif

/* This the only where the definition seems to work (out of
 * vga.c/vgaHW.c/vgaCmap.c).
 */
Bool clgd6225Lcd= FALSE;

#ifdef MONOVGA
/* DAC indices for white and black */
#define WHITE_VALUE 0x3F
#define BLACK_VALUE 0x00
#define OVERSCAN_VALUE 0x01
#endif

static CARD32
vgaOffMode(
#if NeedFunctionPrototypes
     OsTimerPtr	/* timer */,
     CARD32	/* now */,
     pointer	/* arg */
#endif
);

static CARD32
vgaSuspendMode(
#if NeedFunctionPrototypes
     OsTimerPtr	/* timer */,
     CARD32	/* now */,
     pointer	/* arg */
#endif
);

static Bool
setExternClock(
#if NeedFunctionPrototypes
     int	/* clock */
#endif
);

static int currentGraphicsClock = -1;
static int currentExternClock = -1;

int vgaRamdacMask = 0x3F;

#define new ((vgaHWPtr)vgaNewVideoState)

unsigned VGA_IOPorts[] = {
#ifdef PC98_NEC480
	0x60,  0x62,  0x6a,  0x7c,  0x7e,  0x80,  0xa0,  0xa2,  0x3c0, 0x3c2,
	0x3c4, 0x3c6, 0x3c7, 0x3c8, 0x3c9, 0x3cc, 0x3ce, 0x3d0, 0x9a0, 0x9a8,
#else
#ifdef PC98_EGC
/* I/O port address define Normal & Hireso mode */
	0xa8,  0xaa,  0xac,  0xae,  0x4a0, 0x4a1, 0x4a2, 0x4a3, 0x4a4, 0x4a5,
	0x4a6, 0x4a7, 0x4a8, 0x4a9, 0x4aa, 0x4ab, 0x4ac, 0x4ad, 0x4ae, 0x4af,
#else
	0x3B4, 0x3B5, 0x3BA, 0x3C0, 0x3C1, 0x3C2, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 
	0x3C8, 0x3C9, 0x3CA, 0x3CB, 0x3CC, 0x3CE, 0x3CF, 0x3D4, 0x3D5, 0x3DA,
#endif
#endif
};
int Num_VGA_IOPorts = (sizeof(VGA_IOPorts)/sizeof(VGA_IOPorts[0]));

#ifdef NEED_SAVED_CMAP
/* This default colourmap is used only when it can't be read from the VGA */

unsigned char defaultDAC[768] =
{
     0,  0,  0,    0,  0, 42,    0, 42,  0,    0, 42, 42,
    42,  0,  0,   42,  0, 42,   42, 21,  0,   42, 42, 42,
    21, 21, 21,   21, 21, 63,   21, 63, 21,   21, 63, 63,
    63, 21, 21,   63, 21, 63,   63, 63, 21,   63, 63, 63,
     0,  0,  0,    5,  5,  5,    8,  8,  8,   11, 11, 11,
    14, 14, 14,   17, 17, 17,   20, 20, 20,   24, 24, 24,
    28, 28, 28,   32, 32, 32,   36, 36, 36,   40, 40, 40,
    45, 45, 45,   50, 50, 50,   56, 56, 56,   63, 63, 63,
     0,  0, 63,   16,  0, 63,   31,  0, 63,   47,  0, 63,
    63,  0, 63,   63,  0, 47,   63,  0, 31,   63,  0, 16,
    63,  0,  0,   63, 16,  0,   63, 31,  0,   63, 47,  0,
    63, 63,  0,   47, 63,  0,   31, 63,  0,   16, 63,  0,
     0, 63,  0,    0, 63, 16,    0, 63, 31,    0, 63, 47,
     0, 63, 63,    0, 47, 63,    0, 31, 63,    0, 16, 63,
    31, 31, 63,   39, 31, 63,   47, 31, 63,   55, 31, 63,
    63, 31, 63,   63, 31, 55,   63, 31, 47,   63, 31, 39,
    63, 31, 31,   63, 39, 31,   63, 47, 31,   63, 55, 31,
    63, 63, 31,   55, 63, 31,   47, 63, 31,   39, 63, 31,
    31, 63, 31,   31, 63, 39,   31, 63, 47,   31, 63, 55,
    31, 63, 63,   31, 55, 63,   31, 47, 63,   31, 39, 63,
    45, 45, 63,   49, 45, 63,   54, 45, 63,   58, 45, 63,
    63, 45, 63,   63, 45, 58,   63, 45, 54,   63, 45, 49,
    63, 45, 45,   63, 49, 45,   63, 54, 45,   63, 58, 45,
    63, 63, 45,   58, 63, 45,   54, 63, 45,   49, 63, 45,
    45, 63, 45,   45, 63, 49,   45, 63, 54,   45, 63, 58,
    45, 63, 63,   45, 58, 63,   45, 54, 63,   45, 49, 63,
     0,  0, 28,    7,  0, 28,   14,  0, 28,   21,  0, 28,
    28,  0, 28,   28,  0, 21,   28,  0, 14,   28,  0,  7,
    28,  0,  0,   28,  7,  0,   28, 14,  0,   28, 21,  0,
    28, 28,  0,   21, 28,  0,   14, 28,  0,    7, 28,  0,
     0, 28,  0,    0, 28,  7,    0, 28, 14,    0, 28, 21,
     0, 28, 28,    0, 21, 28,    0, 14, 28,    0,  7, 28,
    14, 14, 28,   17, 14, 28,   21, 14, 28,   24, 14, 28,
    28, 14, 28,   28, 14, 24,   28, 14, 21,   28, 14, 17,
    28, 14, 14,   28, 17, 14,   28, 21, 14,   28, 24, 14,
    28, 28, 14,   24, 28, 14,   21, 28, 14,   17, 28, 14,
    14, 28, 14,   14, 28, 17,   14, 28, 21,   14, 28, 24,
    14, 28, 28,   14, 24, 28,   14, 21, 28,   14, 17, 28,
    20, 20, 28,   22, 20, 28,   24, 20, 28,   26, 20, 28,
    28, 20, 28,   28, 20, 26,   28, 20, 24,   28, 20, 22,
    28, 20, 20,   28, 22, 20,   28, 24, 20,   28, 26, 20,
    28, 28, 20,   26, 28, 20,   24, 28, 20,   22, 28, 20,
    20, 28, 20,   20, 28, 22,   20, 28, 24,   20, 28, 26,
    20, 28, 28,   20, 26, 28,   20, 24, 28,   20, 22, 28,
     0,  0, 16,    4,  0, 16,    8,  0, 16,   12,  0, 16,
    16,  0, 16,   16,  0, 12,   16,  0,  8,   16,  0,  4,
    16,  0,  0,   16,  4,  0,   16,  8,  0,   16, 12,  0,
    16, 16,  0,   12, 16,  0,    8, 16,  0,    4, 16,  0,
     0, 16,  0,    0, 16,  4,    0, 16,  8,    0, 16, 12,
     0, 16, 16,    0, 12, 16,    0,  8, 16,    0,  4, 16,
     8,  8, 16,   10,  8, 16,   12,  8, 16,   14,  8, 16,
    16,  8, 16,   16,  8, 14,   16,  8, 12,   16,  8, 10,
    16,  8,  8,   16, 10,  8,   16, 12,  8,   16, 14,  8,
    16, 16,  8,   14, 16,  8,   12, 16,  8,   10, 16,  8,
     8, 16,  8,    8, 16, 10,    8, 16, 12,    8, 16, 14,
     8, 16, 16,    8, 14, 16,    8, 12, 16,    8, 10, 16,
    11, 11, 16,   12, 11, 16,   13, 11, 16,   15, 11, 16,
    16, 11, 16,   16, 11, 15,   16, 11, 13,   16, 11, 12,
    16, 11, 11,   16, 12, 11,   16, 13, 11,   16, 15, 11,
    16, 16, 11,   15, 16, 11,   13, 16, 11,   12, 16, 11,
    11, 16, 11,   11, 16, 12,   11, 16, 13,   11, 16, 15,
    11, 16, 16,   11, 15, 16,   11, 13, 16,   11, 12, 16,
     0,  0,  0,    0,  0,  0,    0,  0,  0,    0,  0,  0,
     0,  0,  0,    0,  0,  0,    0,  0,  0,    0,  0,  0,
};
#endif /* NEED_SAVED_CMAP */

/*
 * With Intel, the version in common_hw/SlowBcopy.s is used.
 * This avoids port I/O during the copy (which causes problems with
 * some hardware).
 */
#ifdef __alpha__
#define slowbcopy_tobus(src,dst,count) SlowBCopyToBus(src,dst,count)
#define slowbcopy_frombus(src,dst,count) SlowBCopyFromBus(src,dst,count)
#else /* __alpha__ */
#define slowbcopy_tobus(src,dst,count) SlowBcopy(src,dst,count)
#define slowbcopy_frombus(src,dst,count) SlowBcopy(src,dst,count)
#endif /* __alpha__ */

/*
 * vgaProtect --
 *	Protect VGA registers and memory from corruption during loads.
 */
void
vgaProtect(on)
     Bool on;
{
#if !defined(PC98_NEC480) && !defined(PC98_EGC)
  unsigned char tmp;

  if (xf86VTSema) {
    if (on) {
      /*
       * Turn off screen and disable sequencer.
       */
      outb(0x3C4, 0x01);
      tmp = inb(0x3C5);

      (*vgaSaveScreenFunc)(SS_START);		/* start synchronous reset */
      outw(0x3C4, ((tmp | 0x20) << 8) | 0x01);	/* disable the display */

      tmp = inb(vgaIOBase + 0x0A);
      outb(0x3C0, 0x00);			/* enable pallete access */
    }
    else {
      /*
       * Reenable sequencer, then turn on screen.
       */
      outb(0x3C4, 0x01);
      tmp = inb(0x3C5);

      outw(0x3C4, ((tmp & 0xDF) << 8) | 0x01);	/* reenable display */
      (*vgaSaveScreenFunc)(SS_FINISH);		/* clear synchronousreset */

      tmp = inb(vgaIOBase + 0x0A);
      outb(0x3C0, 0x20);			/* disable pallete access */
    }
  }
#endif /* !defined(PC98_NEC480) && !defined(PC98_EGC) */
}

/*
 * vgaSaveScreen -- blank the screen.
 */

Bool
vgaSaveScreen(pScreen, on)
     ScreenPtr pScreen;
     Bool  on;
{
#if !defined(PC98_EGC) && !defined(PC98_NEC480)
   unsigned char scrn;

   if (on)
      SetTimeSinceLastInputEvent();

   if (xf86VTSema) {
      /* the server is running on the current vt */
      /* so just go for it */

      outb(0x3C4,1);
      scrn = inb(0x3C5);

      if (on) {
	 scrn &= 0xDF;			/* enable screen */
      } else {
	 scrn |= 0x20;			/* blank screen */
      }

      (*vgaSaveScreenFunc)(SS_START);
      outw(0x3C4, (scrn << 8) | 0x01); /* change mode */
      (*vgaSaveScreenFunc)(SS_FINISH);
   }
#else /* PC98_EGC || PC98_NEC480 */
  if (on)
    SetTimeSinceLastInputEvent();
  
  if (xf86VTSema) 
    {
      if (on) 
	outb(0xa2, 0xd);
      else
	outb(0xa2, 0xc);
    }
#endif /* PC98_EGC || PC98_NEC480 */
   return (TRUE);
}

/*
 * vgaDPMSSet -- Sets VESA Display Power Management Signaling (DPMS) Mode
 *
 * This generic VGA function can only set the Off and On modes.  If the
 * Standby and Suspend modes are to be supported, a chip specific replacement
 * for this function must be written.
 */

void
vgaDPMSSet(PowerManagementMode)
    int PowerManagementMode;
{
#ifdef DPMSExtension
  unsigned char seq1, crtc17;
  if (!xf86VTSema) return;
  switch (PowerManagementMode)
  {
  case DPMSModeOn:
    /* Screen: On; HSync: On, VSync: On */
    seq1 = 0x00;
    crtc17 = 0x80;
    break;
  case DPMSModeStandby:
    /* Screen: Off; HSync: Off, VSync: On -- Not Supported */
    seq1 = 0x20;
    crtc17 = 0x80;
    break;
  case DPMSModeSuspend:
    /* Screen: Off; HSync: On, VSync: Off -- Not Supported */
    seq1 = 0x20;
    crtc17 = 0x80;
    break;
  case DPMSModeOff:
    /* Screen: Off; HSync: Off, VSync: Off */
    seq1 = 0x20;
    crtc17 = 0x00;
    break;
  }
  outw(0x3C4, 0x0100);	/* Synchronous Reset */
  outb(0x3C4, 0x01);	/* Select SEQ1 */
  seq1 |= inb(0x3C5) & ~0x20;
  outb(0x3C5, seq1);
  outb(vgaIOBase+4, 0x17); /* Select CRTC17 */
  crtc17 |= inb(vgaIOBase+5) & ~0x80;
  usleep(10000);
  outb(vgaIOBase+5, crtc17);
  outw(0x3C4, 0x0300);	/* End Reset */
#endif
}

/*
 * vgaHWSaveScreen
 *      perform a sequencer reset.
 */

void
vgaHWSaveScreen(start)
    Bool start;
{
#if !defined(PC98_NEC480) && !defined(PC98_EGC)
  if (start == SS_START)
    outw(0x3C4, 0x0100);        /* synchronous reset */
  else
    outw(0x3C4, 0x0300);        /* end reset */
#endif
}

/*
 * setExternClock
 *      call the external clock program
 *
 */

static Bool
setExternClock(clock2)
     int clock2;       /* the Clock index */
{
    int i;
#ifdef MACH386
    union wait exit_status;
#else
    int exit_status;
#endif

    if (clock2 == currentExternClock)
      return(TRUE);
    switch(fork())
    {
      case -1:
        ErrorF("Fork failed for ClockProg (%s)\n", strerror(errno));
        return(FALSE);
      case 0:  /* child */
	/* 
	 * Make sure that the child doesn't inherit any I/O permissions it
	 * shouldn't have.  It's better to put constraints on the development
	 * of a clock program than to give I/O permissions to a bogus program
	 * in someone's XF86Config file
	 */
	for (i = 0; i < MAXSCREENS; i++)
	  xf86DisableIOPorts(i);
        setuid(getuid());
#if !defined(AMOEBA) && !defined(MINIX)
        /* set stdin, stdout to the consoleFD, and leave stderr alone */
        for (i = 0; i < 2; i++)
        {
          if (xf86Info.consoleFd != i)
          {
            close(i);
            dup(xf86Info.consoleFd);
          }
        }
#endif
        {
          char *progname, clockarg[8], clockindex[4];

          if ((progname = strrchr(vga256InfoRec.clockprog, '/')))
            progname++;
          else
            progname = vga256InfoRec.clockprog;
 	  if(clock2 < MAXCLOCKS){
 	    sprintf(clockarg, "%.3f", vga256InfoRec.clock[clock2] / 1000.0);
 	    sprintf(clockindex, "%d", clock2 );
 	  } else {
 	    sprintf(clockarg, "%.3f", clock2 / 1000.0);
 	    sprintf(clockindex, "%d",255 );
 	  }
          execl(vga256InfoRec.clockprog, progname, clockarg, clockindex, NULL);
          ErrorF("Exec failed for ClockProg command \"%s\" (%s)\n",
                 vga256InfoRec.clockprog, strerror(errno));
          exit(255);
        }
        break;
      default:  /* parent */
        wait(&exit_status);
        if (WIFEXITED(exit_status))
        {
          switch (WEXITSTATUS(exit_status))
          {
            case 0:     /* OK */
              break;
            case 255:   /* exec() failed */
              return(FALSE);
            default:    /* bad exit status */
              ErrorF("ClockProg \"%s\" had bad exit status %d\n",
                     vga256InfoRec.clockprog, WEXITSTATUS(exit_status));
              return(FALSE);
          }
        }
        else if (WIFSIGNALED(exit_status))
        {
          ErrorF("ClockProg \"%s\" died on signal %d\n",
                 vga256InfoRec.clockprog, WTERMSIG(exit_status));
          return(FALSE);
        }
#ifdef WIFSTOPPED
        else if (WIFSTOPPED(exit_status))
        {
          ErrorF("ClockProg \"%s\" stopped by signal %d\n",
                 vga256InfoRec.clockprog, WSTOPSIG(exit_status));
          return(FALSE);
        }
#endif
        else /* should never get to this point */
        {
          ErrorF("ClockProg \"%s\" has unknown exit condition\n",
                 vga256InfoRec.clockprog);
          return(FALSE);
        }
    }
    currentExternClock = clock2;
    return(TRUE);
}


/*
 * vgaHWRestore --
 *      restore a video mode
 */

void
vgaHWRestore(restore)
     vgaHWPtr restore;
{
  int i,tmp;

#if !defined(PC98_NEC480) && !defined(PC98_EGC)
  tmp = inb(vgaIOBase + 0x0A);		/* Reset flip-flop */
  outb(0x3C0, 0x00);			/* Enables pallete access */
#endif

  /*
   * This here is a workaround a bug in the kd-driver. We MUST explicitely
   * restore the font we got, when we entered graphics mode.
   * The bug was seen on ESIX, and ISC 2.0.2 when using a monochrome
   * monitor. 
   *
   * BTW, also GIO_FONT seems to have a bug, so we cannot use it, to get
   * a font.
   */
  
  vgaSaveScreen(NULL, FALSE);
#if defined(SAVE_TEXT) || defined(SAVE_FONT1) || defined(SAVE_FONT2)
  if(restore->FontInfo1 || restore->FontInfo2 || restore->TextInfo) {
    /*
     * here we switch temporary to 16 color-plane-mode, to simply
     * copy the font-info and saved text
     *
     * BUGALLERT: The vga's segment-select register MUST be set appropriate !
     */
    tmp = inb(vgaIOBase + 0x0A); /* reset flip-flop */
    outb(0x3C0,0x30); outb(0x3C0, 0x01); /* graphics mode */
#ifdef XF86VGA16
    outw(0x3CE,0x0003); /* GJA - don't rotate, write unmodified */
    outw(0x3CE,0xFF08); /* GJA - write all bits in a byte */
    outw(0x3CE,0x0001); /* GJA - all planes come from CPU */
#endif
#ifdef SAVE_FONT1
    if (restore->FontInfo1) {
      outw(0x3C4, 0x0402);    /* write to plane 2 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0204);    /* read plane 2 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_tobus(restore->FontInfo1, vgaBase, FONT_AMOUNT);
    }
#endif
#ifdef SAVE_FONT2
    if (restore->FontInfo2) {
      outw(0x3C4, 0x0802);    /* write to plane 3 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0304);    /* read plane 3 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_tobus(restore->FontInfo2, vgaBase, FONT_AMOUNT);
    }
#endif
#ifdef SAVE_TEXT
    if (restore->TextInfo) {
      outw(0x3C4, 0x0102);    /* write to plane 0 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0004);    /* read plane 0 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_tobus(restore->TextInfo, vgaBase, TEXT_AMOUNT);
      outw(0x3C4, 0x0202);    /* write to plane 1 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0104);    /* read plane 1 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_tobus((char *)restore->TextInfo + TEXT_AMOUNT, vgaBase, TEXT_AMOUNT);
    }
#endif
  }
#endif /* defined(SAVE_TEXT) || defined(SAVE_FONT1) || defined(SAVE_FONT2) */
  vgaSaveScreen(NULL, TRUE);

#if !defined(PC98_NEC480) && !defined(PC98_EGC)

  tmp = inb(vgaIOBase + 0x0A);			/* Reset flip-flop */
  outb(0x3C0, 0x00);				/* Enables pallete access */

  /* Don't change the clock bits when using an external clock program */
  if (restore->NoClock < 0)
  {
    tmp = inb(0x3CC);
    restore->MiscOutReg = (restore->MiscOutReg & 0xF3) | (tmp & 0x0C);
  }
  if (vgaIOBase == 0x3B0)
    restore->MiscOutReg &= 0xFE;
  else
    restore->MiscOutReg |= 0x01;

  outb(0x3C2, restore->MiscOutReg);

  for (i=1; i<5;  i++) outw(0x3C4, (restore->Sequencer[i] << 8) | i);
  
  /* Ensure CRTC registers 0-7 are unlocked by clearing bit 7 or CRTC[17] */

  outw(vgaIOBase + 4, ((restore->CRTC[17] & 0x7F) << 8) | 17);

  for (i=0; i<25; i++) outw(vgaIOBase + 4,(restore->CRTC[i] << 8) | i);

  for (i=0; i<9;  i++) outw(0x3CE, (restore->Graphics[i] << 8) | i);

  for (i=0; i<21; i++) {
    tmp = inb(vgaIOBase + 0x0A);
    outb(0x3C0,i); outb(0x3C0, restore->Attribute[i]);
  }
  
  if (clgd6225Lcd)
  {
    for (i= 0; i<768; i++)
    {
      /* The LCD doesn't like white */
      if (restore->DAC[i] == 63) restore->DAC[i]= 62;
    }
  }
  outb(0x3C6,0xFF);
  outb(0x3C8,0x00);
  for (i=0; i<768; i++)
  {
     outb(0x3C9, restore->DAC[i]);
     DACDelay;
  }

  /* Turn on PAS bit */
  tmp = inb(vgaIOBase + 0x0A);
  outb(0x3C0, 0x20);

#endif /* !defined(PC98_NEC480) && !defined(PC98_EGC) */

  /* If restoring text mode, and a text clock is specified with clkprog */
  if (((restore->Attribute[0x10] & 0x01) == 0))
  {
    if (vga256InfoRec.clockprog)
    {
      if (vga256InfoRec.textclock >= 0)
        setExternClock(vga256InfoRec.textclock);
      /*
       * Invalidate the saved extern clock when switching to text mode
       * because I think some systems modify this when VT switching.
       */
      currentExternClock = -1;
    }
  }
  else
  /* If restoring a graphics mode */
  {
    if (currentGraphicsClock >= 0)
      setExternClock(currentGraphicsClock);
  }
}

/*
 * vgaHWSave --
 *      save the current video mode
 */

void *
vgaHWSave(save, size)
     vgaHWPtr save;
     int          size;
{
  int           i,tmp;
  Bool	        first_time = FALSE;  /* Should be static? */

  if (save == NULL) {
    tmp = size;
    if (tmp < 0)
      tmp = -size;
    if (tmp < sizeof(vgaHWRec))
      tmp = sizeof(vgaHWRec);
    save = (vgaHWPtr)xcalloc(1,tmp);
    /*
     * Here we are, when we first save the videostate. This means we came here
     * to save the original Text mode. Because some drivers may depend
     * on NoClock we set it here to a resonable value.
     */
    first_time = TRUE;
    if (vga256InfoRec.clockprog && vga256InfoRec.textclock >= 0)
      save->NoClock = -1;
#if !defined(PC98_NEC480) && !defined(PC98_EGC)
    else
      /* This isn't very useful -- it ignores the high order CS bits */
      save->NoClock = (inb(0x3CC) >> 2) & 3;
#endif /* !defined(PC98_NEC480) && !defined(PC98_EGC) */
  }

#if !defined(PC98_NEC480) && !defined(PC98_EGC)
  save->MiscOutReg = inb(0x3CC);
#ifdef PC98
  save->MiscOutReg |= 0x01;
#endif
  vgaIOBase = (save->MiscOutReg & 0x01) ? 0x3D0 : 0x3B0;

  tmp = inb(vgaIOBase + 0x0A); /* reset flip-flop */
  outb(0x3C0, 0x00);

#ifdef NEED_SAVED_CMAP
  /*
   * Some recent (1991) ET4000 chips have a HW bug that prevents the reading
   * of the color lookup table.  Mask rev 9042EAI is known to have this bug.
   *
   * XF86 already keeps track of the contents of the color lookup table so
   * reading the HW isn't needed.  Therefore, as a workaround for this HW
   * bug, the following (correct) code has been #ifdef'ed out.  This is also
   * a valid change for ET4000 chips that don't have the HW bug.  The code
   * is really just being retained for reference.  MWS 22-Aug-91
   *
   * This is *NOT* true for 386BSD, Mach -- the initial colour map must be
   * restored.  When saving the text mode, we check if the colourmap is
   * readable.  If so we read it.  If not, we set the saved map to a
   * default map (taken from Ferraro's "Programmer's Guide to the EGA and
   * VGA Cards" 2nd ed).
   */

  if (first_time)
  {
    int read_error = 0;

    outb(0x3C6,0xFF);
    /*
     * check if we can read the lookup table
     */
    outb(0x3C7,0x00);
    for (i=0; i<3; i++) save->DAC[i] = inb(0x3C9);
#if defined(PC98_PW) || defined(PC98_XKB) || defined(PC98_NEC) || defined(PC98_PWLB)
    for (i=0; i<300; i++) inb(vgaIOBase + 4);
#endif
    outb(0x3C8,0x00);
    for (i=0; i<3; i++) outb(0x3C9, ~save->DAC[i] & vgaRamdacMask);
#if defined(PC98_PW) || defined(PC98_XKB) || defined(PC98_NEC) || defined(PC98_PWLB)
    for (i=0; i<300; i++) inb(vgaIOBase + 4);
#endif
    outb(0x3C7,0x00);
    for (i=0; i<3; i++)
    {
      unsigned char tmp2 = inb(0x3C9);
#if defined(PC98_PW) || defined(PC98_XKB) || defined(PC98_NEC) || defined(PC98_PWLB)
      if (tmp2 != (~save->DAC[i]&0xFF))
#endif
      if (tmp2 != (~save->DAC[i] & vgaRamdacMask)) read_error++;
    }
  
    if (read_error)
    {
      /*			 
       * save the default lookup table
       */
      bcopy(defaultDAC, save->DAC, 768);
      ErrorF("%s: Cannot read colourmap from VGA.", vga256InfoRec.name);
      ErrorF("  Will restore with default\n");
    }
    else
    {
      /*			 
       * save the colorlookuptable 
       */
      outb(0x3C7,0x01);
      for (i=3; i<768; i++)
      {
	save->DAC[i] = inb(0x3C9); 
	DACDelay;
      }
    }
  }
#endif /* NEED_SAVED_CMAP */

  for (i=0; i<25; i++) { outb(vgaIOBase + 4,i);
			 save->CRTC[i] = inb(vgaIOBase + 5); }

  for (i=0; i<21; i++) {
    tmp = inb(vgaIOBase + 0x0A);
    outb(0x3C0,i);
    save->Attribute[i] = inb(0x3C1);
  }

  for (i=0; i<9;  i++) { outb(0x3CE,i); save->Graphics[i]  = inb(0x3CF); }

  for (i=0; i<5;  i++) { outb(0x3C4,i); save->Sequencer[i]   = inb(0x3C5); }

#endif /* !defined(PC98_NEC480) && !defined(PC98_EGC) */

  vgaSaveScreen(NULL, FALSE);
  
#if !defined(PC98_NEC480) && !defined(PC98_EGC)
  /* XXXX Still not sure if this is needed.  It isn't done in the Restore */
  outb(0x3C2, save->MiscOutReg | 0x01);		/* shift to colour emulation */
  /* Since forced to colour mode, must use 0x3Dx instead of (vgaIOBase + x) */

#if defined(SAVE_TEXT) || defined(SAVE_FONT1) || defined(SAVE_FONT2)
  /*
   * get the character sets, and text screen if required
   */
  if (((save->Attribute[0x10] & 0x01) == 0) && (size >= 0)) {
#ifdef SAVE_FONT1
    if (save->FontInfo1 == NULL) {
      save->FontInfo1 = (pointer)xalloc(FONT_AMOUNT);
      /*
       * Here we switch temporary to 16 color-plane-mode, to simply
       * copy the font-info
       *
       * BUGALLERT: The vga's segment-select register MUST be set appropriate !
       */
      tmp = inb(0x3D0 + 0x0A); /* reset flip-flop */
      outb(0x3C0,0x30); outb(0x3C0, 0x01); /* graphics mode */
      outw(0x3C4, 0x0402);    /* write to plane 2 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0204);    /* read plane 2 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_frombus(vgaBase, save->FontInfo1, FONT_AMOUNT);
    }
#endif /* SAVE_FONT1 */
#ifdef SAVE_FONT2
    if (save->FontInfo2 == NULL) {
      save->FontInfo2 = (pointer)xalloc(FONT_AMOUNT);
      tmp = inb(0x3D0 + 0x0A); /* reset flip-flop */
      outb(0x3C0,0x30); outb(0x3C0, 0x01); /* graphics mode */
      outw(0x3C4, 0x0802);    /* write to plane 3 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0304);    /* read plane 3 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_frombus(vgaBase, save->FontInfo2, FONT_AMOUNT);
    }
#endif /* SAVE_FONT2 */
#ifdef SAVE_TEXT
    if (save->TextInfo == NULL) {
      save->TextInfo = (pointer)xalloc(2 * TEXT_AMOUNT);
      tmp = inb(0x3D0 + 0x0A); /* reset flip-flop */
      outb(0x3C0,0x30); outb(0x3C0, 0x01); /* graphics mode */
      /*
       * This is a quick hack to save the text screen for system that don't
       * restore it automatically.
       */
      outw(0x3C4, 0x0102);    /* write to plane 0 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0004);    /* read plane 0 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_frombus(vgaBase, save->TextInfo, TEXT_AMOUNT);
      outw(0x3C4, 0x0202);    /* write to plane 1 */
      outw(0x3C4, 0x0604);    /* enable plane graphics */
      outw(0x3CE, 0x0104);    /* read plane 1 */
      outw(0x3CE, 0x0005);    /* write mode 0, read mode 0 */
      outw(0x3CE, 0x0506);    /* set graphics */
      slowbcopy_frombus(vgaBase, (char *)save->TextInfo + TEXT_AMOUNT, TEXT_AMOUNT);
    }
#endif /* SAVE_TEXT */
  }
#endif /* defined(SAVE_TEXT) || defined(SAVE_FONT1) || defined(SAVE_FONT2) */

  outb(0x3C2, save->MiscOutReg);		/* back to original setting */
#endif /* !defined(PC98_NEC480) && !defined(PC98_EGC) */
  
  vgaSaveScreen(NULL, TRUE);

#if !defined(PC98_NEC480) && !defined(PC98_EGC)
  /* Turn on PAS bit */
  tmp = inb(vgaIOBase + 0x0A);
  outb(0x3C0, 0x20);
#endif /* !defined(PC98_NEC480) && !defined(PC98_EGC) */
  
  return ((void *) save);
}



/*
 * vgaHWInit --
 *      Handle the initialization, etc. of a screen.
 *      Return FALSE on failure.
 */

Bool
vgaHWInit(mode, size)
     DisplayModePtr      mode;
     int             size;
{
  unsigned int       i;

  /*
   * If we're using an external clock program, the first thing we do is
   * call it.  If it fails for any reason, we abort the mode switch and
   * hope that it hasn't screwed up the old clock setting.
   */
  if (vga256InfoRec.clockprog)
  {
    if (setExternClock(mode->Clock))
      currentGraphicsClock = mode->Clock;
    else
      return(FALSE);
  }

  if (vgaNewVideoState == NULL) {
    if (size < sizeof(vgaHWRec))
      size = sizeof(vgaHWRec);
    vgaNewVideoState = (void *)xcalloc(1,size);

    /*
     * initialize default colormap for monochrome
     */
    for (i=0; i<3;   i++) new->DAC[i] = 0x00;
    for (i=3; i<768; i++) new->DAC[i] = 0x3F;
#ifdef MONOVGA
    i = BLACK_VALUE * 3;
    new->DAC[i++] = vga256InfoRec.blackColour.red;
    new->DAC[i++] = vga256InfoRec.blackColour.green;
    new->DAC[i] = vga256InfoRec.blackColour.blue;
    i = WHITE_VALUE * 3;
    new->DAC[i++] = vga256InfoRec.whiteColour.red;
    new->DAC[i++] = vga256InfoRec.whiteColour.green;
    new->DAC[i] = vga256InfoRec.whiteColour.blue;
    i = OVERSCAN_VALUE * 3;
    new->DAC[i++] = 0x00;
    new->DAC[i++] = 0x00;
    new->DAC[i] = 0x00;
#endif
#ifndef MONOVGA
    /* Initialise overscan register */
    new->Attribute[17] = 0xFF;
#endif

  }

  /*
   * Get NoClock.  Set it to -1 if using an external clock setting program
   */
  if (vga256InfoRec.clockprog)
    new->NoClock = -1;
  else
    new->NoClock = mode->Clock;

  /*
   * compute correct Hsync & Vsync polarity 
   */
  if ((mode->Flags & (V_PHSYNC | V_NHSYNC))
      && (mode->Flags & (V_PVSYNC | V_NVSYNC)))
      {
	new->MiscOutReg = 0x23;
	if (mode->Flags & V_NHSYNC) new->MiscOutReg |= 0x40;
	if (mode->Flags & V_NVSYNC) new->MiscOutReg |= 0x80;
      }
      else
      {
	int VDisplay = mode->VDisplay;
	if (mode->Flags & V_DBLSCAN)
	  VDisplay *= 2;
	if      (VDisplay < 400)
		new->MiscOutReg = 0xA3;		/* +hsync -vsync */
	else if (VDisplay < 480)
		new->MiscOutReg = 0x63;		/* -hsync +vsync */
	else if (VDisplay < 768)
		new->MiscOutReg = 0xE3;		/* -hsync -vsync */
	else
		new->MiscOutReg = 0x23;		/* +hsync +vsync */
      }
  if (!vga256InfoRec.clockprog)
    new->MiscOutReg |= (new->NoClock & 0x03) << 2;
  
  /*
   * Time Sequencer
   */
#ifdef XF86VGA16
  new->Sequencer[0] = 0x02;
#else
  new->Sequencer[0] = 0x00;
#endif
  if (mode->Flags & V_CLKDIV2) 
    new->Sequencer[1] = 0x09;
  else
    new->Sequencer[1] = 0x01;
#ifdef MONOVGA
  new->Sequencer[2] = 1 << BIT_PLANE;
#else
  new->Sequencer[2] = 0x0F;
#endif
  new->Sequencer[3] = 0x00;                             /* Font select */
#if defined(MONOVGA) || defined(XF86VGA16)
  new->Sequencer[4] = 0x06;                             /* Misc */
#else
  new->Sequencer[4] = 0x0E;                             /* Misc */
#endif

  if (!mode->CrtcVAdjusted && (mode->Flags & V_INTERLACE) &&
      vgaInterlaceType == VGA_DIVIDE_VERT) {
    mode->CrtcVDisplay >>= 1;
    mode->CrtcVSyncStart >>= 1;
    mode->CrtcVSyncEnd >>= 1;
    mode->CrtcVTotal >>= 1;
    mode->CrtcVAdjusted = TRUE;
  }

  /*
   * CRTC Controller
   */
  new->CRTC[0]  = (mode->CrtcHTotal >> 3) - 5;
  new->CRTC[1]  = (mode->CrtcHDisplay >> 3) - 1;
  new->CRTC[2]  = (mode->CrtcHSyncStart >> 3) -1;
  new->CRTC[3]  = ((mode->CrtcHSyncEnd >> 3) & 0x1F) | 0x80;
  i = (((mode->CrtcHSkew << 2) + 0x10) & ~0x1F);
  if (i < 0x80)
    new->CRTC[3] |= i;
  new->CRTC[4]  = (mode->CrtcHSyncStart >> 3);
  new->CRTC[5]  = (((mode->CrtcHSyncEnd >> 3) & 0x20 ) << 2 )
    | (((mode->CrtcHSyncEnd >> 3)) & 0x1F);
  new->CRTC[6]  = (mode->CrtcVTotal - 2) & 0xFF;
  new->CRTC[7]  = (((mode->CrtcVTotal -2) & 0x100) >> 8 )
    | (((mode->CrtcVDisplay -1) & 0x100) >> 7 )
      | ((mode->CrtcVSyncStart & 0x100) >> 6 )
	| (((mode->CrtcVSyncStart) & 0x100) >> 5 )
	  | 0x10
	    | (((mode->CrtcVTotal -2) & 0x200)   >> 4 )
	      | (((mode->CrtcVDisplay -1) & 0x200) >> 3 )
		| ((mode->CrtcVSyncStart & 0x200) >> 2 );
  new->CRTC[8]  = 0x00;
  new->CRTC[9]  = ((mode->CrtcVSyncStart & 0x200) >>4) | 0x40;
  if (mode->Flags & V_DBLSCAN)
    new->CRTC[9] |= 0x80;
  new->CRTC[10] = 0x00;
  new->CRTC[11] = 0x00;
  new->CRTC[12] = 0x00;
  new->CRTC[13] = 0x00;
  new->CRTC[14] = 0x00;
  new->CRTC[15] = 0x00;
  new->CRTC[16] = mode->CrtcVSyncStart & 0xFF;
  new->CRTC[17] = (mode->CrtcVSyncEnd & 0x0F) | 0x20;
  new->CRTC[18] = (mode->CrtcVDisplay -1) & 0xFF;
  new->CRTC[19] = vga256InfoRec.displayWidth >> 4;  /* just a guess */
  new->CRTC[20] = 0x00;
  new->CRTC[21] = mode->CrtcVSyncStart & 0xFF; 
  new->CRTC[22] = (mode->CrtcVSyncEnd + 1) & 0xFF;
#if defined(MONOVGA) || defined(XF86VGA16)
  new->CRTC[23] = 0xE3;
#else
  new->CRTC[23] = 0xC3;
#endif
  new->CRTC[24] = 0xFF;

  /*
   * Graphics Display Controller
   */
  new->Graphics[0] = 0x00;
  new->Graphics[1] = 0x00;
  new->Graphics[2] = 0x00;
  new->Graphics[3] = 0x00;
#ifdef MONOVGA
  new->Graphics[4] = BIT_PLANE;
  new->Graphics[5] = 0x00;
#else
  new->Graphics[4] = 0x00;
#ifdef XF86VGA16
  new->Graphics[5] = 0x02;
#else
  new->Graphics[5] = 0x40;
#endif
#endif
  new->Graphics[6] = 0x05;   /* only map 64k VGA memory !!!! */
  new->Graphics[7] = 0x0F;
  new->Graphics[8] = 0xFF;
  
#ifdef MONOVGA
  /* Initialise the Mono map according to which bit-plane gets written to */

  for (i=0; i<16; i++)
    if (i & (1<<BIT_PLANE))
      new->Attribute[i] = WHITE_VALUE;
    else
      new->Attribute[i] = BLACK_VALUE;

  new->Attribute[16] = 0x01;  /* -VGA2- */ /* wrong for the ET4000 */
  new->Attribute[17] = OVERSCAN_VALUE;  /* -VGA2- */
#else
  new->Attribute[0]  = 0x00; /* standard colormap translation */
  new->Attribute[1]  = 0x01;
  new->Attribute[2]  = 0x02;
  new->Attribute[3]  = 0x03;
  new->Attribute[4]  = 0x04;
  new->Attribute[5]  = 0x05;
  new->Attribute[6]  = 0x06;
  new->Attribute[7]  = 0x07;
  new->Attribute[8]  = 0x08;
  new->Attribute[9]  = 0x09;
  new->Attribute[10] = 0x0A;
  new->Attribute[11] = 0x0B;
  new->Attribute[12] = 0x0C;
  new->Attribute[13] = 0x0D;
  new->Attribute[14] = 0x0E;
  new->Attribute[15] = 0x0F;
#ifdef XF86VGA16
  new->Attribute[16] = 0x81; /* wrong for the ET4000 */
  new->Attribute[17] = 0x00; /* GJA -- overscan. */
#else
  new->Attribute[16] = 0x41; /* wrong for the ET4000 */
#endif
  /*
   * Attribute[17] is the overscan, and is initalised above only at startup
   * time, and not when mode switching.
   */
#endif
  new->Attribute[18] = 0x0F;
  new->Attribute[19] = 0x00;
  new->Attribute[20] = 0x00;
#ifdef PC98_EGC
  outb (0x7c, 0x00);
  /* set to 16color mode */
  outb(0x6a, 0x01);
  /* set EGC enable */
  outb(0x7c, 0xc0); /* GRCG enable, RMW mode */
  outb(0x6a, 0x07);
  outb(0x6a, 0x05);
  outb(0x6a, 0x06);

     for (i = 0; i < 4 ; i++)
     {
             outb (0x7e,0xff);
     }

       /* set up VGA registers */

     outw (EGC_READ, 0x0000) ;
     outw (EGC_MASK, 0xffff) ;
     outw (EGC_READ ,0x40ff) ;
     outw (EGC_ADD, 0x0000) ;
     outw (EGC_LENGTH, 0x000f) ;
#endif /* PC98_EGC */
  return(TRUE);
}


/*
 * vgaGetClocks --
 *      get the dot-clocks via a BIG BAD hack ...
 */

void
vgaGetClocks(int num,
#if NeedNestedPrototypes
     Bool (*ClockFunc)(int))
#else
     Bool (*ClockFunc)())
#endif
{
  xf86GetClocks(num, ClockFunc, vgaProtect, vgaSaveScreen,
		(vgaIOBase + 0x0A), 0x08, 1, 28322, &vga256InfoRec);
}
