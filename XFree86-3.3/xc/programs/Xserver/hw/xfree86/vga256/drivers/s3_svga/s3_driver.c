/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/s3_svga/s3_driver.c,v 3.17.2.2 1997/05/09 07:15:47 hohndel Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Thomas Roell makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 * 
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * 
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 */
/* $XConsortium: s3_driver.c /main/11 1996/10/25 10:33:45 kaleb $ */

#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"

  typedef struct {
     vgaHWRec std;		/* good old IBM VGA */
     unsigned char ExtStart;	/* Tseng S3 specials   CRTC 0x33/0x34/0x35 */
     unsigned char Compatibility;
     unsigned char OverflowHigh;
     unsigned char StateControl;/* TS 6 & 7 */
     unsigned char AuxillaryMode;
     unsigned char Misc;	/* ATC 0x16 */
     unsigned char SegSel;
     unsigned char s3reg[10];	/* Video Atribute */
     unsigned char s3sysreg[36];/* Video Atribute */
     unsigned char SDAC[6];       /* S3 SDAC command and PLL registers */
     unsigned char Trio[14];      /* Trio32/64 ext. sequenzer (PLL) registers */
  }
vgaS3Rec, *vgaS3Ptr;
static vgaS3Ptr oldS3 = NULL;
static Bool AlreadyInited = FALSE;

unsigned short chip_id, s3ChipRev;
#define S3_911_ONLY       (chip_id==0x81)
#define S3_924_ONLY       (chip_id==0x82)
#define S3_911_SERIES     ((chip_id&0xf0)==0x80)
#define S3_801_SERIES     ((chip_id&0xf0)==0xa0)
#define S3_805_I_SERIES   ((chip_id&0xf8)==0xa8)  /* (S3_801_SERIES && ( & 0x08)) */
#define S3_801_REV_C      (S3_801_SERIES && (chip_id & 0x07) >= 2)
#define S3_928_P          ((chip_id&0xf0)==0xb0)
#define S3_928_ONLY       ((chip_id&0xd0)==0x90)  /* (((chip_id&0xf0)==0x90)||S3_928_P) */
#define S3_928_REV_E      (S3_928_ONLY && (chip_id & 0x0F) >= 4)
#define S3_801_928_SERIES (S3_801_SERIES||S3_928_ONLY||S3_x64_SERIES)
#define S3_8XX_9XX_SERIES (S3_911_SERIES||S3_801_928_SERIES)
#define S3_864_SERIES     ((chip_id&0xf0)==0xc0)
#define S3_964_SERIES     ((chip_id&0xf0)==0xd0)
#define S3_866_SERIES     ((chip_id&0xfff0)==0x80e0)
#define S3_868_SERIES     ((chip_id&0xfff0)==0x90e0)
#define S3_86x_SERIES	  ((chip_id&0xf0)==0xc0 || (chip_id&0xeff0)==0x80e0)
#undef S3_864_SERIES
#define S3_864_SERIES	  S3_86x_SERIES
#define S3_968_SERIES     ((chip_id&0xfff0)==0xf0e0)
#define S3_964_ONLY	  S3_964_SERIES
#undef S3_964_SERIES
#define S3_964_SERIES     (((chip_id&0xf0)==0xd0) || S3_968_SERIES)
#define S3_x66_SERIES     S3_866_SERIES
#define S3_x68_SERIES     ((chip_id&0x9ff0)==0x90e0)  /* ((S3_868_SERIES || S3_968_SERIES) */
#define S3_x6x_SERIES     ((chip_id&0x8ff0)==0x80e0)  /* ((S3_x66_SERIES || S3_x68_SERIES) */
#define S3_TRIO32_SERIES  ((chip_id&0xfff0)==0x10e0)
#define S3_TRIO64_SERIES  ((chip_id&0xfff0)==0x11e0)
#define S3_TRIO64V_SERIES (S3_TRIO64_SERIES && (s3ChipRev & 0x400) == 0x400)
#define S3_TRIOxx_SERIES  ((chip_id&0xfef0)==0x10e0 || S3_ViRGE_SERIES)  /* (S3_TRIO32_SERIES || S3_TRIO64_SERIES || S3_ViRGE_SERIES) */
#define S3_ViRGE_SERIES   ((chip_id&0xfff0)==0x31e0)
#define S3_x64_SERIES	  (((chip_id&0xe0)==0xc0) || S3_x6x_SERIES ||  S3_TRIOxx_SERIES)
#define S3_928_SERIES     (S3_928_ONLY || S3_x64_SERIES)
#define S3_ANY_SERIES     (S3_8XX_9XX_SERIES || S3_x64_SERIES || S3_x66_SERIES || S3_x68_SERIES || S3_ViRGE_SERIES)


static Bool S3ClockSelect ();
static char *S3Ident ();
static Bool S3Probe ();
static void S3EnterLeave ();
static Bool S3Init ();
static int  S3ValidMode ();
static void *S3Save ();
static void S3Restore ();
static void S3Adjust ();
extern void S3SetRead ();
static Bool LegendClockSelect ();
 
vgaVideoChipRec S3_SVGA =
{
   S3Probe,
   S3Ident,
   S3EnterLeave,
   S3Init,
   S3ValidMode,
   S3Save,
   S3Restore,
   S3Adjust,
   vgaHWSaveScreen,
   (void (*)())NoopDDA,
   (void (*)())NoopDDA,
   S3SetRead,
   S3SetRead,
   S3SetRead,
   0x10000,
   0x10000,
   16,
   0xFFFF,
   0x00000, 0x10000,
   0x00000, 0x10000,
   FALSE,
   VGA_DIVIDE_VERT,
   {0,},
   8,
   FALSE,
   0,
   0,
   FALSE,
   FALSE,
   FALSE,
   NULL,
   1,			/* int ChipClockMulFactor */
   1			/* int ChipClockDivFactor */
};

#define new ((vgaS3Ptr)vgaNewVideoState)

Bool (*ClockSelect) ();
short old_clock = 0;

static unsigned S3_ExtPorts[] = {0x22e8};
static int Num_S3_ExtPorts = 
   (sizeof(S3_ExtPorts)/sizeof(S3_ExtPorts[0]));

/*
 * S3Ident --
 */
char *
S3Ident (int n)
{
   static char *chipsets[] =
   {"s3"};

   if (n + 1 > sizeof (chipsets) / sizeof (char *))
        return (NULL);
   else
      return (chipsets[n]);

}


/*
 * S3ClockSelect -- select one of the possible clocks ...
 */

static Bool
S3ClockSelect (no)
     int no;
{
   static unsigned char save1, save2;
   unsigned char temp;
   
   switch(no)
   {
   case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaIOBase + 4, 0x42);
      save2 = inb(vgaIOBase + 5);
      break; 
   case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outb(vgaIOBase + 4, 0x42);
      outb(vgaIOBase + 5, save2);
      break;
   default:
      temp = inb (0x3CC);
      outb (0x3C2, /* temp | 0x01 */ 0x3f);     /* JNT */
      outb (vgaIOBase + 4, 0x42);
      temp = (inb (vgaIOBase + 5) & 0xf0 );
      outb (vgaIOBase + 5, (temp | no));
   }
   return(TRUE);
}



/*
 * LegendClockSelect -- select one of the possible clocks ...
 */

static Bool
LegendClockSelect (no)
     int no;
{
 /*
  * Sigma Legend special handling
  * 
  * The Legend uses an ICS 1394-046 clock generator.  This can generate 32
  * different frequencies.  The Legend can use all 32.  Here's how:
  * 
  * There are two flip/flops used to latch two inputs into the ICS clock
  * generator.  The five inputs to the ICS are then
  * 
  * ICS     ET-4000 ---     --- FS0     CS0 FS1     CS1 FS2     ff0
  * flip/flop 0 output FS3     CS2 FS4     ff1     flip/flop 1 output
  * 
  * The flip/flops are loaded from CS0 and CS1.  The flip/flops are latched by
  * CS2, on the rising edge. After CS2 is set low, and then high, it is then
  * set to its final value.
  * 
  */
   unsigned char temp = inb (0x3CC);

   outb (0x3C2, (temp & 0xF3) | ((no & 0x10) >> 1) | (no & 0x04));
   outw (vgaIOBase + 4, 0x0034);
   outw (vgaIOBase + 4, 0x0234);
   outw (vgaIOBase + 4, ((no & 0x08) << 6) | 0x34);
   outb (0x3C2, (temp & 0xF3) | ((no << 2) & 0x0C));

   return TRUE;
}


/* The GENDAC code also works for the SDAC, used for Trio32/Trio64 
   and for ViRGE too */

static Bool
S3GendacClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;
   unsigned char tmp;
 
   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = S3ClockSelect(freq);
      break;
   default:
      {

	 if (S3_TRIOxx_SERIES) {
	    (void) S3TrioSetClock(freq, 2); /* can't fail */
	 }
	 else {
	    if (OFLG_ISSET(CLOCK_OPTION_ICS5342, &vga256InfoRec.clockOptions))
	       (void) ICS5342SetClock(freq, 2); /* can't fail */
	    else
	       (void) S3gendacSetClock(freq, 2); /* can't fail */
	    outb(vgaIOBase + 4, 0x42);/* select the clock */
	    tmp = inb(vgaIOBase + 5) & 0xf0;
	    outb(vgaIOBase + 5, tmp | 0x02);
	    usleep(150000);
	 }
      }
   }

   return(result);
}



/*
 * S3Probe -- check up whether a S3 based board is installed
 */

static Bool
S3Probe ()
{
   int numClocks;
   unsigned short config;
   char *clockprobed = XCONFIG_GIVEN;

   xf86ClearIOPortList(vga256InfoRec.scrnIndex);
   xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
   xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_S3_ExtPorts, S3_ExtPorts);

   if (vga256InfoRec.chipset) {
      if (strncmp (vga256InfoRec.chipset, "s3", 2))
	 return (FALSE);
   }

   S3EnterLeave(ENTER);

   outb(0x3d4, 0x30);
   chip_id = inb(0x3d5);
   s3ChipRev = chip_id & 0x0f;
   if (chip_id >= 0xe0) {
      outb(vgaIOBase + 4, 0x2e);
      chip_id |= (inb(vgaIOBase + 5) << 8);
      outb(vgaIOBase + 4, 0x2f);
      s3ChipRev |= (inb(vgaIOBase + 5) << 4);      
   }


   old_clock = inb(0x3cc);
   if (!S3_ANY_SERIES) {
      S3EnterLeave(FALSE);
      return(FALSE);
   }

   if (xf86Verbose) {
      if (S3_x64_SERIES) {
	 char *chipname = "unknown";

	 if (S3_868_SERIES) {
	    chipname = "868";
	 } else if (S3_866_SERIES) {
	    chipname = "866";
	 } else if (S3_864_SERIES) {
	    chipname = "864";
	 } else if (S3_968_SERIES) {
	    chipname = "968";
	 } else if (S3_964_SERIES) {
	    chipname = "964";
	 } else if (S3_ViRGE_SERIES) {
	    chipname = "ViRGE";
	 } else if (S3_TRIO32_SERIES) {
	    chipname = "Trio32";
	 } else if (S3_TRIO64V_SERIES) {
	    chipname = "Trio64V+";
	 } else if (S3_TRIO64_SERIES) {
	    chipname = "Trio64";
	 }
	 ErrorF("%s %s: chipset:   %s rev. %x\n",
                XCONFIG_PROBED, vga256InfoRec.name, chipname, s3ChipRev);
      } else if (S3_801_928_SERIES) {
	 if (S3_801_SERIES) {
            if (S3_805_I_SERIES) {
               ErrorF("%s %s: chipset:   805i",
                      XCONFIG_PROBED, vga256InfoRec.name);
               if ((config & 0x03) == 3)
                  ErrorF(" (ISA)");
               else
                  ErrorF(" (VL)");
            }
	    else if (!((config & 0x03) == 3))
	       ErrorF("%s %s: chipset:   805",
                      XCONFIG_PROBED, vga256InfoRec.name);
	    else
	       ErrorF("%s %s: chipset:   801",
                       XCONFIG_PROBED, vga256InfoRec.name);
	    ErrorF(", ");
	    if (S3_801_REV_C)
	       ErrorF("rev C or above\n");
	    else
	       ErrorF("rev A or B\n");
	 } else if (S3_928_SERIES) {
	    char *pci = S3_928_P ? "-P" : "";
	    if (S3_928_REV_E)
		ErrorF("%s %s: chipset:   928%s, rev E or above\n",
                   XCONFIG_PROBED, vga256InfoRec.name, pci);
	    else
	        ErrorF("%s %s: chipset:   928%s, rev D or below\n",
                   XCONFIG_PROBED, vga256InfoRec.name, pci);
	 }
      } else if (S3_911_SERIES) {
	 if (S3_911_ONLY) {
	    ErrorF("%s %s: chipset:   911 \n",
                   XCONFIG_PROBED, vga256InfoRec.name);
	 } else if (S3_924_ONLY) {
	    ErrorF("%s %s: chipset:   924\n",
                   XCONFIG_PROBED, vga256InfoRec.name);
	 } else {
	    ErrorF("%s %s: S3 chipset type unknown, chip_id = 0x%02x\n",
		   XCONFIG_PROBED, vga256InfoRec.name, chip_id);
	 }
      }
   }
   else {
      ErrorF ("%s %s: Unknown chipset : chip_id is 0x%02x\n",
	      XCONFIG_PROBED, vga256InfoRec.name, chip_id);
   }

   outb (0x3d4, 0x36);          /* for register CR36 (CONFG_REG1), */
   config = inb (0x3d5);        /* get amount of vram installed */

   if ((config & 0x03) == 0) {
      ErrorF ("%s %s: This is an EISA card\n",
	      XCONFIG_PROBED, vga256InfoRec.name);
   }
   if ((config & 0x03) == 1) {
      ErrorF ("%s %s: This is a 386/486 localbus card\n",
	      XCONFIG_PROBED, vga256InfoRec.name);
   }
   if ((config & 0x03) == 3) {
      ErrorF ("%s %s: This is an ISA card\n",
	      XCONFIG_PROBED, vga256InfoRec.name);
   }
   if (!vga256InfoRec.videoRam) {
      if ((config & 0x20) != 0) {       /* if bit 5 is a 1, then 512k RAM */

         vga256InfoRec.videoRam = 512;
      } else {                  /* must have more than 512k */

         switch ((config & 0xC0) >> 6) {        /* look at bits 6 and 7 */
           case 0:
              vga256InfoRec.videoRam = 4096;
              break;
           case 1:
              vga256InfoRec.videoRam = 3072;
              break;
           case 2:
              vga256InfoRec.videoRam = 2048;
              break;
           case 3:
              vga256InfoRec.videoRam = 1024;
              break;
	 }
      }
#if 0
      ErrorF ("%d K of memory found\n", vga256InfoRec.videoRam);
#endif
   }
#if 0
   else ErrorF ("%d K videoram\n", vga256InfoRec.videoRam);
#endif


   if (S3_TRIOxx_SERIES) {
      unsigned char sr8;
      int m,n,n1,n2, mclk;

      if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
	 OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
	 OFLG_SET(CLOCK_OPTION_S3TRIO, &vga256InfoRec.clockOptions);
	 clockprobed = XCONFIG_PROBED;
      }
      ClockSelect = S3GendacClockSelect;
      numClocks = 3;

      outb(0x3c4, 0x08);
      sr8 = inb(0x3c5);
      outb(0x3c5, 0x06);
      
      outb(0x3c4, 0x11);
      m = inb(0x3c5);
      outb(0x3c4, 0x10);
      n = inb(0x3c5);
      
      outb(0x3c4, 0x08);
      outb(0x3c5, sr8);
      
      m &= 0x7f;
      n1 = n & 0x1f;
      n2 = (n>>5) & 0x03;
      mclk = ((1431818 * (m+2)) / (n1+2) / (1 << n2) + 50) / 100;
      if (xf86Verbose)
	 ErrorF("%s %s: Using Trio32/64/ViRGE programmable clock (MCLK %1.3f MHz)\n"
		,clockprobed, vga256InfoRec.name
		,mclk / 1000.0);
   }
   else if (OFLG_ISSET(OPTION_LEGEND, &vga256InfoRec.options)) {
      ClockSelect = LegendClockSelect;
      numClocks = 32;
   } else {
      ClockSelect = S3ClockSelect;
      numClocks = 16;
   }

   if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)
       && !vga256InfoRec.clocks)
      vgaGetClocks (numClocks, ClockSelect);

  vga256InfoRec.chipset = S3Ident(0);
   return (TRUE);
}


void S3CleanUp()
{
   unsigned char tmp;

   vgaProtect(TRUE);

   /* Restore S3 Trio32/64 ext. sequenzer (PLL) registers */
   if (S3_TRIOxx_SERIES) {
      outb(0x3c2, oldS3->Trio[0]);
      outb(0x3c4, 0x08); outb(0x3c5, 0x06);

      outb(0x3c4, 0x09); outb(0x3c5, oldS3->Trio[2]);
      outb(0x3c4, 0x0a); outb(0x3c5, oldS3->Trio[3]);
      outb(0x3c4, 0x0b); outb(0x3c5, oldS3->Trio[4]);
      outb(0x3c4, 0x0d); outb(0x3c5, oldS3->Trio[5]);

      outb(0x3c4, 0x10); outb(0x3c5, oldS3->Trio[8]); 
      outb(0x3c4, 0x11); outb(0x3c5, oldS3->Trio[9]); 
      outb(0x3c4, 0x12); outb(0x3c5, oldS3->Trio[10]); 
      outb(0x3c4, 0x13); outb(0x3c5, oldS3->Trio[11]); 
      outb(0x3c4, 0x1a); outb(0x3c5, oldS3->Trio[12]); 
      outb(0x3c4, 0x1b); outb(0x3c5, oldS3->Trio[13]); 
      outb(0x3c4, 0x15);
      tmp = inb(0x3c5);
      outb(0x3c4, tmp & ~0x20);
      outb(0x3c4, tmp |  0x20);
      outb(0x3c4, tmp & ~0x20);

      outb(0x3c4, 0x15); outb(0x3c5, oldS3->Trio[6]); 
      outb(0x3c4, 0x18); outb(0x3c5, oldS3->Trio[7]);

      outb(0x3c4, 0x08); outb(0x3c5, oldS3->Trio[1]);
   }
   AlreadyInited = 1;

   vgaProtect(FALSE);
}


/*
 * S3EnterLeave -- enable/disable io-mapping
 */

static void
S3EnterLeave (enter)
     Bool enter;
{
   unsigned char temp;

   if (enter) {
      xf86EnableIOPorts(vga256InfoRec.scrnIndex);
      vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
      vgaIOBase = 0x3D0;
      /* unlock everything */
      outb (0x3d4, 0x38);
      outb (0x3d5, 0x48);
      outb (0x3d4, 0x39);
      outb (0x3d5, 0xa5);

      /* This stuff should probably go elsewhere (DHD) */
      outb (0x3d4, 0x35);          /* select segment 0 */
      temp = inb (0x3d5);
      outb (0x3d5, temp & 0xf0);

      outb (0x3ce, 0x6);	/* set graphics mode & 64k */
      temp = inb (0x3cf);
      outb (0x3cf, temp | 5);

    /*
     * outb(0x3c0, 0x10);   this was to enable the palette outb(0x3c1, 0x41);
     */

      temp = inb (0x3CC);	/* enable ram address decoder */
      outb (0x3C2, temp | 2);
    /* vertical retrace low-end */
      outb (vgaIOBase + 4, 0x11);
      temp = inb (vgaIOBase + 5);
      outb (vgaIOBase + 5, temp & 0x7F);


   } else {
    /*
     * outb(0x3BF, 0x01);                            relock S3 special
     * outb(vgaIOBase + 8, 0xA0);
     */
      if (AlreadyInited) {
	 S3CleanUp();
	 /* AlreadyInited = FALSE; */
      }

      xf86DisableIOPorts(vga256InfoRec.scrnIndex);
   }
}





/*
 * S3Restore -- restore a video mode
 */

static void
S3Restore (restore)
     vgaS3Ptr restore;
{
   unsigned char i;
   extern Bool xf86Exiting;
   
   outb (0x3d4, 0x35);		/* select bank zero */
   i = inb (0x3d5);
   outb (0x3d5, (i & 0xf0));
   outb(0x3CD, 0x00); /* segment select */

   vgaHWRestore ((vgaHWPtr)restore);

  i = inb(vgaIOBase + 0x0A); /* reset flip-flop */
  outb(0x3C0, 0x36);
  outb(0x3C0, restore->Misc);

 /* restore s3 special bits */
   if (S3_801_928_SERIES) {
    /* restore 801 specific registers */

      for (i = 32; i < 35; i++) {
	 outb (0x3d4, 0x40 + i);
	 outb (0x3d5, restore->s3sysreg[i]);

      }
   }
   for (i = 0; i < 5; i++) {
      outb (0x3d4, 0x31 + i);
      outb (0x3d5, restore->s3reg[i]);
   } 
   for (i = 2 ; i < 5; i++) { /* don't restore the locks */   
      outb (0x3d4, 0x38 + i);
      outb (0x3d5, restore->s3reg[5 + i]);
   }

   for (i = 0; i < 16; i++) {
      outb (0x3d4, 0x40 + i);
      outb (0x3d5, restore->s3sysreg[i]);
   }

  if (restore->std.NoClock >= 0) {
     if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
	(ClockSelect)(vga256InfoRec.clock[restore->std.NoClock]);
     else
	(ClockSelect)(restore->std.NoClock);
  }
   

  if (xf86Exiting)
   outb(0x3c2, old_clock);
  outw(0x3C4, 0x0300); /* now reenable the timing sequencer */


}

/*
 * S3Save -- save the current video mode
 */

static void *
S3Save (save)
     vgaS3Ptr save;
{
   int i;
   unsigned char temp;

   temp = inb(0x3CD); outb(0x3CD, 0x00); /* segment select */

   save = (vgaS3Ptr) vgaHWSave ((vgaHWPtr)save, sizeof (vgaS3Rec));


   for (i = 0; i < 5; i++) {
      outb (0x3d4, 0x31 + i);
      save->s3reg[i] = inb (0x3d5);
      outb (0x3d4, 0x38 + i);
      save->s3reg[5 + i] = inb (0x3d5);
   }

   outb (0x3d4, 0x11);		/* allow writting? */
   outb (0x3d5, 0x00);
   for (i = 0; i < 16; i++) {
      outb (0x3d4, 0x40 + i);

      save->s3sysreg[i] = inb (0x3d5);
   }

   for (i = 32; i < 35; i++) {
      outb (0x3d4, 0x40 + i);
      save->s3sysreg[i] = inb (0x3d5);
   }
   i = inb(vgaIOBase + 0x0A); /* reset flip-flop */
   outb(0x3C0,0x36); save->Misc = inb(0x3C1); outb(0x3C0, save->Misc);

   return ((void *) save);
}




/*
 * S3Init -- Handle the initialization of the VGAs registers
 */

static Bool
S3Init (mode)
     DisplayModePtr mode;
{
   int i;
   static int s3Initialised = 0;

   vgaHWInit (mode, sizeof (vgaS3Rec));

   if (!s3Initialised) {
      s3Initialised = 1;

      outb(0x3d4, 0x38);
      outb(0x3d5, 0x48);
      old_clock = inb(0x3CC);

      oldS3 = vgaHWSave((vgaHWPtr)oldS3, sizeof(vgaS3Rec));

      /* Save S3 Trio32/64 ext. sequenzer (PLL) registers */
      if (S3_TRIOxx_SERIES) {
	 oldS3->Trio[0] = inb(0x3cc);

	 outb(0x3c4, 0x08); oldS3->Trio[1] = inb(0x3c5);
	 outb(0x3c5, 0x06);

	 outb(0x3c4, 0x09); oldS3->Trio[2]  = inb(0x3c5);
	 outb(0x3c4, 0x0a); oldS3->Trio[3]  = inb(0x3c5);
	 outb(0x3c4, 0x0b); oldS3->Trio[4]  = inb(0x3c5);
	 outb(0x3c4, 0x0d); oldS3->Trio[5]  = inb(0x3c5);
	 outb(0x3c4, 0x15); oldS3->Trio[6]  = inb(0x3c5) & 0xfe; 
	 outb(0x3c5, oldS3->Trio[6]);
	 outb(0x3c4, 0x18); oldS3->Trio[7]  = inb(0x3c5);

	 outb(0x3c4, 0x10); oldS3->Trio[8]  = inb(0x3c5);
	 outb(0x3c4, 0x11); oldS3->Trio[9]  = inb(0x3c5);
	 outb(0x3c4, 0x12); oldS3->Trio[10] = inb(0x3c5);
	 outb(0x3c4, 0x13); oldS3->Trio[11] = inb(0x3c5);
	 outb(0x3c4, 0x1a); oldS3->Trio[12] = inb(0x3c5);
	 outb(0x3c4, 0x1b); oldS3->Trio[13] = inb(0x3c5);

	 outb(0x3c4, 8);
	 outb(0x3c5, 0x00);
      }
   }

   new->std.Attribute[16] = 0x01;	/* use the FAST 256 Color Mode */
   new->std.CRTC[19] = vga256InfoRec.displayWidth >> 3;
   new->std.CRTC[20] = 0x60;
   new->std.CRTC[23] = 0xAB;
   new->StateControl = 0x00;

   new->ExtStart = 0x00;

 /* DON'T forget to set Interlace else where!!!!!! */
   new->OverflowHigh =
      ((mode->CrtcVSyncStart & 0x400) >> 7)
      | (((mode->CrtcVDisplay - 1) & 0x400) >> 9)
      | (((mode->CrtcVTotal - 2) & 0x400) >> 5)
      | (((mode->CrtcVTotal - 2) & 0x200) >> 9)
      | (((mode->CrtcVSyncStart) & 0x200) >> 3);


  new->s3reg[0] = 0x8d;
  new->s3reg[1] = 0x00;
  new->s3reg[2] = 0x20;
  new->s3reg[3] = 0x10;
  if (mode->CrtcHDisplay > 800)
      new->s3reg[4] = 0x00;
  else
     new->s3reg[4] = 0x13;

  new->s3reg[5] = 0x48;
  new->s3reg[6] = 0xa5;
  new->s3reg[7] = 0xb5;
  new->s3reg[8] = (new->std.CRTC[0] + new->std.CRTC[4] + 1)/2;  
  new->s3reg[9] = 0x44;
  new->s3reg[10] = mode->Clock;

  outb (0x3d4, 0x40);
  i = (inb (0x3d5) & 0xf6);    /* don't touch local bus setting */
  
  new->s3sysreg[0] = (i | 0x05);
  new->s3sysreg[1] = 0x14;
  new->s3sysreg[2] = mode->Clock;
  new->s3sysreg[3] = 0x00;
  new->s3sysreg[4] = 0x00;
  new->s3sysreg[5] = 0x00;

  for (i = 6 ; i < 16 ; i++) { /* For these I don't have doc */
   outb (0x3d4, 0x40 + i);
   new->s3sysreg[i] = inb(0x3d5);
  }

  if (S3_801_928_SERIES) {
    new->s3sysreg[24] = 0x00;
    new->s3sysreg[25] = 0x0a;
    new->s3sysreg[20] = 0xa0;
    new->s3sysreg[32] = 0x2f;
    new->s3sysreg[33] = 0x81;
    new->s3sysreg[34] = 0x00;
  }


#ifdef ENHANCED_ENABLE
   if (mode->CrtcHDisplay < 800) {	/* MOVE ME - JNT */
      outw (0x4ae8, 0x0003);
   } else
      outw (0x4ae8, 0x0007);
#endif

   if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
      (ClockSelect)(vga256InfoRec.clock[new->std.NoClock]);
   else
      (ClockSelect)(new->std.NoClock);


   return TRUE;
}



/*
 * S3Adjust -- adjust the current video frame to display the mousecursor
 */

static void
S3Adjust (x, y)
     int x, y;
{
  int Base;

  Base = (y * vga256InfoRec.displayWidth + x) >> 2;

  outb(0x3d4, 0x31);
  outb(0x3d5, ((Base & 0x030000) >> 12) | 0x8d); 
  
  vgaIOBase = 0x3d0;
  outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);

}


#if 0
go_linear()
{
   if (S3_801_928_SERIES) {
      int   i;
      ErrorF("go linear called \n");
    /* begin 801 sequence for going in to linear mode */
      outb (0x3d4, 0x40);
      i = inb (0x3d5) & 0xf6;
      i |= 0x0a;/* enable fast write buffer and disable
                                 * 8514/a mode */
      outb (0x3d5, (unsigned char) i);
      outb (0x3d4, 0x59);
      outb (0x3d5, 0x0a);
      outb (0x3d4, 0x53);
      outb (0x3d5, 1);
      outb (0x3d4, 0x58);
      outb (0x3d5, 0x10);       /* go on to linear mode */
   }
}
#endif

static int
S3ValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
  return MODE_OK;
}
