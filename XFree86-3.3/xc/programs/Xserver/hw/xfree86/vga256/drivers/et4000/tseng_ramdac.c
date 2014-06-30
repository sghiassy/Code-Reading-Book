/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/et4000/tseng_ramdac.c,v 3.3.2.9 1997/06/01 12:33:38 dawes Exp $ */

/*
 *
 * Copyright 1993-1997 The XFree86 Project, Inc.
 *
 */

/*
 * tseng_ramdac.c.
 *
 * Much of this code was taken from the XF86_W32 (3.2) server [kmg]
 */


#include "X.h"
#include "Xproto.h"
#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86Priv.h"

#include "xf86.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#include "tseng.h"
#include "vga.h"

SymTabRec TsengDacTable[] = {
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
   { STG1700_DAC,        "stg1700" },
   { STG1702_DAC,        "stg1702" },
   { STG1703_DAC,        "stg1703" },
   { ET6000_DAC,         "et6000" },
   { CH8398_DAC,         "ch8398" },
   { UNKNOWN_DAC,        "unknown" },
};

t_ramdactype TsengRamdacType = UNKNOWN_DAC;

/* pixel multiplexing variables */
Bool Tseng_pixMuxPossible = FALSE;
int Tseng_nonMuxMaxClock = 0;
int Tseng_pixMuxMinClock = 0;
int Tseng_pixMuxMinWidth = 1024;


/*** private data ***/

#define RAMDAC_RMR 0x3c6
#define RAMDAC_READ 0x3c7
#define RAMDAC_WRITE 0x3c8
#define RAMDAC_RAM 0x3c9

static unsigned char black_cmap[] = {0x0, 0x0, 0x0};
static unsigned char white_cmap[] = {0xff, 0xff, 0xff};
static Bool dac_is_16bit = FALSE;

/* generic_ramdac avoids RAMDAC code from using ATT-specific extensions */
static BOOL generic_ramdac;

static int RamdacShift;

/*
 * this variable will avoid the server from assigning 8-bit colors to a 6-bit DAC
 * It saves you a lot of space in the palette [kmg]
 */
static Bool TsengDac8Bit = FALSE;

static int vgaRamdacMask;
static int saved_cr;
static int rmr;


static Bool
ProbeSTG1703(Bool quiet)
{
    unsigned char cid, did, daccomm, readmask;
    int i;
    Bool Found = FALSE;

    readmask = inb(RAMDAC_RMR);
    xf86dactopel();
    daccomm = xf86getdaccomm();
    xf86setdaccomm(daccomm | 0x10);
    xf86dactocomm();
    inb(0x3C6);
    outb(RAMDAC_RMR, 0x00);
    outb(RAMDAC_RMR, 0x00);
    cid = inb(RAMDAC_RMR);     /* company ID */
    did = inb(RAMDAC_RMR);     /* device ID */
    xf86dactopel();
    outb(RAMDAC_RMR,readmask);
    xf86setdaccomm(daccomm);

    if (cid == 0x44) {   /* STG170x RAMDAC found */
       Found = TRUE;
       switch (did) {
          case 0x02: TsengRamdacType = STG1702_DAC;
                     break;
          case 0x03: TsengRamdacType = STG1703_DAC;
                     break;
          case 0x00:
          default: TsengRamdacType = STG1700_DAC;
                   /* treat an unknown STG170x as a 1700 */
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

    outb(RAMDAC_READ,0);
    for(i=0; i<2*3; i++)         /* save first two LUT entries */
       savelut[i] = inb(RAMDAC_RAM);
    outb(RAMDAC_WRITE,0);
    for(i=0; i<2*3; i++)         /* set first two LUT entries to zero */
       outb(RAMDAC_RAM,0);

    outb(vgaIOBase + 4, 0x31);
    outb(vgaIOBase + 5, saveCR31 | 0x40);

    outb(RAMDAC_READ,0);
    for(i=clock01=0; i<4; i++)
       clock01 = (clock01 << 8) | (inb(RAMDAC_RAM) & 0xff);
    for(i=clock23=0; i<4; i++)
       clock23 = (clock23 << 8) | (inb(RAMDAC_RAM) & 0xff);

    /* get MClk value */
    outb(RAMDAC_READ,0x0a);
    mclk = (inb(RAMDAC_RAM)+2)*14.31818;
    dbyte = inb(RAMDAC_RAM);
    mclk /= (((dbyte & 0x1f)+2) * 1<<((dbyte & 0x60)>>5));

    outb(vgaIOBase + 4, 0x31);
    outb(vgaIOBase + 5, saveCR31 & ~0x40);

    outb(RAMDAC_WRITE,0);
    for(i=0; i<2*3; i++)         /* restore first two LUT entries */
       outb(RAMDAC_RAM,savelut[i]);

    outb(vgaIOBase + 4, 0x31);
    outb(vgaIOBase + 5, saveCR31);

    if ( clock01 == 0x28613d62 ||
        (clock01 == 0x7f7f7f7f && clock23 != 0x7f7f7f7f)) {
       found = TRUE;

       xf86dactopel();
       inb(RAMDAC_RMR);
       inb(RAMDAC_RMR);
       inb(RAMDAC_RMR);
  
       dbyte = inb(RAMDAC_RMR);
       /* the fourth read will show the GenDAC/SDAC chip ID and revision */
       switch (dbyte & 0xf0) {
         case 0xb0:
          if (!quiet) {
             ErrorF("%s %s: Ramdac: ICS 5341 GenDAC and programmable clock (MClk = %1.2f MHz)\n",
                    XCONFIG_PROBED, vga256InfoRec.name, mclk);
          }
          TsengRamdacType = ICS5341_DAC;
          break;
         case 0xf0:
          if (!quiet) {
             ErrorF("%s %s: Ramdac: ICS 5301 GENDAC and programmable clock (MClk = %1.2f MHz)\n",
                    XCONFIG_PROBED, vga256InfoRec.name, mclk);
          }
          TsengRamdacType = GENDAC_DAC;
          break;
         default:
          if (!quiet) {
             ErrorF("%s %s: Ramdac: unknown GENDAC and programmable clock (ID code = 0x%02x)\n",
                    XCONFIG_PROBED, vga256InfoRec.name, dbyte);
          }
          TsengRamdacType = GENDAC_DAC;
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
    cid = inb(RAMDAC_RMR);
    cid = inb(RAMDAC_RMR);
    cid = inb(RAMDAC_RMR);
    cid = inb(RAMDAC_RMR);  /* this returns chip ID */
    if (cid == 0xc0) {
       Found = TRUE;
       TsengRamdacType = CH8398_DAC;
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
    inb(RAMDAC_WRITE); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    outb(RAMDAC_RMR, cr); GlennsIODelay();
    inb(RAMDAC_WRITE); GlennsIODelay();
}


static int read_cr()
{
    unsigned int cr;

    inb(RAMDAC_WRITE); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    inb(RAMDAC_RMR); GlennsIODelay();
    cr = inb(RAMDAC_RMR); GlennsIODelay();
    inb(RAMDAC_WRITE);
    return cr;
}


static void
write_color(entry, cmap)
    int entry;
    unsigned char *cmap;
{
    outb(RAMDAC_WRITE, entry);
    GlennsIODelay();
    outb(RAMDAC_RAM, cmap[0]);
    GlennsIODelay();
    outb(RAMDAC_RAM, cmap[1]);
    GlennsIODelay();
    outb(RAMDAC_RAM, cmap[2]);
    GlennsIODelay();
}


static void
read_color(entry, cmap)
    int entry;
    unsigned char *cmap;
{
    outb(RAMDAC_READ, entry);
    GlennsIODelay();
    cmap[0] = inb(RAMDAC_RAM);
    GlennsIODelay();
    cmap[1] = inb(RAMDAC_RAM);
    GlennsIODelay();
    cmap[2] = inb(RAMDAC_RAM);
    GlennsIODelay();
}

          

void Check_Tseng_Ramdac()
{
    unsigned char cmap[3], save_cmap[3];
    BOOL cr_saved;

    rmr = inb(RAMDAC_RMR);
    saved_cr = read_cr(); cr_saved = TRUE;

    /* first see if ramdac type was given in XF86Config. If so, assume that is 
     * correct, and don't probe for it.
     */
    if (vga256InfoRec.ramdac) 
    {
       TsengRamdacType = xf86StringToToken(TsengDacTable, vga256InfoRec.ramdac);
       if (TsengRamdacType < 0) {
          ErrorF("%s %s: Unknown RAMDAC type \"%s\"\n", XCONFIG_GIVEN,
                 vga256InfoRec.name, vga256InfoRec.ramdac);
          return ;
       }
    }
    else    /* autoprobe for the RAMDAC */
    {
        /* is it an ICS GenDAC ? */
        if (ProbeGenDAC(FALSE))
        {
          /* It is. Nothing to do here */
        }
        else if (ProbeSTG1703(FALSE))
        {
          /* it's a STG170x */
        }
        else if (ProbeCH8398(FALSE))
        {
          /* it's a CH8398 */
        }
        else
        /* if none of the above: start probing for other DAC's */
        {
          outb(RAMDAC_RMR, 0xff); GlennsIODelay();
          inb(RAMDAC_RMR); GlennsIODelay();
          inb(RAMDAC_RMR); GlennsIODelay();
          inb(RAMDAC_RMR); GlennsIODelay();
          inb(RAMDAC_RMR); GlennsIODelay();
          outb(RAMDAC_RMR, 0x1c); GlennsIODelay();

          if (inb(RAMDAC_RMR) != 0xff)
          {
              cr_saved = FALSE;
              TsengRamdacType = ATT20C47xA_DAC;
              return;
          }

          write_cr(0xe0);
          if ((read_cr() >> 5) != 0x7)
          {
              TsengRamdacType = ATT20C497_DAC;
              return;
          }

          write_cr(0x60);
          if ((read_cr() >> 5) == 0)
          {
              write_cr(0x2);        
              if ((read_cr() & 0x2) != 0)
                  TsengRamdacType = ATT20C490_DAC;
              else
                  TsengRamdacType = ATT20C493_DAC;
          }
          else
          {
              write_cr(0x2);        
              outb(RAMDAC_RMR, 0xff);
              read_color(0xff, save_cmap);

              write_color(0xff, white_cmap);
              read_color(0xff, cmap);

              if (cmap[0] == 0xff && cmap[1] == 0xff && cmap[2] == 0xff)
                  TsengRamdacType = ATT20C491_DAC;
              else
                  TsengRamdacType = ATT20C492_DAC;
              
              write_color(0xff, save_cmap);
          }
        }
    }

   /* defaults: 8-bit wide DAC port, 6-bit color lookup-tables */
   RamdacShift = 10;
   vgaRamdacMask = 0x3f;
   TsengDac8Bit = FALSE;
   dac_is_16bit = FALSE;

   /* now override deaults with appropriate values for each RAMDAC */
   switch(TsengRamdacType)
   {
     case  ATT20C490_DAC:
     case  ATT20C491_DAC:
              RamdacShift = 8;
              vgaRamdacMask = 0xff;
              TsengDac8Bit = TRUE;
              break;
     case Sierra1502X_DAC:
              generic_ramdac = TRUE;   /* avoids treatment as ATT compatible DAC */
              break;
     case ET6000_DAC:
              OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
              OFLG_SET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions);
              generic_ramdac = TRUE;   /* avoids treatment as ATT compatible DAC */
              break;
     case ICS5341_DAC:
              OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
              OFLG_SET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions);
              dac_is_16bit = TRUE;
              break;
     case STG1702_DAC:
     case STG1700_DAC:
              dac_is_16bit = TRUE;
              break;
     case STG1703_DAC:
              OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
              OFLG_SET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions);
              dac_is_16bit = TRUE;
              break;
     case CH8398_DAC:
              dac_is_16bit = TRUE;
              break;
   }

    ErrorF("%s %s: Ramdac: %s\n",
            (vga256InfoRec.ramdac) ? XCONFIG_GIVEN : XCONFIG_PROBED,
            vga256InfoRec.name, xf86TokenToString(TsengDacTable, TsengRamdacType));

    if (cr_saved && RamdacShift == 10)
        write_cr(saved_cr);
    outb(RAMDAC_RMR, 0xff);
}


void tseng_init_clockscale(int bytesperpixel)
{
    /* nothing to do for 1:1 modes */
    if (bytesperpixel <= 1) return;
    if (et4000_type >= TYPE_ET6000) return;

    /* 16-bit ET4000W32p RAMDACs need different treatment than 8-bitters */
     if (dac_is_16bit) {
         switch (bytesperpixel) {
             case 3: ET4000.ChipClockDivFactor = 2; 
                     ET4000.ChipClockMulFactor = 3;
                     break;
             case 4: ET4000.ChipClockMulFactor = 2;
                     break;
         }
         return;
     }
     
     /* 8-bit RAMDACs */
     ET4000.ChipClockMulFactor = bytesperpixel; /* 8-bit RAMDAC */
     return;
}


void tseng_set_dacspeed(int bytesperpixel)
{
   /*
    * Memory bandwidth is important in > 8bpp modes, especially on ET4000
    *
    * This code evaluates a video mode with respect to requested dot clock
    * (depends on the VGA chip and the RAMDAC) and the resulting bandwidth
    * demand on memory (which in turn depends on color depth).
    *
    * For each mode, the minimum of max data transfer speed (dot clock
    * limit) and memory bandwidth determines if the mode is allowed.
    *
    * We should also take acceleration into account: accelerated modes
    * strain the bandwidth heavily, because they cause lots of random
    * acesses to video memory, which is bad for bandwidth due to smaller
    * page-mode memory requests.
    */

    int mem_bw;     /* memory bandwidth */
#ifndef USE_OFFICIAL_TSENG_LIMITS
    unsigned char bw_reg;
#endif

    if (bytesperpixel < 1) bytesperpixel = 1;  /* for MONO/VGA16 */

    /*
     * First, determine if we can use pixel multiplexing. This will have
     * impact on the max allowed pixelclock.
     */

    if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
      if (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions))
      {
        ErrorF("%s %s: Using W32p programmable clock chip ICS5341\n",
               OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions) ?
               XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
        if (vgaBitsPerPixel == 8) {
          Tseng_pixMuxPossible = TRUE;
          Tseng_nonMuxMaxClock = MAX_TSENG_CLOCK;  /* or 75000 ? */
          Tseng_pixMuxMinClock = 67500;
          Tseng_pixMuxMinWidth = 1024;   /* seems to be this way: 1024x768 is wrong with pixmux */
        }
      }
      else if (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions))
      {
        ErrorF("%s %s: Using W32p programmable clock chip STG1703\n",
               OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions) ?
               XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
        if (vgaBitsPerPixel == 8) {
        Tseng_pixMuxPossible = TRUE;
        Tseng_nonMuxMaxClock = MAX_TSENG_CLOCK;  /* or 75000 ? */
        Tseng_pixMuxMinClock = 67500;
        Tseng_pixMuxMinWidth = 1024;   /* seems to be this way: 1024x768 is wrong with pixmux */
        }
      }
      else if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions))
      {
        ErrorF("%s %s: Using W32 programmable clock chip ICD2061a\n",
               OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions) ?
               XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
      }
      else if (OFLG_ISSET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions))
      {
        ErrorF("%s %s: Using ET6000 built-in programmable clock\n",
               OFLG_ISSET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions) ?
               XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
      }
      else
      {
        ErrorF("%s %s: Unsupported clock chip given for ET4000 W32\n",
               XCONFIG_GIVEN, vga256InfoRec.name);
        /* return(FALSE); */
      }
    }

    /* if not set in the XF86Config file, use defaults */
    if (vga256InfoRec.dacSpeeds[0] <= 0) {
        switch(TsengRamdacType) {
          case CH8398_DAC:
          case GENDAC_DAC:
          case ICS5341_DAC:
          case STG1700_DAC:
          case STG1702_DAC:
          case STG1703_DAC:
              if (Tseng_pixMuxPossible)
                   vga256InfoRec.dacSpeeds[0] = 135000;
              else
                   vga256InfoRec.dacSpeeds[0] = MAX_TSENG_CLOCK;
              break;
          case ET6000_DAC:
              vga256InfoRec.dacSpeeds[0] = 135000;
              break;
          case ET6300_DAC:
              vga256InfoRec.dacSpeeds[0] = 175000;
              break;
          default:
              vga256InfoRec.dacSpeeds[0] = MAX_TSENG_CLOCK;
        }
    }

    if (et4000_type < TYPE_ET6000) {
        mem_bw = 90000;
        if (vga256InfoRec.videoRam > 1024)
            mem_bw = 150000;  /* interleaved DRAM gives 70% more bandwidth */

        /* note that the vga code will scale the maxclock using
         * ClockMulFactor and ClockDivFactor, so we have to take this into
         * account here.
         */
        vga256InfoRec.maxClock =
          min(vga256InfoRec.dacSpeeds[0],
           ((mem_bw/bytesperpixel)*ET4000.ChipClockMulFactor)/ET4000.ChipClockDivFactor);
    }
    else
    {
#define USE_OFFICIAL_TSENG_LIMITS
    /* According to Tseng:
     * "Besides the 135 MHz maximum pixel clock frequency, the other limit has to
     * do with where you get FIFO breakdown (usually appears as stray horizontal
     * lines on the screen). Assuming the accelerator is running steadily doing a
     * worst case operation, to avoid FIFO breakdown you should keep the product
     *   pixel_clock*(bytes/pixel) <= 225 MHz . This is based on an XCLK
     * (system/memory) clock of 92 MHz (which is what we currently use) and
     * a value in the RAS/CAS Configuration register (CFG 44) of either 015h
     * or 014h (depending on the type of MDRAM chips). Also, the FIFO low
     * threshold control bit (bit 4 of CFG 41) should be set for modes where
     * pixel_clock*(bytes/pixel) > 130 MHz . These limits are for the
     * current ET6000 chips. An upcoming revision of the ET6000 [scheduled
     * for q3, the ET6300] will raise the pixel clock limit to 175 MHz and
     * the pixel_clock*(bytes/pixel) FIFO breakdown limit to about 275 MHz."
     */
#ifdef USE_OFFICIAL_TSENG_LIMITS
      if (et4000_type > TYPE_ET6000)      /* ET6300 */
          mem_bw = 275000;
      else                                /* ET6000 */
          mem_bw = 225000;
#else
      mem_bw = 90000 * 2 * 5/6;       /* 1 MDRAM bandwidth at 90 MHz, with 80% efficiency */
      bw_reg = inb(ET6Kbase + 0x45);
      if (bw_reg & 0x04) mem_bw *=2;  /* 2 MDRAM channels  (2 chips) */
      if (bw_reg & 0x03) mem_bw *=2;  /* interleaved MDRAM (4 chips) */
#endif
      vga256InfoRec.maxClock = min(vga256InfoRec.dacSpeeds[0], mem_bw/bytesperpixel);
    }
    
#ifndef USE_OFFICIAL_TSENG_LIMITS
    if (xf86Verbose) {
    ErrorF("%s %s: Estimated video memory bandwidth: %d MB/s.\n",
             XCONFIG_PROBED, vga256InfoRec.name, mem_bw/1000);
    }
#endif

#ifndef MONOVGA  /* cosmetic: vgaBitsPerPixel is "0" in VGA16 and MONO mode */
#ifndef XF86VGA16
    if (xf86Verbose) {
      ErrorF("%s %s: Ramdac speed at %dbpp: %3.3f MHz\n",
             OFLG_ISSET(XCONFIG_DACSPEED, &vga256InfoRec.xconfigFlag) ?
             XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name,
             vgaBitsPerPixel, vga256InfoRec.dacSpeeds[0] / 1000.0);
    }
#endif
#endif

    /* Check that maxClock is not higher than dacSpeed */
    if (vga256InfoRec.maxClock > vga256InfoRec.dacSpeeds[0])
      vga256InfoRec.maxClock = vga256InfoRec.dacSpeeds[0];


}

void tseng_validate_mode(DisplayModePtr mode, int bytesperpixel, Bool verbose)
{
   int pixel_clock;
   int hdiv=1, hmul=1;

   /*
    * A true weirdness is that in ET4000Init(), mode->Clock points to the
    * clock _index_ in the array of clocks (usually 2 for a programmable
    * clock), while in ET4000Validate() mode->Clock is the actual pixel
    * clock (in kHZ). In other words, in ET4000Init() you need to use
    * vga256InfoRec.clock[mode->Clock] to access the clock frequency, while
    * in ET4000Validate(), mode->Clock is enough.
    */
    pixel_clock = vga256InfoRec.clock[mode->Clock];
 
   /*
    * For programmable clocks, fill in the SynthClock value
    * and set V_DBLCLK as required for each mode
    */

   if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
         mode->SynthClock = (pixel_clock * ET4000.ChipClockMulFactor) / ET4000.ChipClockDivFactor;
   }

   /* nothing more to do for MONO/VGA16 (pixmux doesn't work anyway) */
   if (bytesperpixel < 1) return;

   /*
    * Check what impact each mode has on pixel multiplexing,
    * and mark those modes for which pixmux must be used.
    * PixMux is only possible in 8bpp.
    * Pixmux SHOULD work in interlaced mode, but I don't know how to do it :-(
    */

   if ( (Tseng_pixMuxPossible) &&
        ((pixel_clock / 1000) > (Tseng_nonMuxMaxClock / 1000)) &&
        (mode->HDisplay >= Tseng_pixMuxMinWidth) &&
        (!(mode->Flags & V_INTERLACE)) ) {
            mode->Flags |= V_PIXMUX;
            switch(TsengRamdacType) {
                case ICS5341_DAC:
                case STG1700_DAC:
                case STG1702_DAC:
                case STG1703_DAC:
                   if (mode->SynthClock > Tseng_nonMuxMaxClock) {
                      mode->SynthClock /= 2;
                      mode->Flags |= V_DBLCLK;
                   }
                   break;
                default:
                   /* Do nothing */
                   break;
            }
            if (verbose)
                ErrorF("%s %s: Mode \"%s\" will use pixel multiplexing.\n",
                        XCONFIG_PROBED, vga256InfoRec.name, mode->name);
        }

   /*
    * define hdiv and hmul depending on mode, pixel multiplexing and ramdac type
    */
    if (mode->Flags & V_PIXMUX)
    {
       if ((OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions))
            && (TsengRamdacType==ICS5341_DAC))
       {
          if (mode->Flags & V_DBLCLK)
              hdiv = 2;
       }
       if ((OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions))
            && (TsengRamdacType==STG1703_DAC))
       {
          if (mode->Flags & V_DBLCLK)
              hdiv = 2;
       }
    }
    
    hmul *= ET4000.ChipClockMulFactor;
    hdiv *= ET4000.ChipClockDivFactor;

   /*
    * Modify mode timings accordingly
    */
     if (!mode->CrtcHAdjusted) {
          /* now divide and multiply the horizontal timing parameters as required */
          mode->CrtcHTotal     = (mode->CrtcHTotal * hmul) / hdiv;
          mode->CrtcHDisplay   = (mode->CrtcHDisplay * hmul) / hdiv;
          mode->CrtcHSyncStart = (mode->CrtcHSyncStart * hmul) / hdiv;
          mode->CrtcHSyncEnd   = (mode->CrtcHSyncEnd * hmul) / hdiv;
          mode->CrtcHSkew      = (mode->CrtcHSkew * hmul) / hdiv;
          if (bytesperpixel == 3) {
             int rgb_skew;
            /* in 24bpp, the position of the BLANK signal determines the
             * phase of the R,G and B values. XFree86 sets blanking equal to
             * the Sync, so setting the Sync correctly will also set the
             * BLANK corectly, and thus also the RGB phase */
             rgb_skew = (mode->CrtcHTotal/8 - mode->CrtcHSyncEnd/8 - 1) % 3;
             mode->CrtcHSyncEnd += rgb_skew * 8 + 24;
             /* HSyncEnd must come BEFORE HTotal */
             if (mode->CrtcHSyncEnd > mode->CrtcHTotal)
               mode->CrtcHSyncEnd -= 24;
             /* HSyncEnd could now have been moved BEFORE HSyncStart,
              * but if that happens, it means you had a sync of only 8
              * clocks long. This should not happen
              */
          }
          mode->CrtcHAdjusted  = TRUE;
    }
}


/*
 * The following arrays hold command register values for all possible
 * modes of the 16-bit DACs used on ET4000W32p cards (bpp/pixel_bus_width):
 *
 * { 8bpp/8, 15bpp/8, 16bpp/8, 24bpp/8, 32bpp/8,
 *   8bpp/16, 15bpp/16, 16bpp/16, 24bpp/16, 32bpp/16
 * } 
 *
 * "0xFF" is used as a "not-supported" flag. Assuming no RAMDAC uses this
 * value for some real configuration...
 */
static unsigned char CMD_ICS5341[] = { 0x00, 0x20, 0x60, 0x40, 0xFF,
                                       0x10, 0x30, 0x50, 0x90, 0x70 };

static unsigned char CMD_STG1703[] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
                                       0x05, 0x02, 0x03, 0x04, 0xFF };

static unsigned char CMD_CH8398[]  = { 0x04, 0xC4, 0x64, 0x74, 0xFF,
                                       0x24, 0x14, 0x34, 0xB4, 0xFF };

static unsigned char CMD_ATT49x[]  = { 0x00, 0xa0, 0xc0, 0xe0, 0xe0,
                                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/*
 * This sets up the RAMDAC registers for the correct BPP and pixmux values.
 * (also set VGA controller registers for pixmux and BPP)
 */
void tseng_set_ramdac_bpp(DisplayModePtr mode, vgaET4000Ptr tseng_regs, int bytesperpixel)
{
   Bool rgb555, rgb565, dac16bit;
   unsigned char* cmd_array = NULL;
   unsigned char* cmd_dest = NULL;
   int index;
   
   if (bytesperpixel < 1) bytesperpixel = 1;  /* for MONO/VGA16 */

   rgb555 = (xf86weight.red == 5 && xf86weight.green == 5 && xf86weight.blue == 5);
   rgb565 = (xf86weight.red == 5 && xf86weight.green == 6 && xf86weight.blue == 5);
   
  /* This is not the good way to find out if we're in 8- or 16-bit RAMDAC
   * mode It should rather be passed on from the tseng_validate_mode() code.
   * Right now it'd better agree with what tseng_validate_mode() proposed.
   */ 
   dac16bit = (dac_is_16bit) && ((bytesperpixel > 1) || (mode->Flags & V_PIXMUX));

   tseng_regs->Misc &= 0xCF; /* ATC index 0x16 -- bits-per-PCLK */
   if (et4000_type >= TYPE_ET6000)
       tseng_regs->Misc |= (bytesperpixel-1) << 4;
   else if (dac16bit)
       tseng_regs->Misc |= 0x20;


   switch (TsengRamdacType) {
       case ATT20C490_DAC:
       case ATT20C491_DAC:
       case ATT20C492_DAC:
       case ATT20C493_DAC:
           cmd_array = CMD_ATT49x;
           cmd_dest = &(tseng_regs->ATTdac_cmd);
           break;
       case STG1700_DAC:
       case STG1702_DAC:
       case STG1703_DAC:
           tseng_regs->gendac.cmd_reg |= 8;
           cmd_array = CMD_STG1703;
           cmd_dest = &(tseng_regs->gendac.PLL_ctrl);
           break;
       case ICS5341_DAC:
       case GENDAC_DAC:
           cmd_array = CMD_ICS5341;
           tseng_regs->gendac.PLL_ctrl = 0;
           cmd_dest = &(tseng_regs->gendac.cmd_reg);
           break;
       case CH8398_DAC:
           cmd_array = CMD_CH8398;
           cmd_dest = &(tseng_regs->gendac.cmd_reg);
           break;
       case ET6000_DAC:
           if (vgaBitsPerPixel == 16) {
               if (rgb555)
                   tseng_regs->ET6KVidCtrl1 &= ~0x02; /* 5-5-5 RGB mode */
               if (rgb565)
                   tseng_regs->ET6KVidCtrl1 |=  0x02; /* 5-6-5 RGB mode */
           }
           break;
   }

   if (cmd_array != NULL) {
       switch (bytesperpixel) {
           case 1: index = 0; break;
           case 2: index = rgb555 ? 1 : 2; break;
           case 3: index = 3; break;
           case 4: index = 4; break;
       }
       if (dac16bit) index += 5;
       if (cmd_array[index] != 0xFF) {
           if (cmd_dest != NULL) {
               *cmd_dest = cmd_array[index];
           }
           else
               ErrorF("%s %s: cmd_dest = NULL -- please report\n",
                      XCONFIG_PROBED, vga256InfoRec.name);
       }
       else {
           tseng_regs->gendac.cmd_reg = 0;
           ErrorF("%s %s: %dbpp not supported in %d-bit DAC mode on this RAMDAC -- Please report.\n",
                 XCONFIG_PROBED, vga256InfoRec.name, bytesperpixel*8, dac16bit ? 16 : 8);
       }
   }

   if ( (mode->Flags & V_PIXMUX) && (et4000_type < TYPE_ET6000) ) {
       tseng_regs->std.CRTC[0x17] = (tseng_regs->std.CRTC[0x17] & 0xFB);

      /* to avoid blurred vertical line during flyback, disable H-blanking
       * (better solution needed !!!)
       */
       tseng_regs->std.CRTC[0x02] = 0xff;
   }
}
