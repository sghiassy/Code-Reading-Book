/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/ramdac.c,v 3.14.2.1 1997/05/16 11:35:12 hohndel Exp $ */ 
/*
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
 */
/* $XConsortium: ramdac.c /main/12 1996/10/24 07:10:32 kaleb $ */

#include "X.h"
#include "Xproto.h"
#include "windowstr.h"
#include "compiler.h"

#include "xf86.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#include "xf86.h"
#include "vga.h"
#include "w32.h"
#include "xf86_Config.h"

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];
				/* current colormap for each screen */

static BOOL generic_ramdac;

static SymTabRec W32DacTable[] = {
   { NORMAL_DAC,         "normal" },
   { ATT20C47xA_DAC,     "att20c47xa" },
   { Sierra1502X_DAC,    "sc1502x" },
   { ATT20C497_DAC,      "att20c497" },
   { ATT20C490_DAC,      "att20c490" },
   { ATT20C493_DAC,      "att20c493" },
   { ATT20C491_DAC,      "att20c491" },
   { ATT20C492_DAC,      "att20c492" },
   { ICS5341_DAC,        "ics5341" },
   { GENDAC_DAC,         "gendac" },
   { STG1700_DAC,	 "stg1700" },
   { STG1702_DAC,        "stg1702" },
   { STG1703_DAC,	 "stg1703" },
   { ET6000_DAC,	 "et6000" },
   { CH8398_DAC,	 "ch8398" },
   { -1,                "" },
};


int
vgaListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}


static void
set_overscan(overscan)
    int overscan;
{
    inb(0x3da);
    GlennsIODelay();
    outb(0x3c0, 0x11);
    GlennsIODelay();
    outb(0x3c0, overscan);
    GlennsIODelay();
    inb(0x3da);
    GlennsIODelay();
    outb(0x3c0, 0x20);
    GlennsIODelay();
} 


#define RMR 0x3c6
#define READ 0x3c7
#define WRITE 0x3c8
#define RAM 0x3c9


static void
write_color(entry, cmap)
    int entry;
    unsigned char *cmap;
{
    outb(WRITE, entry);
    GlennsIODelay();
    outb(RAM, cmap[0]);
    GlennsIODelay();
    outb(RAM, cmap[1]);
    GlennsIODelay();
    outb(RAM, cmap[2]);
    GlennsIODelay();
} 


static void
read_color(entry, cmap)
    int entry;
    unsigned char *cmap;
{
    outb(READ, entry);
    GlennsIODelay();
    cmap[0] = inb(RAM);
    GlennsIODelay();
    cmap[1] = inb(RAM);
    GlennsIODelay();
    cmap[2] = inb(RAM);
    GlennsIODelay();
}


static W32Blanked = FALSE;

static unsigned char black_cmap[] = {0x0, 0x0, 0x0};
static unsigned char white_cmap[] = {0xff, 0xff, 0xff};

void
vgaStoreColors(pmap, ndef, pdefs)
     ColormapPtr	pmap;
     int		ndef;
     xColorItem	        *pdefs;
{
    int		i;
    unsigned char *cmap, *tmp;
    xColorItem	directDefs[256];
    Bool          new_overscan = FALSE;
    unsigned char overscan = ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN];
    unsigned char old_overscan; 
    unsigned char tmp_overscan;
   
    extern RamdacShift;

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
        return;

    /* GJA -- We don't want cfb code right now (in vga16 server) */
    if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    {
        ndef = cfbExpandDirectColors (pmap, ndef, pdefs, directDefs);
        pdefs = directDefs;
    }

    for(i = 0; i < ndef; i++)
    {
        if (pdefs[i].pixel == overscan)
	{
	    old_overscan = overscan; 
	    new_overscan = TRUE;
	}
        cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[pdefs[i].pixel*3];
        cmap[0] = pdefs[i].red   >> RamdacShift;
        cmap[1] = pdefs[i].green >> RamdacShift;
        cmap[2] = pdefs[i].blue  >> RamdacShift;

        if (xf86VTSema && (!W32Blanked || pdefs[i].pixel != overscan))
	{
	    outb(0x3C8, pdefs[i].pixel);
	    GlennsIODelay();
	    outb(0x3C9, cmap[0]);
	    GlennsIODelay();
	    outb(0x3C9, cmap[1]);
	    GlennsIODelay();
	    outb(0x3C9, cmap[2]);
	    GlennsIODelay();
	}
    }	
    if (new_overscan)
    {
	new_overscan = FALSE;
        for(i = 0; i < ndef; i++)
        {
            if (pdefs[i].pixel == overscan)
	    {
	        if ((pdefs[i].red != 0) || 
	            (pdefs[i].green != 0) || 
	            (pdefs[i].blue != 0))
	        {
	            new_overscan = TRUE;
		    tmp_overscan = overscan;
        	    tmp = &((vgaHWPtr)vgaNewVideoState)->DAC[pdefs[i].pixel*3];
	        }
	        break;
	    }
        }
        if (new_overscan)
        {
            /*
             * Find a black pixel, or the nearest match.
             */
            for (i=255; i >= 0; i--)
	    {
                cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[i*3];
	        if ((cmap[0] == 0) && (cmap[1] == 0) && (cmap[2] == 0))
	        {
	            overscan = i;
	            break;
	        }
	        else
	        {
/* CHANGE to SUMMATION--GGLGGL */ 
	            if ((cmap[0] < tmp[0]) && 
		        (cmap[1] < tmp[1]) && (cmap[2] < tmp[2]))
	            {
		        tmp = cmap;
		        tmp_overscan = i;
	            }
	        }
	    }
	    if (i < 0)
	    {
	        overscan = tmp_overscan;
	    }
	    ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN] = overscan;
            if (xf86VTSema)
	    {
		if (W32Blanked)
		    write_color(overscan, black_cmap);

	        (void)inb(vgaIOBase + 0x0A);
		GlennsIODelay();
	        outb(0x3C0, OVERSCAN);
		GlennsIODelay();
	        outb(0x3C0, overscan);
		GlennsIODelay();
	        (void)inb(vgaIOBase + 0x0A);
		GlennsIODelay();
	        outb(0x3C0, 0x20);
		GlennsIODelay();

		if (W32Blanked)
		{
		    cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[old_overscan * 3];
		    write_color (old_overscan, cmap);
		}
	    }
        }
    }
}


void
vgaInstallColormap(pmap)
     ColormapPtr	pmap;
{
  ColormapPtr oldmap = InstalledMaps[pmap->pScreen->myNum];
  int         entries;
  Pixel *     ppix;
  xrgb *      prgb;
  xColorItem *defs;
  int         i;


  if (pmap == oldmap)
    return;

  if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    entries = (pmap->pVisual->redMask |
	       pmap->pVisual->greenMask |
	       pmap->pVisual->blueMask) + 1;
  else
    entries = pmap->pVisual->ColormapEntries;

  ppix = (Pixel *)ALLOCATE_LOCAL( entries * sizeof(Pixel));
  prgb = (xrgb *)ALLOCATE_LOCAL( entries * sizeof(xrgb));
  defs = (xColorItem *)ALLOCATE_LOCAL(entries * sizeof(xColorItem));

  if ( oldmap != NOMAPYET)
    WalkTree( pmap->pScreen, TellLostMap, &oldmap->mid);

  InstalledMaps[pmap->pScreen->myNum] = pmap;

  for ( i=0; i<entries; i++) ppix[i] = i;

  QueryColors( pmap, entries, ppix, prgb);

  for ( i=0; i<entries; i++) /* convert xrgbs to xColorItems */
    {
      defs[i].pixel = ppix[i];
      defs[i].red = prgb[i].red;
      defs[i].green = prgb[i].green;
      defs[i].blue = prgb[i].blue;
      defs[i].flags =  DoRed|DoGreen|DoBlue;
    }

  vgaStoreColors( pmap, entries, defs);

  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
  
  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}


void
vgaUninstallColormap(pmap)
     ColormapPtr pmap;
{
  ColormapPtr defColormap;
  
  if ( pmap != InstalledMaps[pmap->pScreen->myNum] )
    return;

  defColormap = (ColormapPtr) LookupIDByType( pmap->pScreen->defColormap,
					      RT_COLORMAP);

  if (defColormap == InstalledMaps[pmap->pScreen->myNum])
    return;

  (*pmap->pScreen->InstallColormap) (defColormap);
}


/*
 *    The power saver is for w32p_rev_c and later only--GGL
 */
Bool
W32SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
    unsigned char state;
    unsigned char *cmap;
    unsigned char overscan; 

    if (on)
	SetTimeSinceLastInputEvent();
    if (xf86VTSema)
    {
	outb(vgaIOBase + 4, 0x34);
	state = inb(vgaIOBase + 5);
  
	if (on)
	{
	    state &= ~0x21;
	    W32Blanked = FALSE; 

	    overscan = ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN];
	    cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[overscan * 3];

	    inb(vgaIOBase + 0x0A);
	    GlennsIODelay();
	    outb(0x3C0, 0x20);         
	    GlennsIODelay();

	    write_color(overscan, cmap);

#ifdef DPMSExtension
	    if (DPMSEnabled && W32pCAndLater)
	    {
		outb(vgaIOBase + 4, 0x34);
		GlennsIODelay();
		outb(vgaIOBase + 5, state);
		GlennsIODelay();
	    }
#endif
	}
	else
	{
	    state |= 0x21;
	    W32Blanked = TRUE; 
    
	    overscan = ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN];
	    write_color(overscan, black_cmap);

	    inb(vgaIOBase + 0x0A);
	    GlennsIODelay();
	    outb(0x3C0, 0x00);         
	    GlennsIODelay();

#ifdef DPMSExtension
	    if (DPMSEnabled && W32pCAndLater)
	    {
		outb(vgaIOBase + 4, 0x34);
		GlennsIODelay();
		outb(vgaIOBase + 5, state);
		GlennsIODelay();
	    }
#endif
	}
    }
    return(TRUE);
}

int W32RamdacType = UNKNOWN_DAC;

static Bool
ProbeSTG1703(Bool quiet)
{
	unsigned char cid, did, daccomm, readmask;
	int i;
	Bool Found = FALSE;

	readmask = inb(0x3c6);
	xf86dactopel();
	daccomm = xf86getdaccomm();
	xf86setdaccomm(daccomm | 0x10);
	xf86dactocomm();
	inb(0x3C6);
	outb(0x3c6, 0x00);
	outb(0x3c6, 0x00);
	cid = inb(0x3c6);     /* company ID */
	did = inb(0x3c6);     /* device ID */
	xf86dactopel();
	outb(0x3c6,readmask);
	xf86setdaccomm(daccomm);

	if (cid == 0x44) { /* STG170x found */
	   Found = TRUE;
	   switch (did) {
	      case 0x02:
		W32RamdacType = STG1702_DAC;
		break;
	      case 0x03:
		W32RamdacType = STG1703_DAC;
		break;
	      case 0x00:
	      default: /* Unknown STG170x - treat as 1700. */
		W32RamdacType = STG1700_DAC;
	   }
	}

	return(Found);
}

static Bool
ProbeGenDAC(Bool quiet)
{
   /* probe for ICS GENDAC (ICS5341) */
   /*
    * GENDAC and SDAC have two fixed read only PLL clocks
    *     CLK0 f0: 25.255MHz   M-byte 0x28  N-byte 0x61
    *     CLK0 f1: 28.311MHz   M-byte 0x3d  N-byte 0x62
    * which can be used to detect GENDAC and SDAC since there is no chip-id
    * for the GENDAC.
    *
    * code was taken from S3 XFree86 driver.
    * NOTE: for the GENDAC on a ET4000W32p, reading PLL values
    * for CLK0 f0 and f1 always returns 0x7f (but is documented "read only")
    * In fact, all "read only" registers return 0x7f
    */
   
   unsigned char saveCR31, savelut[6];
   int i;
   long clock01, clock23;
   Bool found = FALSE;
   unsigned char dbyte=0;
   float mclk=0.0;

   outb(vgaIOBase + 4, 0x31);
   saveCR31 = inb(vgaIOBase + 5);

   outb(vgaIOBase + 5, saveCR31 & ~0x40);
   
   outb(0x3c7,0);
   for(i=0; i<2*3; i++)		/* save first two LUT entries */
      savelut[i] = inb(0x3c9);
   outb(0x3c8,0);
   for(i=0; i<2*3; i++)		/* set first two LUT entries to zero */
      outb(0x3c9,0);

   outb(vgaIOBase + 4, 0x31);
   outb(vgaIOBase + 5, saveCR31 | 0x40);

   outb(0x3c7,0);
   for(i=clock01=0; i<4; i++)
      clock01 = (clock01 << 8) | (inb(0x3c9) & 0xff);
   for(i=clock23=0; i<4; i++)
      clock23 = (clock23 << 8) | (inb(0x3c9) & 0xff);

   /* get MClk value */     
   outb(0x3c7,0x0a);
   mclk = (inb(0x3c9)+2)*14.31818;
   dbyte = inb(0x3c9);
   mclk /= (((dbyte & 0x1f)+2) * 1<<((dbyte & 0x60)>>5));

   outb(vgaIOBase + 4, 0x31);
   outb(vgaIOBase + 5, saveCR31 & ~0x40);

   outb(0x3c8,0);
   for(i=0; i<2*3; i++)		/* restore first two LUT entries */
      outb(0x3c9,savelut[i]);

   outb(vgaIOBase + 4, 0x31);
   outb(vgaIOBase + 5, saveCR31);

   if ( clock01 == 0x28613d62 ||
       (clock01 == 0x7f7f7f7f && clock23 != 0x7f7f7f7f)) {
      found = TRUE;

      xf86dactopel();
      inb(0x3c6);
      inb(0x3c6);
      inb(0x3c6);

      dbyte = inb(0x3c6);
      /* the fourth read will show the SDAC chip ID and revision */
      if ((dbyte & 0xf0) == 0xb0) {
         if (!quiet) {
	    ErrorF("%s %s: Ramdac: ICS 5341 GenDAC ,and programmable clock (MClk = %1.2f MHz)\n",
		   XCONFIG_PROBED, vga256InfoRec.name, mclk);
	 }
         W32RamdacType = ICS5341_DAC;
      } else {
         if (!quiet) {
	    ErrorF("%s %s: Ramdac: unknown GENDAC and programmable clock (ID code = 0x%02x)\n",
		   XCONFIG_PROBED, vga256InfoRec.name, dbyte);
	 }
	 W32RamdacType = GENDAC_DAC;
      }
      xf86dactopel();
   }
   return found;
}


static Bool
ProbeCH8398(Bool quiet)  
{  
    unsigned char cid;   
    Bool Found = FALSE;  
   
    xf86dactopel();      
    cid = inb(RMR);
    cid = inb(RMR);
    cid = inb(RMR);
    cid = inb(RMR);  /* this returns chip ID */
    if (cid == 0xc0) {   
       Found = TRUE;     
       W32RamdacType = CH8398_DAC;
    }
    xf86dactopel();

    return Found;
}


/*
 *  For a description of the following, see AT&T's data sheet for ATT20C490/491
 *  and ATT20C492/493--GGL
 */
static void write_cr(cr)
    int cr;
{
    inb(WRITE); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    outb(RMR, cr); GlennsIODelay();
    inb(WRITE); GlennsIODelay();
}


static int read_cr()
{
    unsigned int cr;

    inb(WRITE); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    inb(RMR); GlennsIODelay();
    cr = inb(RMR); GlennsIODelay();
    inb(WRITE);
    return cr;
}


int RamdacShift;
/*
 * this variable will avoid the server from assigning 8-bit colors to a 6-bit DAC
 * It saves you a lot of space in the palette [kmg]
 */
Bool W32Dac8Bit = FALSE;

extern int vgaRamdacMask;
static int saved_cr;
static BOOL cr_saved;
static int rmr;
static void check_ramdac()
{
    unsigned char cmap[3], save_cmap[3];

    RamdacShift = 10;
    vgaRamdacMask = 0x3f;
    W32Dac8Bit = FALSE;
    
    rmr = inb(RMR);
    saved_cr = read_cr();
    cr_saved = TRUE;

    /* first see if ramdac type was given in XF86Config. If so, assume that is 
     * correct, and don't probe for it.
     */
    if (vga256InfoRec.ramdac) 
    {
       W32RamdacType = xf86StringToToken(W32DacTable, vga256InfoRec.ramdac);
       if (W32RamdacType < 0) {
          ErrorF("%s %s: Unknown RAMDAC type \"%s\"\n", XCONFIG_GIVEN,
                 vga256InfoRec.name, vga256InfoRec.ramdac);
          return ;
       }
       else
       {
          switch(W32RamdacType)
          {
            case  ATT20C490_DAC:
            case  ATT20C491_DAC:
       	          RamdacShift = 8;
       	          vgaRamdacMask = 0xff;
                  W32Dac8Bit = TRUE;
       	          break;
            case Sierra1502X_DAC:
		  generic_ramdac = TRUE;   /* avoids RAMDAC code from using */
		  			   /* ATT-specific extensions */
                  RamdacShift = 10;
                  vgaRamdacMask = 0x3f;
                  W32Dac8Bit = FALSE;
	    	  break;
	    case CH8398_DAC:
            case NORMAL_DAC: 
            case ATT20C47xA_DAC:
            case ATT20C497_DAC:
            case ATT20C493_DAC:
            case ATT20C492_DAC:
            case ICS5341_DAC:
            case GENDAC_DAC:
	    case STG1702_DAC:
            case STG1703_DAC:
            default:
                  RamdacShift = 10;
                  vgaRamdacMask = 0x3f;
                  W32Dac8Bit = FALSE;
          }
       }
    }

    else
    {
          /* now see if it's an ICS GenDAC */
          if (ProbeGenDAC(FALSE))
	  {
	    /* this is a gendac. nothing to do here */
	  }
	  else if (ProbeSTG1703(FALSE))
	  {
	    /* this is the STG1703 */
	  }
	  else if (ProbeCH8398(FALSE))
	  {
	    /* this is a CH8398 */
	  }
	  else
          /* if none of the above: start probing for other DAC's */
          {
            outb(RMR, 0xff); GlennsIODelay();
            inb(RMR); GlennsIODelay();
            inb(RMR); GlennsIODelay();
            inb(RMR); GlennsIODelay();
            inb(RMR); GlennsIODelay();
            outb(RMR, 0x1c); GlennsIODelay();

            if (inb(RMR) != 0xff)
            {
        	cr_saved = FALSE;
        	W32RamdacType = ATT20C47xA_DAC;
        	return;
            }

            write_cr(0xe0);
            if ((read_cr() >> 5) != 0x7)
            {
        	W32RamdacType = ATT20C497_DAC;
        	return;
            }

            write_cr(0x60);
            if ((read_cr() >> 5) == 0)
            {
        	write_cr(0x2);	
        	if ((read_cr() & 0x2) != 0)
        	{
        	    W32RamdacType = ATT20C490_DAC;
        	    RamdacShift = 8;
        	    vgaRamdacMask = 0xff;
                    W32Dac8Bit = TRUE;
        	}
        	else
        	{
        	    W32RamdacType = ATT20C493_DAC;
        	}
            }
            else
            {
        	write_cr(0x2);	
        	outb(RMR, 0xff);
        	read_color(0xff, save_cmap);

        	write_color(0xff, white_cmap);
        	read_color(0xff, cmap);

        	if (cmap[0] == 0xff && cmap[1] == 0xff && cmap[2] == 0xff)
        	{
        	    W32RamdacType = ATT20C491_DAC;
        	    RamdacShift = 8;
        	    vgaRamdacMask = 0xff;
                    W32Dac8Bit = TRUE;
        	}
        	else
        	{
        	    W32RamdacType = ATT20C492_DAC;
        	}
        	
        	write_color(0xff, save_cmap);
            }
          }
    }
  ErrorF("%s %s: Ramdac: %s\n",
          (vga256InfoRec.ramdac) ? XCONFIG_GIVEN : XCONFIG_PROBED,
          vga256InfoRec.name, xf86TokenToString(W32DacTable, W32RamdacType));
  
    
}



void setup_et6000_ramdac()
{
   /* we do nothing special for the ET6000 RAMDAC yet, just set some 
    * important variables
    */
   W32RamdacType = ET6000_DAC;
   generic_ramdac = TRUE;   /* avoids RAMDAC code from using ATT-specific extensions */
   RamdacShift = 10;
   vgaRamdacMask = 0x3f;
   W32Dac8Bit = FALSE;
}

void VGARamdac()
{
    if (generic_ramdac)
	return;
    if (cr_saved)
	write_cr(saved_cr);
    outb(RMR, rmr);
}


void XRamdac()
{
    if (generic_ramdac)
	return;
    if (RamdacShift == 8)
	write_cr(0x2);
    outb(RMR, 0xff);
}


void SetupRamdac()
{
#if WHY_HARDCODE_THIS
    if (OFLG_ISSET(XCONFIG_RAMDAC, &vga256InfoRec.xconfigFlag))
    {
	cr_saved = FALSE; 

	if (strcmp(vga256InfoRec.ramdac, "generic") == 0) 
	{
	    generic_ramdac = TRUE;

            W32Dac8Bit = FALSE;
	    RamdacShift = 10;
	    vgaRamdacMask = 0x3f;
	    ErrorF("%s %s: Ramdac:  generic\n", XCONFIG_GIVEN,
		   vga256InfoRec.name);
	}
	else
	{
	    FatalError("%s: SetupRamdac:  Unrecognized RAMDAC: \"%s\"",
		       vga256InfoRec.name, vga256InfoRec.ramdac);
	}
	return;
    }
#endif

    check_ramdac();
    if (cr_saved && RamdacShift == 10)
	write_cr(saved_cr);
    outb(RMR, 0xff);
}
