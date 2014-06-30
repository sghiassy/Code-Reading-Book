/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000init.c,v 3.15 1996/12/23 06:40:46 dawes Exp $ */
/*
 * Copyright 1994 Erik Nygren (nygren@mit.edu)
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Harry Langenbacher (harry@brain.jpl.nasa.gov)
 *
 * ERIK NYGREN AND HARRY LANGENBACHER
 * DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ERIK NYGREN
 * OR HARRY LANGENBACHER BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* $XConsortium: p9000init.c /main/11 1996/05/07 17:11:31 kaleb $ */

#include "X.h"
#include "input.h"

#include "xf86.h"
#include "xf86_OSlib.h"

#include "p9000.h"
#include "p9000reg.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

extern p9000CRTCRegRec  p9000CRTCRegs;

static short p9000Inited = FALSE;

int p9000InitCursorFlag = TRUE;

extern pointer vgaBase;

/* prototypes */

static void p9000ProbeMemConfig(
#if NeedFunctionPrototypes
   void
#endif
);

/*	
 * p9000CalcSysconfigHres --
 *    Calculates the horizontal resolution component of the sysconfig
 *    register.  Returns whether the horiz res is possible or not.
 *    Stores the value in sysconfigval.
 */
Bool p9000CalcSysconfigHres(hres, bytesperpixel, sysconfigval)
     int hres; 
     unsigned long bytesperpixel;
     unsigned long *sysconfigval;
{
  static struct p9000SysconfigHresFieldEntry {
    unsigned long fieldval;  /* The value to store in the field */
    int           add;       /* How much to add for this field */
    Bool          specin[3]; /* Which fields can this be spec'd in */
  } p9000SysconfigHresFields[] = {
    /* val  add     0      1      2  */
    { 0x7, 2048, {FALSE, FALSE, TRUE}  },
    { 0x6, 1024, {FALSE, TRUE,  TRUE}  },
    { 0x5, 512,  {TRUE,  TRUE,  TRUE}  },
    { 0x4, 256,  {TRUE,  TRUE,  TRUE}  },
    { 0x3, 128,  {TRUE,  TRUE,  TRUE}  },
    { 0x2, 64,   {TRUE,  TRUE,  FALSE} },
    { 0x1, 32,   {TRUE,  FALSE, FALSE} },
    { 0x0, 0,    {TRUE,  TRUE,  TRUE}  },
  };
  int p9000SysconfigHresFieldEntries = sizeof(p9000SysconfigHresFields)
    / sizeof(struct p9000SysconfigHresFieldEntry);
  int remhres;                     /* The remaining hres */
  unsigned long cursysconfig = 0;  /* The current version of sysconfig */
  int curfield, curfv;             /* The current field and field value */
  
  remhres = bytesperpixel*hres;

  /* NOTE - the p9000 manual figure 60 has the fields labeled
     backwards - but if you look at values from viper.ini
     you'll see that it's the left-most (msb) field that
     can have values up to 7 , so that must be field 3 . It
     doesn't work for rectangles if you mess this up, i.e. if
     you calculate a wrong number or you use a field value
     illegal for that field */

  for (curfield = 2; curfield >=0; curfield--)
    for (curfv = 0; curfv < p9000SysconfigHresFieldEntries; curfv++)
      if (p9000SysconfigHresFields[curfv].specin[curfield]
	  && (p9000SysconfigHresFields[curfv].add <= remhres))
	{
	  remhres -= p9000SysconfigHresFields[curfv].add;
	  cursysconfig |= 
	    p9000SysconfigHresFields[curfv].fieldval << (curfield*3 + 14);
	  break;
	}
  if (remhres)
    {
      ErrorF("\tIt is not possible to have a horiz resolution\n\tof %d at %d bytes per pixel.  Try a horizontal\n\tresolution of %d instead.\n",
	     hres, bytesperpixel, hres - remhres/bytesperpixel);
      return(FALSE);
    }
  *sysconfigval = cursysconfig;
  return(TRUE);
}


/*	
 * p9000CalcMiscRegs --
 *    Initializes the P9000 for the parameters which do not
 *    depend on video mode, such as memory size.
 */
void p9000CalcMiscRegs()
{
  /* figure out how much memory the board has */
  p9000ProbeMemConfig();
  if (p9000MiscReg.memconfig == 0x2L)	      /* If 2 Meg. Byte */
    p9000MiscReg.srtctl = 0x000001E5L;
  else
    p9000MiscReg.srtctl = 0x000001E4L;   /* else is 1 meg */
  if (xf86Verbose)
    ErrorF("%s %s: MemSize: 0x%x MemConfig: 0x%x\n",
	   XCONFIG_GIVEN, p9000InfoRec.name,
	   p9000MiscReg.memsize, p9000MiscReg.memconfig);
}


/*
 * p9000CalcCRTCRegs --
 *     Initializes the P9000 for the currently selected CRTC parameters.
 *     Thanks to Harry Langenbacher for figuring this out!
 */
void p9000CalcCRTCRegs(crtcRegs, mode)
     p9000CRTCRegPtr crtcRegs;
     DisplayModePtr mode;
{

  crtcRegs->XSize = mode->CrtcHDisplay;
  crtcRegs->YSize = mode->CrtcVDisplay;
  crtcRegs->BytesPerPixel = p9000InfoRec.bitsPerPixel / 8;
  
  /* Where the BytesPerPixel gets multiplied in SHOULD CHANGE!!! *TO*DO* */
  crtcRegs->hrzt  = (crtcRegs->BytesPerPixel) * (mode->CrtcHTotal / 4) - 1 ;
  crtcRegs->hrzsr = (crtcRegs->BytesPerPixel) * 
    ((mode->CrtcHSyncEnd - mode->CrtcHSyncStart) / 4 ) - 1 ;
  crtcRegs->hrzbr = (crtcRegs->BytesPerPixel) * 
    ((mode->CrtcHTotal - mode->CrtcHSyncStart) / 4 ) - 1 ;
  crtcRegs->hrzbf = (crtcRegs->BytesPerPixel) * 
    ((mode->CrtcHDisplay+(mode->CrtcHTotal-mode->CrtcHSyncStart)) /4) - 1 ;

  crtcRegs->vrtt  = mode->CrtcVTotal ;
  crtcRegs->vrtsr = mode->CrtcVSyncEnd - mode->CrtcVSyncStart ;
  crtcRegs->vrtbr = mode->CrtcVTotal - mode->CrtcVSyncStart ;
  crtcRegs->vrtbf = crtcRegs->vrtbr + mode->CrtcVDisplay ;
  crtcRegs->prehrzc = 0x0L;
  crtcRegs->prevrtc = 0x0L;

  if (mode->Flags & V_NHSYNC) crtcRegs->hp = SP_NEGATIVE;
  else crtcRegs->hp = SP_POSITIVE;

  if (mode->Flags & V_NVSYNC) crtcRegs->vp = SP_NEGATIVE;
  else crtcRegs->vp = SP_POSITIVE;

  if (mode->Flags & V_INTERLACE) crtcRegs->interlaced = TRUE;
  else crtcRegs->interlaced = FALSE;

  /* Calculate sysconfig */
  if (!p9000CalcSysconfigHres(mode->CrtcHDisplay, (int)crtcRegs->BytesPerPixel,
			      &crtcRegs->sysconfig))
    ErrorF("Bad horizontal resolution!!!!\n");
#ifdef DEBUG
  ErrorF("p9000CalcSysconfigHres returned 0x%lx\n", crtcRegs->sysconfig);
#endif
  crtcRegs->sysconfig |= 0x00003000;

  /* This is set to the default for now. *TO*DO* */
  crtcRegs->memspeed = MEMSPEED;
  /* Get the dot clock */
  crtcRegs->dotfreq = mode->SynthClock * 1000;

#ifdef DEBUG
  ErrorF("The value of mode->Clock is %d.  sysconfig is %lx. Dotfreq is %ld\n",
	 mode->Clock,crtcRegs->sysconfig,crtcRegs->dotfreq);
  ErrorF("Using %dx%d screen\n",mode->HDisplay,mode->VDisplay);
  ErrorF("HRZT %lx  HRZSR %lx  HRZBR %lx   HRZBF %lx   \nVRTSR %lx  VRTBR %lx   VRTBF %lx  VRTT: %lx\n",
	 crtcRegs->hrzt, crtcRegs->hrzsr, crtcRegs->hrzbr, crtcRegs->hrzbf,
	 crtcRegs->vrtsr,
	 crtcRegs->vrtbr, crtcRegs->vrtbf, crtcRegs->vrtt);
#endif
}


/* 
 * p9000SetCRTCRegs --
 *     Initializes the p9000 for the currently selected CRTC parameters.
 */
void p9000SetCRTCRegs(crtcRegs)
     p9000CRTCRegPtr crtcRegs;
{
  unsigned long ClipMax;

  /* System Configuration (width, byte order) */
  p9000Store(SYSCONFIG,CtlBase,crtcRegs->sysconfig);

  /* Disable interrupts from P9000 */
  p9000Store(INTERRUPT_EN,CtlBase,0x00000080L);

  /* Write Horizontal Timing Registers */
  p9000Store(PREHRZC,CtlBase, crtcRegs->prehrzc);
  p9000Store(HRZSR,CtlBase, crtcRegs->hrzsr);	/* HRZSR */
  p9000Store(HRZBR,CtlBase, crtcRegs->hrzbr);	/* HRZBR */
  p9000Store(HRZBF,CtlBase, crtcRegs->hrzbf);	/* HRZBF */
  p9000Store(HRZT,CtlBase, crtcRegs->hrzt);	/* HRZT  */

  /* Write Vertical Timing Registers */
  p9000Store(PREVRTC,CtlBase, crtcRegs->prevrtc); 
  p9000Store(VRTSR,CtlBase, crtcRegs->vrtsr);	/* VRTSR */
  p9000Store(VRTBR,CtlBase, crtcRegs->vrtbr);	/* VRTBR */
  p9000Store(VRTBF,CtlBase, crtcRegs->vrtbf);	/* VRTBF */
  p9000Store(VRTT,CtlBase, crtcRegs->vrtt);	/* VRTT  */

  /* Memory Configuration */
  p9000Store(MEM_CONFIG,CtlBase, p9000MiscReg.memconfig);

  p9000Store(RFPERIOD,CtlBase, 0x00000186L);
  p9000Store(RLMAX,CtlBase, 0x000000FAL);

  /* Enable writing to all 8 planes */
  p9000Store(PMASK,CtlBase,0x000000FFL);
  /* Select buffer 1 and allow drawing in window */
  p9000Store(DRAW_MODE,CtlBase,0x0000000AL);
  /* Disable any window (coord) offset */
  p9000Store(W_MIN,CtlBase,0x00000000L);
  /* Set the pattern origin to 0,0 */
  p9000Store(PAT_ORIGINX,CtlBase,0x00000000L);
  p9000Store(PAT_ORIGINY,CtlBase,0x00000000L); 

  /* Calulate and set registers for clipping */
  ClipMax= ((long)(crtcRegs->XSize*crtcRegs->BytesPerPixel)-1)<<16
    | ( p9000MiscReg.memsize / (crtcRegs->XSize*crtcRegs->BytesPerPixel) - 1);
  p9000Store(W_MIN,CtlBase,0);        /* Minimum Clipping */
  p9000Store(W_MAX,CtlBase,ClipMax);  /* Maximum Clipping */
#ifdef DEBUG
  ErrorF("ClipMax set to 0x%lx\n",ClipMax);
#endif

  /* Clear the screen the first time before enabling video */
  if (!p9000Inited)
    p9000ClearScreen();

  /* Enable video (1e5, 1e4, or 1c4) */
  p9000Store(SRTCTL,CtlBase, p9000MiscReg.srtctl);

#ifdef DEBUG
  ErrorF("About to set the clock\n");
#endif
  p9000VendorPtr->SetClock(crtcRegs->dotfreq, crtcRegs->memspeed);

  p9000InitCursorFlag = TRUE;
  p9000Inited = TRUE;
}


/* 
 * p9000InitAperture --
 *     Initialize the aperture for the p9000.
 */
void p9000InitAperture(screen_idx)
    int screen_idx;
{
  /* Map standard VGA in for saving and restoration */
  vgaBase = xf86MapVidMem(screen_idx, VGA_REGION, (pointer)0xA0000,
			  0x10000);  /* 64k Banks */

  /* Map the P9000 in */
  p9000VideoMem = xf86MapVidMem(screen_idx, LINEAR_REGION, 
				(pointer)(p9000InfoRec.MemBase),
				0x400000L);  /* 4 Megabytes */
  
  /* the start of video memory */
  VidBase = (volatile unsigned long *)((unsigned char *)p9000VideoMem +
				       0x200000L);
  /* Point It Past Non-P9000 Area Into Linear Control Region */
  CtlBase = (volatile unsigned long *)((unsigned char *)p9000VideoMem +
				       0x100000L); 

#ifdef DEBUG
  ErrorF("P9000 address space mapped to BASE: %lx  Vid: %lx  Ctl: %lx\n",
	 (long)p9000VideoMem, (long)VidBase,
	 (long)CtlBase);
#endif
}


/*
 * p9000CleanUp --
 *    Disables the P9000 and restores things to text mode.
 */
void
p9000CleanUp()
{
    /*disable video in the video controller */
    p9000Store(SRTCTL,CtlBase,0x01C4L); 

    /* Disable the P9000 and switch back to good old VGA */
    p9000VendorPtr->Disable();

    /* Restore the VT's LUT among other things */
    p9000RestoreVT();
}

/* 
 * p9000InitEnvironment --
 *    Initializes the P9000's drawing environment and clears the display.
 */
void p9000InitEnvironment()
{
    if (serverGeneration == 1)
    {
        /* Clear the display.  Need to set the color, origin, and size.
         * Then draw.
         */
      /* p9000ClearScreen(); */
    }

    /* Load the LUT */
    p9000InitLUT();
}


/* 
 * p9000ClearScreen --
 *     Clears video memory
 */
void  p9000ClearScreen(void)
{
  p9000NotBusy();         /* Wait for the P9000 to be free */
  /* Drawing a big black rectangle is probably a good way to clear things */
  if (p9000CRTCRegs.BytesPerPixel == 1)
    p9000Store(FGROUND, CtlBase, 0); /* 8 bit */
  else
    p9000Store(FGROUND, CtlBase, 0); /* 16 and 32 bit */
  p9000Store(RASTER, CtlBase, IGM_F_MASK);
  p9000Store(META_COORD | MC_QUAD | MC_YX, CtlBase,
	     YX_PACK(0,0));
  p9000Store(META_COORD | MC_QUAD | MC_YX, CtlBase,
	     YX_PACK((p9000CRTCRegs.XSize)*p9000CRTCRegs.BytesPerPixel,0));
  p9000Store(META_COORD | MC_QUAD | MC_YX, CtlBase,
	     YX_PACK((p9000CRTCRegs.XSize)*p9000CRTCRegs.BytesPerPixel,
		     p9000CRTCRegs.YSize));
  p9000Store(META_COORD | MC_QUAD | MC_YX, CtlBase,
	     YX_PACK(0, p9000CRTCRegs.YSize));
  while (p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN) ;
  /* Wait for the engine to be free again */
  p9000QBNotBusy();
  return;
}


/*
 * p9000ProbeMemConfig --
 *     Runs tests on the memory to determine which memory configuration
 *     is being used on the video card
 */
/* NOTE: This has not been well tested since I only have a 2MB card
 * and haven't found any people with 1 meg cards... */
void p9000ProbeMemConfig()
{

#ifdef DO_BROKEN_P9000_MEM_PROBE  /* This doesn't work, apparently */
  unsigned long memctr;

  /* Assume that we have 2 megs.  If this is false, errors
   * will occur and we can detect the type of memory
   * by looking at the errors.
   */
  p9000Store(MEM_CONFIG, CtlBase, 0x2L);

  /* Write something into a non-first row */
  p9000Store(0x100000, VidBase, 0x5a5a5a5a);

  /* Load values into all the memory */
  for (memctr = 0L; memctr < 1024L; memctr++)
    p9000Store(memctr*4L, VidBase, ~(memctr*4L));

  /* If there's an error in the first 32 longwords, 
   * we must have one bank of 8 256Kx4 VRAM's.
   * This is 1MB. */
  for (memctr = 0L; memctr < 32L; memctr++)
    if (p9000Fetch(memctr*4L, VidBase) != ~(memctr*4L))
      {
	p9000MiscReg.memconfig = 0x0L;
	p9000MiscReg.memsize = 0x100000L;
	p9000Store(MEM_CONFIG, CtlBase, p9000MiscReg.memconfig);
	return;
      }

  /* If there's an error after the 512th longword, 
   * we must have two banks of 8 128Kx8 VRAM's.
   * This is 1MB. */
  for (memctr = 32L; memctr < 1024L; memctr++)
    if (p9000Fetch(memctr*4L, VidBase) != ~(memctr*4L))
      {
	p9000MiscReg.memconfig = 0x1L;
	p9000MiscReg.memsize = 0x100000L;
	p9000Store(MEM_CONFIG, CtlBase, p9000MiscReg.memconfig);
	return;
      }

  /* Otherwise we guessed right and really do have 2MB of VRAM. 
   * (unless probe failed and we're overridden) */
  if (p9000InfoRec.videoRam == 1024) /* Allow user to override */
    {
      if (OFLG_ISSET(OPTION_VRAM_128, &p9000InfoRec.options))
	p9000MiscReg.memconfig = 0x1L;  /* 128Kx8 VRAM */
      else  
	p9000MiscReg.memconfig = 0x0L;  /* 256Kx4 VRAM */
      p9000MiscReg.memsize = 0x100000L;
      p9000Store(MEM_CONFIG, CtlBase, p9000MiscReg.memconfig);      
    }
  else  /* Go with the probed result */
    {
      p9000MiscReg.memconfig = 0x2L;
      p9000MiscReg.memsize = 0x200000L;
      p9000Store(MEM_CONFIG, CtlBase, p9000MiscReg.memconfig);
    }
#else /* not DO_BROKEN_P9000_MEM_PROBE */

  /* Set the memory parameters as explicitly requested */
  if (p9000InfoRec.videoRam == 1024) /* Allow user to override */
    {
      if (OFLG_ISSET(OPTION_VRAM_128, &p9000InfoRec.options))
	p9000MiscReg.memconfig = 0x1L;  /* 128Kx8 VRAM */
      else  
	p9000MiscReg.memconfig = 0x0L;  /* 256Kx4 VRAM */
      p9000MiscReg.memsize = 0x100000L;
    }
  else  /* Go with the probed result */
    {
      p9000MiscReg.memconfig = 0x2L;
      p9000MiscReg.memsize = 0x200000L;
    }
  p9000Store(MEM_CONFIG, CtlBase, p9000MiscReg.memconfig);      
#endif

}
