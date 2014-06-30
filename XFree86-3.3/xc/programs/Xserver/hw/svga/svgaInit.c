/* $XConsortium: svgaInit.c,v 1.8 93/09/26 12:26:33 rws Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "svga.h"
#include "servermd.h"
#include "mi.h"
#include "mipointer.h"
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <sys/kd.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <sys/stat.h>
#include <sys/sysi86.h>
#include <sys/tss.h>
#include <sys/v86.h>
#include <sys/vt.h>

extern char *display;
extern Bool mieqInit();
extern void ProcessInputEvents();

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

char *svgaconfig = SVGACONFIG;

static const PixmapFormatRec svgaFormats[] = {
  {  1,  1, BITMAP_SCANLINE_PAD },
  {  8,  8, BITMAP_SCANLINE_PAD },
};

#define NUMFORMATS (sizeof(svgaFormats) / sizeof(svgaFormats[0]))

int svgaConsoleFd = -1;
static int svgaDefaultMode = -1;
static int svgaVTNo = -1;

/* ARGSUSED */

static void
svgaWakeup(
    pointer blockData,
    int     err,
    pointer pReadmask
)
{
  if (svgaInputPending) ProcessInputEvents();
  if (svgaVTRequestsPending) svgaVTSwitch();
}

void
OsVendorInit()
{
  FILE             *fp;
  int              fd, pid;
  char             fname[PATH_MAX];
  struct vt_mode   VT;
  static Bool      inited = FALSE;
  static struct    kd_quemode xqueMode;

  if (!inited)
    {
      inited = TRUE;

      /*
       * USL compatibilty stuff ...
       */
      sprintf(fname, "/dev/X/server.%s.pid", display);

      /*
       * First check whether there is such a file, and if so check
       * for the server whether it is running, or just a dead file ...
       */
      if ((fp = fopen(fname, "r"))) {
	fscanf(fp, "%d", &pid);

	if (kill(pid, 0) == 0) {
	  sprintf(fname, "/dev/X/server.%s", display);
	  if (open(fname, O_RDWR) != -1) {
	    ErrorF("Server on display %s is already running\n", display);
	    exit(1);
	  }
	}
      }

      /*
       * Ok, no other server is running, just setup all files for this
       * new one ....
       */
      fclose(fp);
      sprintf(fname, "/dev/X/Nserver.%s",    display); unlink(fname);
      sprintf(fname, "/dev/X/server.%s",     display); unlink(fname);
      sprintf(fname, "/dev/X/server.%s.pid", display); unlink(fname);

      if (!mkdir("/dev/X", 0777)) chmod("/dev/X", 0777);

      if ((fp = fopen(fname, "w"))) {
	fprintf(fp, "%-5ld\n", getpid());
	fclose(fp);
      }
      else
	ErrorF("Cannot write to %s\n", fname);

      /*
       * Get the correct VT number once at startup time
       */
      if (svgaVTNo == -1){
	if ((fd = open("/dev/console",O_WRONLY | O_NOCTTY)) <0) 
	  FatalError("Cannot open /dev/console\n");

	if (ioctl(fd, VT_OPENQRY, &svgaVTNo) < 0 || svgaVTNo == -1) 
	  FatalError("Cannot find a free VT\n");

	close(fd);
      }

      sprintf(fname, "/dev/vt%02d", svgaVTNo);

      fclose(stdin);
      fclose(stdout);

      /*
       * For XQUEUE device we MUST force a new process group in order to
       * let the kd driver be attached to the mouse device ...
       */
      setpgrp ();

      if ((svgaConsoleFd = open(fname, O_RDWR | O_NONBLOCK)) < 0)
	FatalError ("Couldn't open %s\n", fname);

      if ((svgaDefaultMode = ioctl(svgaConsoleFd, CONS_GET, 0)) < 0)
	FatalError("CONS_GET failed\n");

      if (ioctl(svgaConsoleFd, VT_GETMODE, &VT) < 0) 
        FatalError ("VT_GETMODE failed\n");

      VT.mode = VT_PROCESS;
      VT.relsig = SIGUSR2;
      VT.acqsig = SIGUSR2;

      if (ioctl(svgaConsoleFd, VT_SETMODE, &VT) < 0) 
	FatalError ("VT_SETMODE VT_PROCESS failed\n");

      if (ioctl(svgaConsoleFd, KDSETMODE, KD_GRAPHICS) < 0)
	FatalError ("KDSETMODE KD_GRAPHICS failed\n");

      if (open("/dev/mouse", O_RDONLY | O_NONBLOCK) < 0)
	FatalError("Cannot open /dev/mouse\n");
  
      xqueMode.qsize = 64;    /* max events */
      xqueMode.signo = SIGUSR1;
      if (ioctl(svgaConsoleFd, KDQUEMODE, &xqueMode) < 0)
	FatalError("Cannot set KDQUEMODE\n");
      
      XqueQaddr = (xqEventQueue *)xqueMode.qaddr;
      XqueQaddr->xq_sigenable = 0; /* LOCK */
    }

  /*
   * Get the IOPL for EVERY port
   */
  if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) == -1) {
    FatalError("Failed to aquire IO permissions\n");
  }
}

/*
 * InitOutput --
 *	Initialize screenInfo for all actually accessible framebuffers.
 *      That includes vt-manager setup, querying all possible devices and
 *      collecting the pixmap formats.
 */

void
InitOutput(
    ScreenInfo *pScreenInfo,
    int        argc,
    char       **argv
)
{
  int i;
  
  /*
   * now force to get the VT
   */
  if (ioctl(svgaConsoleFd, VT_ACTIVATE, svgaVTNo) != 0)
    ErrorF("VT_ACTIVATE failed\n");
  if (ioctl(svgaConsoleFd, VT_WAITACTIVE, svgaVTNo) != 0)
    ErrorF("VT_WAITACTIVE failed\n");

  svgaVTActive = TRUE;

  /*
   * Use the previous collected parts to setup pScreenInfo
   */
  pScreenInfo->imageByteOrder = IMAGE_BYTE_ORDER;
  pScreenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
  pScreenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
  pScreenInfo->bitmapBitOrder = BITMAP_BIT_ORDER;
  pScreenInfo->numPixmapFormats = NUMFORMATS;
  for ( i=0; i < NUMFORMATS; i++ ) pScreenInfo->formats[i] = svgaFormats[i];

  AddScreen(svgaSVGAScreenInit, argc, argv);

  RegisterBlockAndWakeupHandlers((BlockHandlerProcPtr)NoopDDA,
				 svgaWakeup, (void *)0);
}



/*
 * InitInput --
 *      Initialize all supported input devices...what else is there
 *      besides pointer and keyboard? Two DeviceRec's are allocated and
 *      registered as the system pointer and keyboard devices.
 */

/* ARGSUSED */
void
InitInput(
    int  argc,
    char **argv
)
{
  DevicePtr pKeyboard, pPointer;

  svgaVTRequestsPending = FALSE;

  (void) OsSignal(SIGUSR1, XqueRequest);
  (void) OsSignal(SIGUSR2, svgaVTRequest);
  
  pKeyboard = AddInputDevice(XqueKeyboardProc, TRUE); 
  pPointer  = AddInputDevice(XquePointerProc, TRUE);

  if (pKeyboard && pPointer) {
    RegisterKeyboardDevice(pKeyboard); 
    RegisterPointerDevice(pPointer); 

    miRegisterPointerDevice(screenInfo.screens[0], pPointer);
    mieqInit(pKeyboard, pPointer);
  }

  svgaInputPending = FALSE;
}



/*
 * ddxGiveUp --
 *      Device dependent cleanup. Called by by dix before normal server death.
 *      For SVR4.2 we must switch the terminal back to normal mode. No error-
 *      checking here, since there should be restored as much as possible.
 */

void
ddxGiveUp()
{
  struct vt_mode VT;
  char           fname[PATH_MAX];

  /*
   * If we are on the active VT we have to restore text mode correctly,
   * hence leave all graphics modes and unmap everything ...
   */

  (void)ioctl(svgaConsoleFd, KDQUEMODE, NULL);
  (void)ioctl(svgaConsoleFd, VT_ACTIVATE, svgaVTNo);
  (void)ioctl(svgaConsoleFd, VT_WAITACTIVE, 0);

  (void)ioctl(svgaConsoleFd, MODESWITCH | svgaDefaultMode, 0);
  (void)ioctl(svgaConsoleFd, KDSETMODE, KD_TEXT0);

  if (ioctl(svgaConsoleFd, VT_GETMODE, &VT) != -1) {
    VT.mode = VT_AUTO;
    (void)ioctl(svgaConsoleFd, VT_SETMODE, &VT);
  }

  /*
   * Garbage collection ...
   */
  sprintf(fname, "/dev/X/Nserver.%s",    display); unlink(fname);
  sprintf(fname, "/dev/X/server.%s",     display); unlink(fname);
  sprintf(fname, "/dev/X/server.%s.pid", display); unlink(fname);

  close(svgaConsoleFd);                    /* make the vt-manager happy */
}



/*
 * AbortDDX --
 *      DDX - specific abort routine.  Called by AbortServer(). The attempt is
 *      made to restore all original setting of the displays. Also all devices
 *      are closed.
 */

void
AbortDDX()
{
  DevicePtr pKeyboard, pPointer;

  /*
   * try to deinitialize all input devices
   */
  if ((pKeyboard = LookupKeyboardDevice()) != NULL)
    (void)XqueKeyboardProc((DeviceIntPtr)pKeyboard, DEVICE_CLOSE);
  if ((pPointer = LookupPointerDevice()) != NULL)
    (void)XquePointerProc((DeviceIntPtr)pPointer, DEVICE_CLOSE);

  if (screenInfo.screens[0]) svgaSVPMISetText(screenInfo.screens[0]);

  /*
   * This is needed for a abnormal server exit, since the normal exit stuff
   * MUST also be performed (i.e. the vt must be left in a defined state)
   */

  ddxGiveUp();
}


/*
 * ddxProcessArgument --
 *	Process device-dependent command line args. Returns 0 if argument is
 *      not device dependent, otherwise Count of number of elements of argv
 *      that are part of a device dependent commandline option.
 */

/* ARGSUSED */
int
ddxProcessArgument(
    int  argc,
    char *argv[],
    int  i
)
{
  if (!strncmp(argv[i], "vt", 2)) {
    if (sscanf(argv[i] +2, "%d", &svgaVTNo) != 1) {
      ErrorF("bad argument \"%s\"\n", argv[i]);
      return 0;
    }
    return 1;
  }
  else if (!strcmp(argv[i], "-config")) {
    if (i + 1 < argc)
      svgaconfig = argv[i + 1];
    return 2;
  }

  return 0;
}


/*
 * ddxUseMsg --
 *	Print out correct use of device dependent commandline options.
 *      Maybe the user now knows what really to do ...
 */

void
ddxUseMsg()
{
  ErrorF("\n");
  ErrorF("\n");
  ErrorF("Device Dependent Usage:\n");
  ErrorF("\n");
  ErrorF("vtxx                   server should use virtual terminal xx\n");
  ErrorF("-config \"PMIFILE:XxY\"  configuration\n");
  ErrorF("\n");
}
