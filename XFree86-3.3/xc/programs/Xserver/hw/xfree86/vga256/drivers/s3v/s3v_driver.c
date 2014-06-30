/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/s3v/s3v_driver.c,v 1.1.2.10 1997/06/01 12:33:40 dawes Exp $ */

/*
 *
 * Copyright 1995-1997 The XFree86 Project, Inc.
 *
 */


/* This is an intial version of the ViRGE driver for XAA 
 * Started 09/03/97 by S. Marineau
 *
 * What works: 
 * - Supports PCI hardware, ViRGE and ViRGE/VX, probably ViRGE/DXGX
 * - Supports 8bpp, 16bpp and 24bpp. There is some support for 32bpp.
 * - VT switching seems to work well, no corruption. 
 * - A whole slew of XConfig options for memory, PCI and acceleration
 * - Acceleration is quite complete
 * 
 * 
 * What does not work:
 * - None of this doublescan stuff
 * - No hardware cursor etc.
 * - No VLB
 *
 * 
 * What I attempt to do here:
 *
 *  - Rewrite the init/save functions from the accel server such that they
 *    work as XAA intends
 *  - Keep the driver variables inside a private data structure, to avoid having
 *    millions of global variables.
 *  - Keep the structure as simple as possible, try to move RAMDAC stuff to 
 *    separate source files.
 *
 *  So much for that.... We cannot use the s3 ramdac code, because we 
 *  want to wait before we write out any registers. Fortunately, the 
 *  built-in virge ramdac is straighforward to set-up. Also, I did not succeed
 *  in keeping the code as modular as I had wished... 
 *
 *
 * Notes:
 * - The driver only supports linear addressing and MMIO. 
 *
 */


/* General and xf86 includes */
#include "X.h"
#include "input.h"
#include "screenint.h"
#include "compiler.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "vga.h"

/* Includes for Options flags */
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

/* DGA includes */
#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

/* S3V internal includes */
#include "s3v_driver.h"
#include "regs3v.h"

static Bool    S3VProbe();
static char *  S3VIdent();
static Bool    S3VClockSelect();
static void    S3VEnterLeave();
static Bool    S3VInit();
static int     S3VValidMode();
static void *  S3VSave();
static void    S3VRestore();
static void    S3VAdjust();
static void    S3VFbInit();
void           S3VSetRead();
void           S3VAccelInit();
void           S3VAccelInit32();
void           S3VInitSTREAMS();
void           S3VDisableSTREAMS();
void           S3VRestoreSTREAMS();
void           S3VSaveSTREAMS();

/* Temporary debug function to print virge regs */
void S3VPrintRegs();

/*
 * And the data structure which defines the driver itself 
 * This is mostly the struct from the s3 driver with adjusted names 
 * and adjusted default values.
 */

vgaVideoChipRec S3V = {
  S3VProbe,              /* Bool (* ChipProbe)() */
  S3VIdent,              /* char * (* ChipIdent)() */
  S3VEnterLeave,         /* void (* ChipEnterLeave)() */
  S3VInit,               /* Bool (* ChipInit)() */
  S3VValidMode,          /* int (* ChipValidMode)() */
  S3VSave,               /* void * (* ChipSave)() */
  S3VRestore,            /* void (* ChipRestore)() */
  S3VAdjust,             /* void (* ChipAdjust)() */
  vgaHWSaveScreen,       /* void (* ChipSaveScreen)() */
  (void(*)())NoopDDA,    /* void (* ChipGetMode)() */
  S3VFbInit,             /* void (* ChipFbInit)() */
  (void (*)())NoopDDA,   /* void (* ChipSetRead)() */
  (void (*)())NoopDDA,   /* void (* ChipSetWrite)() */
  (void (*)())NoopDDA,   /* void (* ChipSetReadWrite)() */
  0x10000,              /* int ChipMapSize */
  0x10000,              /* int ChipSegmentSize */
  16,                   /* int ChipSegmentShift */
  0xFFFF,               /* int ChipSegmentMask */
  0x00000, 0x10000,      /* int ChipReadBottom, int ChipReadTop */
  0x00000, 0x10000,     /* int ChipWriteBottom, int ChipWriteTop */
  FALSE,                /* Bool ChipUse2Banks */
  VGA_DIVIDE_VERT,      /* int ChipInterlaceType */
  {0,},                 /* OFlagSet ChipOptionFlags */
  8,                    /* int ChipRounding */
  TRUE,                 /* Bool ChipUseLinearAddressing */
  0,                    /* int ChipLinearBase */
  0,                    /* int ChipLinearSize */
  /*
   * This is TRUE if the driver has support for the given depth for
   * the detected configuration. It must be set in the Probe function.
   * It most cases it should be FALSE.
   */
  TRUE,        /* 16bpp */
  TRUE,        /* 24bpp */
  TRUE,        /* 32bpp */
  NULL,                 /* DisplayModePtr ChipBuiltinModes */
	/*
	 * This is a factor that can be used to scale the raw clocks
	 * to pixel clocks.	 This is rarely used, and in most cases, set
	 * it to 1.
	 */
	1
};

/* entries must be in sequence with chipset numbers !! */
SymTabRec s3vChipTable[] = {
   { S3_UNKNOWN,   "unknown"},
   { S3_ViRGE,     "ViRGE"}, 
   { S3_ViRGE_VX,  "ViRGE/VX"},
   { S3_ViRGE_DXGX,  "ViRGE/DXGX"},
   { -1,           ""},
   };

/* Declare the private structure which stores all internal info */

S3VPRIV s3vPriv;


/* And other glabal vars to hold vga base regs and MMIO base mem pointer */

int vgaCRIndex, vgaCRReg;
pointer s3vMmioMem = NULL;   /* MMIO base address */
extern vgaHWCursorRec vgaHWCursor;



/* This function returns the string name for a supported chipset */

static char *
S3VIdent(n)
int n;
{
   char *chipset = "s3_virge";

   if(n == 0) return(chipset);
   else return NULL;

}


/* The EnterLeave function which en/dis access to IO ports and ext. regs */

static void 
S3VEnterLeave(enter)
Bool enter;
{
   static int enterCalled = FALSE;
   unsigned char tmp;

#ifdef XFreeXDGA
	if (vga256InfoRec.directMode&XF86DGADirectGraphics && !enter)
		return;
#endif

   if (enter){
      xf86ClearIOPortList(vga256InfoRec.scrnIndex);
      xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
      xf86EnableIOPorts(vga256InfoRec.scrnIndex);

      /* Init the vgaIOBase reg index, depends on mono/color operation */
      vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
      vgaCRIndex = vgaIOBase + 4;
      vgaCRReg = vgaIOBase + 5;

      /* Unprotect CRTC[0-7]  */
      outb(vgaCRIndex, 0x11);      /* for register CR11 */
      tmp = inb(vgaCRReg);         /* enable CR0-7 and disable interrupts */
      outb(vgaCRReg, tmp & 0x7f);

      /* And unlock extended regs */
      outb(vgaCRIndex, 0x38);      /* for register CR38, (REG_LOCK1) */
      outb(vgaCRReg, 0x48);        /* unlock S3 register set for read/write */
      outb(vgaCRIndex, 0x39);    
      outb(vgaCRReg, 0xa5);
      outb(vgaCRIndex, 0x40);
      tmp = inb(vgaCRIndex);     
      outb(vgaCRReg, tmp | 0x01);   /* Unlocks extended functions */
      enterCalled = TRUE;
      }

   else {
      if (s3vMmioMem) {
	 unsigned char cr3a, cr53, cr66;
	 outb(vgaCRIndex, 0x53);
	 cr53 = inb(vgaCRReg);
	 outb(vgaCRReg, cr53 | 0x08);  /* Enable NEWMMIO temporarily */

	 outb(vgaCRIndex, 0x66);
	 cr66 = inb(vgaCRReg);
	 outb(vgaCRReg, cr66 | 0x80);
	 outb(vgaCRIndex, 0x3a);
	 cr3a = inb(vgaCRReg);
	 outb(vgaCRReg, cr3a | 0x80);

         WaitIdle();           /* DOn't know if these map properly ? */
         WaitCommandEmpty();   /* We should probably do a DMAEmpty() as well */

	 outb(vgaCRIndex, 0x53);
	 outb(vgaCRReg, cr53);   /* Restore CR53 to original for MMIO */

	 outb(vgaCRIndex, 0x66);
	 outb(vgaCRReg, cr66);
	 outb(vgaCRIndex, 0x3a);             
	 outb(vgaCRReg, cr3a);
         }
      if (enterCalled){

         /* Protect CR[0-7] */
         outb(vgaCRIndex, 0x11);      /* for register CR11 */
         tmp = inb(vgaCRReg);         /* disable CR0-7 */
         outb(vgaCRReg, (tmp & 0x7f) | 0x80);
      
         /* Relock extended regs-> To DO */
      
         xf86DisableIOPorts(vga256InfoRec.scrnIndex);
         enterCalled = FALSE;
         }
     }
}


/* 
 * This function is used to restore a video mode. It writes out all  
 * of the standart VGA and extended S3 registers needed to setup a 
 * video mode.
 *
 * Note that our life is made more difficult because of the STREAMS
 * processor which must be used for 24bpp. We need to disable STREAMS
 * before we switch video modes, or we risk locking up the machine. 
 * We also have to follow a certain order when reenabling it. 
 */

static void
S3VRestore (restore)
vgaS3VPtr restore;
{
unsigned char tmp, cr3a, cr53, cr66, cr67;

   vgaProtect(TRUE);

   /* Are we going to reenable STREAMS in this new mode? */
   s3vPriv.STREAMSRunning = restore->CR67 & 0x0c; 

   /* First reset GE to make sure nothing is going on */
   if(s3vPriv.chip == S3_ViRGE_VX) {
      outb(vgaCRIndex, 0x63);
      if(inb(vgaCRReg) & 0x01) S3VGEReset();
      }
   else {
      outb(vgaCRIndex, 0x66);
      if(inb(vgaCRReg) & 0x01) S3VGEReset();
      }

   /* As per databook, always disable STREAMS before changing modes */
   outb(vgaCRIndex, 0x67);
   cr67 = inb(vgaCRReg);
   if ((cr67 & 0x0c) == 0x0c) {
      S3VDisableSTREAMS();     /* If STREAMS was running, disable it */
      }

   /* Restore S3 extended regs */
   outb(vgaCRIndex, 0x63);             
   outb(vgaCRReg, restore->CR63);
   outb(vgaCRIndex, 0x66);             
   outb(vgaCRReg, restore->CR66);
   outb(vgaCRIndex, 0x3a);             
   outb(vgaCRReg, restore->CR3A);
   outb(vgaCRIndex, 0x31);    
   outb(vgaCRReg, restore->CR31);
   outb(vgaCRIndex, 0x58);             
   outb(vgaCRReg, restore->CR58);

   outb(0x3c4, 0x08);
   outb(0x3c5, 0x06); 
   outb(0x3c4, 0x12);
   outb(0x3c5, restore->SR12);
   outb(0x3c4, 0x13);
   outb(0x3c5, restore->SR13);
   outb(0x3c4, 0x15);
   outb(0x3c5, restore->SR15); 

   /* Restore the standard VGA registers */
   vgaHWRestore((vgaHWPtr)restore);

   /* Extended mode timings registers */  
   outb(vgaCRIndex, 0x53);             
   outb(vgaCRReg, restore->CR53); 
   outb(vgaCRIndex, 0x5d);     
   outb(vgaCRReg, restore->CR5D);
   outb(vgaCRIndex, 0x5e);             
   outb(vgaCRReg, restore->CR5E);
   outb(vgaCRIndex, 0x3b);             
   outb(vgaCRReg, restore->CR3B);
   outb(vgaCRIndex, 0x3c);             
   outb(vgaCRReg, restore->CR3C);
   outb(vgaCRIndex, 0x43);             
   outb(vgaCRReg, restore->CR43);
   outb(vgaCRIndex, 0x65);             
   outb(vgaCRReg, restore->CR65);


   /* Restore the desired video mode with CR67 */
        
   outb(vgaCRIndex, 0x67);             
   cr67 = inb(vgaCRReg) & 0xf; /* Possible hardware bug on VX? */
   outb(vgaCRReg, 0x50 | cr67); 
   outb(vgaCRReg, restore->CR67 & ~0x0c); /* Don't enable STREAMS yet */

   /* Other mode timing and extended regs */
   outb(vgaCRIndex, 0x34);             
   outb(vgaCRReg, restore->CR34);
   outb(vgaCRIndex, 0x42);             
   outb(vgaCRReg, restore->CR42);
   outb(vgaCRIndex, 0x51);             
   outb(vgaCRReg, restore->CR51);
   outb(vgaCRIndex, 0x54);             
   outb(vgaCRReg, restore->CR54);
   
   /* Memory timings */
   outb(vgaCRIndex, 0x36);             
   outb(vgaCRReg, restore->CR36);
   outb(vgaCRIndex, 0x68);             
   outb(vgaCRReg, restore->CR68);
   outb(vgaCRIndex, 0x69);
   outb(vgaCRReg, restore->CR69);

   outb(vgaCRIndex, 0x33);
   outb(vgaCRReg, restore->CR33);
   if (s3vPriv.chip == S3_ViRGE_DXGX) {
      outb(vgaCRIndex, 0x86);
      outb(vgaCRReg, restore->CR86);
      outb(vgaCRIndex, 0x90);
      outb(vgaCRReg, restore->CR90);
   }

   /* Unlock extended sequencer regs */
   outb(0x3c4, 0x08);
   outb(0x3c5, 0x06); 


   /* Restore extended sequencer regs for MCLK. SR10 == 255 indicates that 
    * we should leave the default SR10 and SR11 values there.
    */

   if (restore->SR10 != 255) {   
       outb(0x3c4, 0x10);
       outb(0x3c5, restore->SR10);
       outb(0x3c4, 0x11);
       outb(0x3c5, restore->SR11);
       }

   /* Restore extended sequencer regs for DCLK */
   outb(0x3c4, 0x12);
   outb(0x3c5, restore->SR12);
   outb(0x3c4, 0x13);
   outb(0x3c5, restore->SR13);

   outb(0x3c4, 0x18);
   outb(0x3c5, restore->SR18); 

   /* Load new m,n PLL values for DCLK & MCLK */
   outb(0x3c4, 0x15);
   tmp = inb(0x3c5) & ~0x21;

   outb(0x3c5, tmp | 0x03);
   outb(0x3c5, tmp | 0x23);
   outb(0x3c5, tmp | 0x03);
   outb(0x3c5, restore->SR15);


   /* Now write out CR67 in full, possibly starting STREAMS */
   
   VerticalRetraceWait();
   outb(vgaCRIndex, 0x67);    
   outb(vgaCRReg, 0x50);   /* For possible bug on VX?! */          
   outb(vgaCRReg, restore->CR67); 


   /* And finally, we init the STREAMS processor if we have CR67 indicate 24bpp
    * We also restore FIFO and TIMEOUT memory controller registers.
    */

      outb(vgaCRIndex, 0x53);
   cr53 = inb(vgaCRReg);
   outb(vgaCRReg, cr53 | 0x08);  /* Enable NEWMMIO temporarily */

   outb(vgaCRIndex, 0x66);
   cr66 = inb(vgaCRReg);
   outb(vgaCRReg, cr66 | 0x80);
   outb(vgaCRIndex, 0x3a);
   cr3a = inb(vgaCRReg);
   outb(vgaCRReg, cr3a | 0x80);

   if (s3vPriv.NeedSTREAMS) {
      if(s3vPriv.STREAMSRunning) S3VRestoreSTREAMS(restore->STREAMS);
      }

   /* Now, before we continue, check if this mode has the graphic engine ON 
    * If yes, then we reset it. 
    * This fixes some problems with corruption at 24bpp with STREAMS
    * Also restore the MIU registers. 
    */


   if(s3vPriv.chip == S3_ViRGE_VX) {
      if(restore->CR63 & 0x01) S3VGEReset();
      }
   else {
      if(restore->CR66 & 0x01) S3VGEReset();
      }

   VerticalRetraceWait();
   ((mmtr)s3vMmioMem)->memport_regs.regs.fifo_control = restore->MMPR0;
   WaitIdle();                  /* Don't ask... */
   ((mmtr)s3vMmioMem)->memport_regs.regs.miu_control = restore->MMPR1;
   WaitIdle();                  
   ((mmtr)s3vMmioMem)->memport_regs.regs.streams_timeout = restore->MMPR2;
   WaitIdle();
   ((mmtr)s3vMmioMem)->memport_regs.regs.misc_timeout = restore->MMPR3;

   outb(vgaCRIndex, 0x53);
   outb(vgaCRReg, cr53);   /* Restore CR53 to original for MMIO */

   outb(vgaCRIndex, 0x66);             
   outb(vgaCRReg, cr66);
   outb(vgaCRIndex, 0x3a);             
   outb(vgaCRReg, cr3a);

   if (xf86Verbose > 1) {
      ErrorF("\n\nViRGE driver: done restoring mode, dumping CR registers:\n\n");
      S3VPrintRegs();
   }

   vgaProtect(FALSE);

}

/* 
 * This function performs the inverse of the restore function: It saves all
 * the standard and extended registers that we are going to modify to set
 * up a video mode. Again, we also save the STREAMS context if it is needed.
 */

static void *
S3VSave (save)
vgaS3VPtr save;
{
int i;
unsigned char cr3a, cr53, cr66;

   /*
    * This function will handle creating the data structure and filling
    * in the generic VGA portion.
    */

   save = (vgaS3VPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaS3VRec));

   /* First unlock extended sequencer regs */
   outb(0x3c4, 0x08);
   outb(0x3c5, 0x06); 

   /* Now we save all the s3 extended regs we need */
   outb(vgaCRIndex, 0x31);             
   save->CR31 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x34);             
   save->CR34 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x36);             
   save->CR36 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x3a);             
   save->CR3A = inb(vgaCRReg);
   outb(vgaCRIndex, 0x42);             
   save->CR42 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x51);             
   save->CR51 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x53);             
   save->CR53 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x54);             
   save->CR54 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x58);             
   save->CR58 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x63);
   save->CR63 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x66);             
   save->CR66 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x67);             
   save->CR67 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x68);             
   save->CR68 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x69);
   save->CR69 = inb(vgaCRReg);

   outb(vgaCRIndex, 0x33);             
   save->CR33 = inb(vgaCRReg);
   if (s3vPriv.chip == S3_ViRGE_DXGX) {
      outb(vgaCRIndex, 0x86);
      save->CR86 = inb(vgaCRReg);
      outb(vgaCRIndex, 0x90);
      save->CR90 = inb(vgaCRReg);
   }

   /* Extended mode timings regs */

   outb(vgaCRIndex, 0x3b);             
   save->CR3B = inb(vgaCRReg);
   outb(vgaCRIndex, 0x3c);             
   save->CR3C = inb(vgaCRReg);
   outb(vgaCRIndex, 0x43);             
   save->CR43 = inb(vgaCRReg);
   outb(vgaCRIndex, 0x5d);             
   save->CR5D = inb(vgaCRReg);
   outb(vgaCRIndex, 0x5e);
   save->CR5E = inb(vgaCRReg);  
   outb(vgaCRIndex, 0x65);             
   save->CR65 = inb(vgaCRReg);


   /* Save sequencer extended regs for DCLK PLL programming */

   outb(0x3c4, 0x10);
   save->SR10 = inb(0x3c5);
   outb(0x3c4, 0x11);
   save->SR11 = inb(0x3c5);

   outb(0x3c4, 0x12);
   save->SR12 = inb(0x3c5);
   outb(0x3c4, 0x13);
   save->SR13 = inb(0x3c5);

   outb(0x3c4, 0x15);
   save->SR15 = inb(0x3c5);
   outb(0x3c4, 0x18);
   save->SR18 = inb(0x3c5);


   /* And if streams is to be used, save that as well */

      outb(vgaCRIndex, 0x53);
   cr53 = inb(vgaCRReg);
   outb(vgaCRReg, cr53 | 0x08);  /* Enable NEWMMIO to save MIU context */

   outb(vgaCRIndex, 0x66);
   cr66 = inb(vgaCRReg);
   outb(vgaCRReg, cr66 | 0x80);
   outb(vgaCRIndex, 0x3a);
   cr3a = inb(vgaCRReg);
   outb(vgaCRReg, cr3a | 0x80);

   if(s3vPriv.NeedSTREAMS) {
      S3VSaveSTREAMS(save->STREAMS);
      }

   /* Now save Memory Interface Unit registers, enable MMIO for this */
   save->MMPR0 = ((mmtr)s3vMmioMem)->memport_regs.regs.fifo_control;
   save->MMPR1 = ((mmtr)s3vMmioMem)->memport_regs.regs.miu_control;
   save->MMPR2 = ((mmtr)s3vMmioMem)->memport_regs.regs.streams_timeout;
   save->MMPR3 = ((mmtr)s3vMmioMem)->memport_regs.regs.misc_timeout;

   if (xf86Verbose > 1) {
      /* Debug */
      ErrorF("MMPR regs: %08x %08x %08x %08x\n",
         ((mmtr)s3vMmioMem)->memport_regs.regs.fifo_control,
         ((mmtr)s3vMmioMem)->memport_regs.regs.miu_control,
         ((mmtr)s3vMmioMem)->memport_regs.regs.streams_timeout,
         ((mmtr)s3vMmioMem)->memport_regs.regs.misc_timeout );

      ErrorF("\n\nViRGE driver: saved current video mode. Register dump:\n\n");
   }
   outb(vgaCRIndex, 0x53);
   outb(vgaCRReg, cr53);   /* Restore CR53 to original for MMIO */

   outb(vgaCRIndex, 0x3a);
   outb(vgaCRReg, cr3a);
   outb(vgaCRIndex, 0x66);
   outb(vgaCRReg, cr66);

   if (xf86Verbose > 1) S3VPrintRegs();

   return ((void *) save);
}



static unsigned char *find_bios_string(int BIOSbase, char *match1, char *match2)
{
#define BIOS_BSIZE 1024
#define BIOS_BASE  0xc0000

   static unsigned char bios[BIOS_BSIZE];
   static int init=0;
   int i,j,l1,l2;

   if (!init) {
      init = 1;
      if (xf86ReadBIOS(BIOSbase, 0, bios, BIOS_BSIZE) != BIOS_BSIZE)
	 return NULL;
      if ((bios[0] != 0x55) || (bios[1] != 0xaa))
	 return NULL;
   }
   if (match1 == NULL)
      return NULL;

   l1 = strlen(match1);
   if (match2 != NULL) 
      l2 = strlen(match2);
   else	/* for compiler-warnings */
      l2 = 0;

   for (i=0; i<BIOS_BSIZE-l1; i++)
      if (bios[i] == match1[0] && !memcmp(&bios[i],match1,l1))
	 if (match2 == NULL) 
	    return &bios[i+l1];
	 else
	    for(j=i+l1; (j<BIOS_BSIZE-l2) && bios[j]; j++) 
	       if (bios[j] == match2[0] && !memcmp(&bios[j],match2,l2))
		  return &bios[j+l2];
   return NULL;
}


/* 
 * This is the main probe function for the virge chipsets.
 * Right now, I have taken a shortcut and get most of the info from
 * PCI probing. Some code will have to be added to support VLB cards.
 */

static Bool
S3VProbe()
{
S3PCIInformation *pciInfo = NULL;
unsigned char config1, config2, m, n, n1, n2, cr66;
int mclk;

   if (vga256InfoRec.chipset) {
      if (StrCaseCmp(vga256InfoRec.chipset,S3VIdent(0)))
      return(FALSE);
   } 

   /* Start with PCI probing, this should get us quite far already */
   /* For now, we only use the PCI probing; add in later VLB */

   pciInfo = s3vGetPCIInfo();
   if (!pciInfo)
      return FALSE;

   if (pciInfo && pciInfo->MemBase)
      vga256InfoRec.MemBase = pciInfo->MemBase;
   if (pciInfo)
      if(pciInfo->ChipType != S3_ViRGE && 
         pciInfo->ChipType != S3_ViRGE_VX &&
	 pciInfo->ChipType != S3_ViRGE_DXGX){
          if (xf86Verbose > 1)
             ErrorF("%s %s: Unsupported (non-ViRGE) S3 chipset detected!\n", 
                XCONFIG_PROBED, vga256InfoRec.name);
          return FALSE;
          }
      else {
         s3vPriv.chip = pciInfo->ChipType;
         ErrorF("%s %s: Detected S3 %s\n",XCONFIG_PROBED,
            vga256InfoRec.name, xf86TokenToString(s3vChipTable, s3vPriv.chip));
         ErrorF("%s %s: using driver for chipset \"%s\"\n",XCONFIG_PROBED, 
            vga256InfoRec.name, S3VIdent(0));
	 }
   
   vga256InfoRec.chipset = S3VIdent(0);

#ifdef __alpha__
   if (xf86bpp > 16)
     FatalError("%s %s: %d bpp not yet supported for Alpha/AXP\n",
		XCONFIG_GIVEN, vga256InfoRec.name, xf86bpp);
#endif

   /* Add/enable IO ports to list: call EnterLeave */
   S3VEnterLeave(ENTER);

   /* Unlock sys regs */
   outb(vgaCRIndex, 0x38);
   outb(vgaCRReg, 0x48);
 
   /* Next go on to detect amount of installed ram */

   outb(vgaCRIndex, 0x36);              /* for register CR36 (CONFG_REG1), */
   config1 = inb(vgaCRReg);              /* get amount of vram installed */

   outb(vgaCRIndex, 0x37);              /* for register CR37 (CONFG_REG2), */
   config2 = inb(vgaCRReg);             /* get amount of off-screen ram  */

   outb(vgaCRIndex, 0x30);
   s3vPriv.ChipId = inb(vgaCRReg);         /* get chip id */
   outb(vgaCRIndex, 0x2e);
   s3vPriv.ChipId |= (inb(vgaCRReg) << 8);

   /* And compute the amount of video memory and offscreen memory */
   s3vPriv.MemOffScreen = 0;
   if (!vga256InfoRec.videoRam) {
      if (s3vPriv.chip == S3_ViRGE_VX) {
         switch((config2 & 0x60) >> 5) {
         case 1:
            s3vPriv.MemOffScreen = 4 * 1024;
            break;
         case 2:
            s3vPriv.MemOffScreen = 2 * 1024;
            break;
         }
         switch ((config1 & 0x60) >> 5) {
         case 0:
            vga256InfoRec.videoRam = 2 * 1024;
            break;
         case 1:
            vga256InfoRec.videoRam = 4 * 1024;
            break;
         case 2:
            vga256InfoRec.videoRam = 6 * 1024;
            break;
         case 3:
            vga256InfoRec.videoRam = 8 * 1024;
            break;
         }
         vga256InfoRec.videoRam -= s3vPriv.MemOffScreen;
      }
      else {
         switch((config1 & 0xE0) >> 5) {
         case 0:
            vga256InfoRec.videoRam = 4 * 1024;
            break;
         case 4:
            vga256InfoRec.videoRam = 2 * 1024;
            break;
         case 6:
            vga256InfoRec.videoRam = 1 * 1024;
            break;
         }
      }

      if (xf86Verbose) {
         if (s3vPriv.MemOffScreen)
            ErrorF("%s %s: videoram:  %dk (plus %dk off-screen)\n",
                   XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.videoRam,
                   s3vPriv.MemOffScreen);
         else
            ErrorF("%s %s: videoram:  %dk\n",
                   XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.videoRam);
      }
   } else {
      if (xf86Verbose) {
         ErrorF("%s %s: videoram:  %dk\n",
              XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.videoRam);
      }
   }


   /* reset S3 graphics engine to avoid memory corruption */
   if (s3vPriv.chip != S3_ViRGE_VX) {
      outb(vgaCRIndex, 0x66);
      cr66 = inb(vgaCRReg);
      outb(vgaCRReg, cr66 | 0x02);
      usleep(10000);  /* wait a little bit... */
   }

   if (find_bios_string(vga256InfoRec.BIOSbase,"S3 86C325",
			"MELCO WGP-VG VIDEO BIOS") != NULL) {
      if (xf86Verbose)
	 ErrorF("%s %s: MELCO BIOS found\n",
		XCONFIG_PROBED, vga256InfoRec.name);
      if (vga256InfoRec.MemClk <= 0)       vga256InfoRec.MemClk       =  74000;
      if (vga256InfoRec.dacSpeeds[0] <= 0) vga256InfoRec.dacSpeeds[0] = 191500;
      if (vga256InfoRec.dacSpeeds[1] <= 0) vga256InfoRec.dacSpeeds[1] = 162500;
      if (vga256InfoRec.dacSpeeds[2] <= 0) vga256InfoRec.dacSpeeds[2] = 111500;
      if (vga256InfoRec.dacSpeeds[3] <= 0) vga256InfoRec.dacSpeeds[3] =  83500;
   }

   if (s3vPriv.chip != S3_ViRGE_VX) {
      outb(vgaCRIndex, 0x66);
      outb(vgaCRReg, cr66 & ~0x02);  /* clear reset flag */
      usleep(10000);  /* wait a little bit... */
   }

   /* ViRGE built-in ramdac speeds */

   if (vga256InfoRec.dacSpeeds[3] <= 0 && vga256InfoRec.dacSpeeds[2] > 0)
      vga256InfoRec.dacSpeeds[3] = vga256InfoRec.dacSpeeds[2];

   if (s3vPriv.chip == S3_ViRGE_VX) {
      if (vga256InfoRec.dacSpeeds[0] <= 0) vga256InfoRec.dacSpeeds[0] = 220000;
      if (vga256InfoRec.dacSpeeds[1] <= 0) vga256InfoRec.dacSpeeds[1] = 220000;
      if (vga256InfoRec.dacSpeeds[2] <= 0) vga256InfoRec.dacSpeeds[2] = 135000;
      if (vga256InfoRec.dacSpeeds[3] <= 0) vga256InfoRec.dacSpeeds[3] = 135000;
   }
   else if (s3vPriv.chip == S3_ViRGE_DXGX) {
      if (vga256InfoRec.dacSpeeds[0] <= 0) vga256InfoRec.dacSpeeds[0] = 170000;
      if (vga256InfoRec.dacSpeeds[1] <= 0) vga256InfoRec.dacSpeeds[1] = 170000;
      if (vga256InfoRec.dacSpeeds[2] <= 0) vga256InfoRec.dacSpeeds[2] = 135000;
      if (vga256InfoRec.dacSpeeds[3] <= 0) vga256InfoRec.dacSpeeds[3] = 135000;
   }
   else {
      if (vga256InfoRec.dacSpeeds[0] <= 0) vga256InfoRec.dacSpeeds[0] = 135000;
      if (vga256InfoRec.dacSpeeds[1] <= 0) vga256InfoRec.dacSpeeds[1] =  95000;
      if (vga256InfoRec.dacSpeeds[2] <= 0) vga256InfoRec.dacSpeeds[2] =  57000;
      if (vga256InfoRec.dacSpeeds[3] <= 0) vga256InfoRec.dacSpeeds[3] =  57000;
   }

   if (vga256InfoRec.dacSpeedBpp <= 0)
      if (xf86bpp > 24 && vga256InfoRec.dacSpeeds[3] > 0)
	 vga256InfoRec.dacSpeedBpp = vga256InfoRec.dacSpeeds[3];
      else if (xf86bpp >= 24 && vga256InfoRec.dacSpeeds[2] > 0)
	 vga256InfoRec.dacSpeedBpp = vga256InfoRec.dacSpeeds[2];
      else if (xf86bpp > 8 && xf86bpp < 24 && vga256InfoRec.dacSpeeds[1] > 0)
	 vga256InfoRec.dacSpeedBpp = vga256InfoRec.dacSpeeds[1];
      else if (xf86bpp <= 8 && vga256InfoRec.dacSpeeds[0] > 0)
	 vga256InfoRec.dacSpeedBpp = vga256InfoRec.dacSpeeds[0];

   if (xf86Verbose) {
      ErrorF("%s %s: Ramdac speed: %d MHz",
	     OFLG_ISSET(XCONFIG_DACSPEED, &vga256InfoRec.xconfigFlag) ?
	     XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name,
	     vga256InfoRec.dacSpeeds[0] / 1000);
      if (vga256InfoRec.dacSpeedBpp != vga256InfoRec.dacSpeeds[0])
	 ErrorF("  (%d MHz for %d bpp)",vga256InfoRec.dacSpeedBpp / 1000, xf86bpp);
      ErrorF("\n");
   }


   /* Now set RAMDAC limits */
   vga256InfoRec.maxClock = vga256InfoRec.dacSpeedBpp;

   /* Detect current MCLK and print it for user */
   outb(0x3c4, 0x08);
   outb(0x3c5, 0x06); 
   outb(0x3c4, 0x10);
   n = inb(0x3c5);
   outb(0x3c4, 0x11);
   m = inb(0x3c5);
   m &= 0x7f;
   n1 = n & 0x1f;
   n2 = (n>>5) & 0x03;
   mclk = ((1431818 * (m+2)) / (n1+2) / (1 << n2) + 50) / 100;
   ErrorF("%s %s: Detected current MCLK value of %1.3f MHz\n",XCONFIG_PROBED, 
      vga256InfoRec.name, mclk / 1000.0);


   /* Now check if the user has specified "set_memclk" value in XConfig */
   if (vga256InfoRec.MemClk > 0) {
      if(vga256InfoRec.MemClk <= 100000) {
         ErrorF("%s %s: Using Memory Clock value of %1.3f MHz\n",
		OFLG_ISSET(XCONFIG_DACSPEED, &vga256InfoRec.xconfigFlag) ?
		XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name, 
		vga256InfoRec.MemClk/1000.0);
         s3vPriv.MCLK = vga256InfoRec.MemClk;
         }
      else {
         ErrorF("%s %s: Memory Clock value of %1.3f MHz is larger than limit of 100 MHz\n",
              XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.MemClk/1000.0);
         s3vPriv.MCLK = 0;
         }
      }
   else s3vPriv.MCLK = 0;

   /* Set scale factors for mode timings */

   if (s3vPriv.chip == S3_ViRGE_VX){
      s3vPriv.HorizScaleFactor = 1;
      }
   else if (vgaBitsPerPixel == 8){
      s3vPriv.HorizScaleFactor = 1;
      }
   else if (vgaBitsPerPixel == 16){
      s3vPriv.HorizScaleFactor = 2;
      }
   else {     
      s3vPriv.HorizScaleFactor = 1;
      }


   /* And map MMIO memory, abort if we cannot */

   s3vMmioMem = xf86MapVidMem(vga256InfoRec.scrnIndex, MMIO_REGION, 
      (pointer)(vga256InfoRec.MemBase + S3_NEWMMIO_REGBASE), S3_NEWMMIO_REGSIZE);

   if(s3vMmioMem == NULL) 
      FatalError("S3 ViRGE: Cannot map MMIO registers!\n");
   
   /* Determine if we need to use the STREAMS processor */

   if (vgaBitsPerPixel >= 24) s3vPriv.NeedSTREAMS = TRUE;
      else s3vPriv.NeedSTREAMS = FALSE;
   s3vPriv.STREAMSRunning = FALSE;

   /* And finally set various possible option flags */

   vga256InfoRec.bankedMono = FALSE;

#ifdef XFreeXDGA
   vga256InfoRec.directMode = XF86DGADirectPresent;
#endif

   OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
   OFLG_SET(OPTION_SLOW_EDODRAM, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_FAST_DRAM, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_FPM_VRAM, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_PCI_BURST_ON, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_FIFO_CONSERV, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_FIFO_MODERATE, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_FIFO_AGGRESSIVE, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_PCI_RETRY, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_NOACCEL, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_HW_CURSOR, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_EARLY_RAS_PRECHARGE, &S3V.ChipOptionFlags);
   OFLG_SET(OPTION_LATE_RAS_PRECHARGE, &S3V.ChipOptionFlags);

   s3vPriv.NoPCIRetry = 1;
   S3V.ChipLinearBase = vga256InfoRec.MemBase;
   S3V.ChipLinearSize = vga256InfoRec.videoRam * 1024;

   return TRUE;   
}


/* This validates a given video mode. 
 * Right now, the checks are quite minimal.
 */

static int
S3VValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
   int mem;

/* Check horiz/vert total values */

   if(mode->HTotal*s3vPriv.HorizScaleFactor > 4088) {
      if (verbose)
         ErrorF("%s %s: %s: Horizontal mode timing overflow (%d)\n",
            XCONFIG_PROBED, vga256InfoRec.name,
            vga256InfoRec.chipset, mode->HTotal);
         return MODE_BAD;
         }
   if (mode->VTotal > 2047) {
      if(verbose)
          ErrorF("%s %s: %s: Vertical mode timing overflow (%d)\n",
                  XCONFIG_PROBED, vga256InfoRec.name,
                  vga256InfoRec.chipset, mode->VTotal);
          return MODE_BAD;
        }
   if((mode->Flags & V_INTERLACE) && (vgaBitsPerPixel >= 24)){
      if(verbose)
          ErrorF("%s %s: Interlace modes are not supported at %d bpp\n",
                  XCONFIG_PROBED, vga256InfoRec.name,
                  vgaBitsPerPixel);
          return MODE_BAD;
        }

   /* Now make sure we have enough vidram to support mode */
   mem = ((vga256InfoRec.displayWidth > mode->HDisplay) ? 
             vga256InfoRec.displayWidth : mode->HDisplay) 
             * (vga256InfoRec.bitsPerPixel / 8) * 
             vga256InfoRec.virtualY;
   if (mem > (1024 * vga256InfoRec.videoRam - 1024)) {
     ErrorF("%s %s: Mode \"%s\" requires %d of videoram, only %d is available\n",
         XCONFIG_PROBED, vga256InfoRec.name, mode->name, mem, 
         1024 * vga256InfoRec.videoRam - 1024);
     return MODE_BAD;
     }
 
/* Dont check anything else for now. Leave the warning, fix it later. */
   
   return MODE_OK;
}


/* Used to adjust start address in frame buffer. We use the new 
 * CR69 reg for this purpose instead of the older CR31/CR51 combo.
 * If STREAMS is running, we program the STREAMS start addr. registers. 
 */

static void
S3VAdjust(x, y)
int x, y;
{
int Base, hwidth;
unsigned char tmp;

   if(s3vPriv.STREAMSRunning == FALSE) {
      Base = ((y * vga256InfoRec.displayWidth + x)
		* (vgaBitsPerPixel / 8)) >> 2;

      /* Now program the start address registers */
      outw(vgaCRIndex, (Base & 0x00FF00) | 0x0C);
      outw(vgaCRIndex, ((Base & 0x00FF) << 8) | 0x0D);
      outb(vgaCRIndex, 0x69);
      outb(vgaCRReg, (Base & 0x0F0000) >> 16);   
      }
   else {          /* Change start address for STREAMS case */
      VerticalRetraceWait();
      if(s3vPriv.chip == S3_ViRGE_VX)
	((mmtr)s3vMmioMem)->streams_regs.regs.prim_fbaddr0 =
	  ((y * vga256InfoRec.displayWidth + (x & ~7)) * vgaBitsPerPixel / 8);
      else
	((mmtr)s3vMmioMem)->streams_regs.regs.prim_fbaddr0 =
	((y * vga256InfoRec.displayWidth + (x & ~3)) * vgaBitsPerPixel / 8);
      }

#ifdef XFreeXDGA
   if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
      /* Wait until vertical retrace is in progress. */
      VerticalRetraceWait();
   }
#endif

   return;
}


static Bool
S3VInit(mode)
DisplayModePtr mode;
{
unsigned char tmp;
int width,dclk;
int i, j;

   /* First we adjust the horizontal timings if needed */

   if(s3vPriv.HorizScaleFactor != 1)
      if (!mode->CrtcHAdjusted) {
             mode->CrtcHDisplay *= s3vPriv.HorizScaleFactor;
             mode->CrtcHSyncStart *= s3vPriv.HorizScaleFactor;
             mode->CrtcHSyncEnd *= s3vPriv.HorizScaleFactor;
             mode->CrtcHTotal *= s3vPriv.HorizScaleFactor;
             mode->CrtcHSkew *= s3vPriv.HorizScaleFactor;
             mode->CrtcHAdjusted = TRUE;
             }



   if(!vgaHWInit (mode, sizeof(vgaS3VRec)))
      return FALSE;

   /* Now we fill in the rest of the stuff we need for the virge */
   /* Start with MMIO, linear addr. regs */

   outb(vgaCRIndex, 0x3a);
   tmp = inb(vgaCRReg);
   if(!OFLG_ISSET(OPTION_PCI_BURST_ON, &vga256InfoRec.options)) 
      new->CR3A = tmp | 0x95;      /* ENH 256, no PCI burst! */
   else 
      new->CR3A = (tmp & 0x7f) | 0x15; /* ENH 256, PCI burst */

   new->CR53 = 0x08;     /* Enables MMIO */
   new->CR31 = 0x8c;     /* Dis. 64k window, en. ENH maps */    

   /* Enables S3D graphic engine and PCI disconnects */
   if(s3vPriv.chip == S3_ViRGE_VX){
      new->CR66 = 0x90;  
      new->CR63 = 0x09;
      }
   else {
      new->CR66 = 0x89; 
      new->CR63 = 0;
      }    

/* Now set linear addr. registers */
/* LAW size: we have 2 cases, 2MB, 4MB or >= 4MB for VX */

   outb(vgaCRIndex, 0x58);
   new->CR58 = inb(vgaCRReg) & 0x80;
   if(vga256InfoRec.videoRam == 2048){   
      new->CR58 |= 0x02 | 0x10; 
      }
   else {
      new->CR58 |= 0x03 | 0x10; /* 4MB window on virge, 8MB on VX */
      } 
   if(s3vPriv.chip == S3_ViRGE_VX)
      new->CR58 |= 0x40;
   if (OFLG_ISSET(OPTION_EARLY_RAS_PRECHARGE, &vga256InfoRec.options))
      new->CR58 |= 0x80;
   if (OFLG_ISSET(OPTION_LATE_RAS_PRECHARGE, &vga256InfoRec.options))
      new->CR58 &= 0x7f;

/* ** On PCI bus, no need to reprogram the linear window base address */

/* Now do clock PLL programming. Use the s3gendac function to get m,n */
/* Also determine if we need doubling etc. */

   dclk = vga256InfoRec.clock[mode->Clock];
   new->CR67 = 0x00;             /* Defaults */
   new->SR15 = 0x03 | 0x80; 
   new->SR18 = 0x00;
   new->CR43 = 0x00;
   new->CR65 = 0x20;
   new->CR54 = 0x00;
   
   /* Memory controller registers. Optimize for better graphics engine 
    * performance. These settings are adjusted/overridden below for other bpp/
    * XConfig options.The idea here is to give a longer number of contiguous
    * MCLK's to both refresh and the graphics engine, to diminish the 
    * relative penalty of 3 or 4 mclk's needed to setup memory transfers. 
    */
   new->MMPR0 = 0x010400; /* defaults */
   new->MMPR1 = 0x00;   
   new->MMPR2 = 0x0808;  
   new->MMPR3 = 0x08080810; 


   if (OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, &vga256InfoRec.options) || 
      OFLG_ISSET(OPTION_FIFO_MODERATE, &vga256InfoRec.options) ||
      OFLG_ISSET(OPTION_FIFO_CONSERV, &vga256InfoRec.options)) {
         new->MMPR1 = 0x0200;   /* Low P. stream waits before filling */
         new->MMPR2 = 0x1808;   /* Let the FIFO refill itself */
         new->MMPR3 = 0x08081810; /* And let the GE hold the bus for a while */
      }

   /* And setup here the new value for MCLK. We use the XConfig 
    * option "set_mclk", whose value gets stored in vga256InfoRec.s3MClk.
    * I'm not sure what the maximum "permitted" value should be, probably
    * 100 MHz is more than enough for now.  
    */

   if(s3vPriv.MCLK> 0) {
       commonCalcClock(s3vPriv.MCLK, 1, 1, 31, 0, 3,
	   135000, 270000, &new->SR11, &new->SR10);
       }
   else {
       new->SR10 = 255; /* This is a reserved value, so we use as flag */
       new->SR11 = 255; 
       }

   if(s3vPriv.chip == S3_ViRGE_VX){
       if (vgaBitsPerPixel == 8) {
          if (dclk <= 110000) new->CR67 = 0x00; /* 8bpp, 135MHz */
          else new->CR67 = 0x10;                /* 8bpp, 220MHz */
          }
       else if ((vgaBitsPerPixel == 16) && (vga256InfoRec.weight.green == 5)) {
          if (dclk <= 110000) new->CR67 = 0x20; /* 15bpp, 135MHz */
          else new->CR67 = 0x30;                /* 15bpp, 220MHz */
          }
       else if (vgaBitsPerPixel == 16) {
          if (dclk <= 110000) new->CR67 = 0x40; /* 16bpp, 135MHz */
          else new->CR67 = 0x50;                /* 16bpp, 220MHz */
          }
       else if ((vgaBitsPerPixel == 24) || (vgaBitsPerPixel == 32)) {
          new->CR67 = 0xd0 | 0x0c;              /* 24bpp, 135MHz, STREAMS */
          S3VInitSTREAMS(new->STREAMS, mode);
          new->MMPR0 = 0xc098;            /* Adjust FIFO slots */
          }
       commonCalcClock(dclk, 1, 1, 31, 0, 4, 
	   220000, 440000, &new->SR13, &new->SR12);

      }
   else {           /* Is this correct for DX/GX as well? */
      if (vgaBitsPerPixel == 8) {
         if(dclk > 80000) {                     /* We need pixmux */
            new->CR67 = 0x10;
            new->SR15 |= 0x10;                   /* Set DCLK/2 bit */
            new->SR18 = 0x80;                   /* Enable pixmux */
            }
         }
      else if ((vgaBitsPerPixel == 16) && (vga256InfoRec.weight.green == 5)) {
         new->CR67 = 0x30;                       /* 15bpp */
         }
      else if (vgaBitsPerPixel == 16) {
         new->CR67 = 0x50;
         }
      else if (vgaBitsPerPixel == 24) { 
         new->CR67 = 0xd0 | 0x0c;
         S3VInitSTREAMS(new->STREAMS, mode);
         new->MMPR0 = 0xc000;            /* Adjust FIFO slots */
         }
      else if (vgaBitsPerPixel == 32) { 
         new->CR67 = 0xd0 | 0x0c;
         S3VInitSTREAMS(new->STREAMS, mode);
         new->MMPR0 = 0x10000;            /* Still more FIFO slots */
         }
      commonCalcClock(dclk, 1, 1, 31, 0, 3, 
	135000, 270000, &new->SR13, &new->SR12);
      }

   /* Now adjust the value of the FIFO based upon options specified */
   if(OFLG_ISSET(OPTION_FIFO_MODERATE, &vga256InfoRec.options)) {
      if(vgaBitsPerPixel < 24)
         new->MMPR0 -= 0x8000;
      else 
         new->MMPR0 -= 0x4000;
      }
   else if(OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, &vga256InfoRec.options)) {
      if(vgaBitsPerPixel < 24)
         new->MMPR0 -= 0xc000;
      else 
         new->MMPR0 -= 0x6000;
      }

   /* If we have an interlace mode, set the interlace bit. Note that mode
    * vertical timings are already adjusted by the standard VGA code 
    */
   if(mode->Flags & V_INTERLACE) {
        new->CR42 = 0x20; /* Set interlace mode */
        }
   else {
        new->CR42 = 0x00;
        }

   /* Set display fifo */
   new->CR34 = 0x10;  

   /* Now we adjust registers for extended mode timings */
   /* This is taken without change from the accel/s3_virge code */

   i = ((((mode->CrtcHTotal >> 3) - 5) & 0x100) >> 8) |
       ((((mode->CrtcHDisplay >> 3) - 1) & 0x100) >> 7) |
       ((((mode->CrtcHSyncStart >> 3) - 1) & 0x100) >> 6) |
       ((mode->CrtcHSyncStart & 0x800) >> 7);

   if ((mode->CrtcHSyncEnd >> 3) - (mode->CrtcHSyncStart >> 3) > 64)
      i |= 0x08;   /* add another 64 DCLKs to blank pulse width */

   if ((mode->CrtcHSyncEnd >> 3) - (mode->CrtcHSyncStart >> 3) > 32)
      i |= 0x20;   /* add another 32 DCLKs to hsync pulse width */

   j = (  new->std.CRTC[0] + ((i&0x01)<<8)
        + new->std.CRTC[4] + ((i&0x10)<<4) + 1) / 2;

   if (j-(new->std.CRTC[4] + ((i&0x10)<<4)) < 4)
      if (new->std.CRTC[4] + ((i&0x10)<<4) + 4 <= new->std.CRTC[0]+ ((i&0x01)<<8))
         j = new->std.CRTC[4] + ((i&0x10)<<4) + 4;
      else
         j = new->std.CRTC[0]+ ((i&0x01)<<8) + 1;

   new->CR3B = j & 0xFF;
   i |= (j & 0x100) >> 2;
   new->CR3C = (new->std.CRTC[0] + ((i&0x01)<<8))/2;
   new->CR5D = i;

   new->CR5E = (((mode->CrtcVTotal - 2) & 0x400) >> 10)  |
               (((mode->CrtcVDisplay - 1) & 0x400) >> 9) |
               (((mode->CrtcVSyncStart) & 0x400) >> 8)   |
               (((mode->CrtcVSyncStart) & 0x400) >> 6)   | 0x40;

   
   width = (vga256InfoRec.displayWidth * (vgaBitsPerPixel / 8))>> 3;
   new->std.CRTC[19] = 0xFF & width;
   new->CR51 = (0x300 & width) >> 4; /* Extension bits */
   new->CR65 = 0x20;
   
   /* And finally, select clock source 2 for programmable PLL */
   new->std.MiscOutReg |= 0x0c;      


   new->CR33 = 0x20;
   if (s3vPriv.chip == S3_ViRGE_DXGX) {
      new->CR86 = 0x80;  /* disable DAC power saving to avoid bright left edge */
      new->CR90 = 0x00;  /* disable the stream display fetch length control */
   }
	 

   /* Now we handle various XConfig memory options and others */

   /* option "slow_edodram" sets EDO to 2 cycle mode on ViRGE */
   if (s3vPriv.chip == S3_ViRGE) {
      outb(vgaCRIndex, 0x36);
      new->CR36 = inb(vgaCRReg);
      if(OFLG_ISSET(OPTION_SLOW_EDODRAM, &vga256InfoRec.options)) 
         new->CR36 = (new->CR36 & 0xf3) | 0x08;
      else  
         new->CR36 &= 0xf3;
      }
   
   /* Option "fpm_vram" for ViRGE_VX sets memory in fast page mode */
   if (s3vPriv.chip == S3_ViRGE_VX) {
      outb(vgaCRIndex, 0x36);
      new->CR36 = inb(vgaCRReg);
      if(OFLG_ISSET(OPTION_FPM_VRAM, &vga256InfoRec.options)) 
         new->CR36 = new->CR36 | 0x0c;
      else 
         new->CR36 &= ~0x0c;
      }

   outb(vgaCRIndex, 0x68);
   new->CR68 = inb(vgaCRReg);
   new->CR69 = 0;


  return TRUE;
}

/* This function inits the frame buffer. Right now, it is is rather limited 
 * but the hardware cursor hooks should probably end up here 
 */

void 
S3VFbInit()
{

   /* Call S3VAccelInit to setup the XAA accelerated functions */
   if(!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
      if(vgaBitsPerPixel == 32) 
         S3VAccelInit32();
      else 
         S3VAccelInit();

   if(OFLG_ISSET(OPTION_PCI_RETRY, &vga256InfoRec.options))
      if(OFLG_ISSET(OPTION_PCI_BURST_ON, &vga256InfoRec.options)) {
         s3vPriv.NoPCIRetry = 0;
         }
      else {
         s3vPriv.NoPCIRetry = 1;   
         ErrorF("%s %s: \"pci_retry\" option requires \"pci_burst\".\n",
              XCONFIG_GIVEN, vga256InfoRec.name);
         }
   if (OFLG_ISSET(OPTION_HW_CURSOR, &vga256InfoRec.options)) {
      vgaHWCursor.Initialized = TRUE;
      vgaHWCursor.Init = S3VCursorInit;
      vgaHWCursor.Restore = S3VRestoreCursor;
      vgaHWCursor.Warp = S3VWarpCursor;
      vgaHWCursor.QueryBestSize = S3VQueryBestSize;
      if (xf86Verbose)
                ErrorF("%s %s: %s: Using hardware cursor\n",
                        XCONFIG_PROBED, vga256InfoRec.name,
                        vga256InfoRec.chipset);
      }


}


/* This function inits the STREAMS processor variables. 
 * This has essentially been taken from the accel/s3_virge code and the databook.
 */
void
S3VInitSTREAMS(streams, mode)
int * streams;
DisplayModePtr mode;
{
  
   if ( vga256InfoRec.bitsPerPixel == 24 ) {
                         /* data format 8.8.8 (24 bpp) */
      streams[0] = 0x06000000;
      } 
   else if (vga256InfoRec.bitsPerPixel == 32) {
                         /* one more bit for X.8.8.8, 32 bpp */
      streams[0] = 0x07000000;
   }
                         /* NO chroma keying... */
   streams[1] = 0x0;
                         /* Secondary stream format KRGB-16 */
                         /* data book suggestion... */
   streams[2] = 0x03000000;

   streams[3] = 0x0;

   streams[4] = 0x0;
                         /* use 0x01000000 for primary over second. */
                         /* use 0x0 for second over prim. */
   streams[5] = 0x01000000;

   streams[6] = 0x0;

   streams[7] = 0x0;
                                /* Stride is 3 bytes for 24 bpp mode and */
                                /* 4 bytes for 32 bpp. */
   if ( vga256InfoRec.bitsPerPixel == 24 ) {
      streams[8] = 
             vga256InfoRec.displayWidth * 3;
      } 
   else {
      streams[8] = 
             vga256InfoRec.displayWidth * 4;
      }
                                /* Choose fbaddr0 as stream source. */
   streams[9] = 0x0;
   streams[10] = 0x0;
   streams[11] = 0x0;
   streams[12] = 0x1;

                                /* Set primary stream on top of secondary */
                                /* stream. */
   streams[13] = 0xc0000000;
                               /* Vertical scale factor. */
   streams[14] = 0x0;

   streams[15] = 0x0;
                                /* Vertical accum. initial value. */
   streams[16] = 0x0;
                                /* X and Y start coords + 1. */
   streams[18] =  0x00010001;

         /* Specify window Width -1 and Height of */
         /* stream. */
   streams[19] =
         (mode->HDisplay - 1) << 16 |
         (mode->VDisplay);
   
                                /* Book says 0x07ff07ff. */
   streams[20] = 0x07ff07ff;

   streams[21] = 0x00010001;
                            
}

/* This function saves the STREAMS registers to our private structure */

void
S3VSaveSTREAMS(streams)
int * streams;
{

   streams[0] = ((mmtr)s3vMmioMem)->streams_regs.regs.prim_stream_cntl;
   streams[1] = ((mmtr)s3vMmioMem)->streams_regs.regs.col_chroma_key_cntl;
   streams[2] = ((mmtr)s3vMmioMem)->streams_regs.regs.second_stream_cntl;
   streams[3] = ((mmtr)s3vMmioMem)->streams_regs.regs.chroma_key_upper_bound;
   streams[4] = ((mmtr)s3vMmioMem)->streams_regs.regs.second_stream_stretch;
   streams[5] = ((mmtr)s3vMmioMem)->streams_regs.regs.blend_cntl;
   streams[6] = ((mmtr)s3vMmioMem)->streams_regs.regs.prim_fbaddr0;
   streams[7] = ((mmtr)s3vMmioMem)->streams_regs.regs.prim_fbaddr1;
   streams[8] = ((mmtr)s3vMmioMem)->streams_regs.regs.prim_stream_stride;
   streams[9] = ((mmtr)s3vMmioMem)->streams_regs.regs.double_buffer;
   streams[10] = ((mmtr)s3vMmioMem)->streams_regs.regs.second_fbaddr0;
   streams[11] = ((mmtr)s3vMmioMem)->streams_regs.regs.second_fbaddr1;
   streams[12] = ((mmtr)s3vMmioMem)->streams_regs.regs.second_stream_stride;
   streams[13] = ((mmtr)s3vMmioMem)->streams_regs.regs.opaq_overlay_cntl;
   streams[14] = ((mmtr)s3vMmioMem)->streams_regs.regs.k1;
   streams[15] = ((mmtr)s3vMmioMem)->streams_regs.regs.k2;
   streams[16] = ((mmtr)s3vMmioMem)->streams_regs.regs.dda_vert;
   streams[17] = ((mmtr)s3vMmioMem)->streams_regs.regs.streams_fifo;
   streams[18] = ((mmtr)s3vMmioMem)->streams_regs.regs.prim_start_coord;
   streams[19] = ((mmtr)s3vMmioMem)->streams_regs.regs.prim_window_size;
   streams[20] = ((mmtr)s3vMmioMem)->streams_regs.regs.second_start_coord;
   streams[21] = ((mmtr)s3vMmioMem)->streams_regs.regs.second_window_size;

}

/* This function restores the saved STREAMS registers */

void
S3VRestoreSTREAMS(streams)
int * streams;
{

/* For now, set most regs to their default values for 24bpp 
 * Restore only those that are needed for width/height/stride
 * Otherwise, we seem to get lockups because some registers 
 * when saved have some reserved bits set.
 */

   ((mmtr)s3vMmioMem)->streams_regs.regs.prim_stream_cntl = 
         streams[0] & 0x77000000;
   ((mmtr)s3vMmioMem)->streams_regs.regs.col_chroma_key_cntl = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.second_stream_cntl = 0x03000000;
   ((mmtr)s3vMmioMem)->streams_regs.regs.chroma_key_upper_bound = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.second_stream_stretch = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.blend_cntl = 0x01000000;
   ((mmtr)s3vMmioMem)->streams_regs.regs.prim_fbaddr0 = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.prim_fbaddr1 = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.prim_stream_stride = 
         streams[8] & 0x0fff;
   ((mmtr)s3vMmioMem)->streams_regs.regs.double_buffer = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.second_fbaddr0 = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.second_fbaddr1 = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.second_stream_stride = 0x01;
   ((mmtr)s3vMmioMem)->streams_regs.regs.opaq_overlay_cntl = 0x40000000;
   ((mmtr)s3vMmioMem)->streams_regs.regs.k1 = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.k2 = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.dda_vert = 0x00;
   ((mmtr)s3vMmioMem)->streams_regs.regs.prim_start_coord = 0x00010001;
   ((mmtr)s3vMmioMem)->streams_regs.regs.prim_window_size = 
         streams[19] & 0x07ff07ff;
   ((mmtr)s3vMmioMem)->streams_regs.regs.second_start_coord = 0x07ff07ff;
   ((mmtr)s3vMmioMem)->streams_regs.regs.second_window_size = 0x00010001;


}


/* And this function disables the STREAMS processor as per databook.
 * This is usefull before we do a mode change 
 */

void
S3VDisableSTREAMS()
{
unsigned char tmp;

   VerticalRetraceWait();
   ((mmtr)s3vMmioMem)->memport_regs.regs.fifo_control = 0xC000;
   outb(vgaCRIndex, 0x67);
   tmp = inb(vgaCRReg);
                         /* Disable STREAMS processor */
   outb( vgaCRReg, tmp & ~0x0C );


}

/* This function is used to debug, it prints out the contents of s3 regs */

void
S3VPrintRegs(void)
{
unsigned char tmp1, tmp2;

   outb(0x3c4, 0x10);
   tmp1 = inb(0x3c5);
   outb(0x3c4, 0x11);
   tmp2 = inb(0x3c5);
   ErrorF("SR10: %d SR11: %d\n", tmp1, tmp2);

   outb(0x3c4, 0x12);
   tmp1 = inb(0x3c5);
   outb(0x3c4, 0x13);
   tmp2 = inb(0x3c5);
   ErrorF("SR12: %d SR13: %d\n", tmp1, tmp2);

   outb(0x3c4, 0x0a);
   tmp1 = inb(0x3c5);
   outb(0x3c4, 0x15);
   tmp2 = inb(0x3c5);
   ErrorF("SR0A: %d SR15: %d\n", tmp1, tmp2);

   /* Now load and print a whole rnage of other regs */
for(tmp1=0x0;tmp1<=0x0f;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n");
for(tmp1=0x10;tmp1<=0x1f;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n");
for(tmp1=0x20;tmp1<=0x2f;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n");
for(tmp1=0x30;tmp1<=0x3f;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n");
for(tmp1=0x40;tmp1<=0x4f;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n");
for(tmp1=0x50;tmp1<=0x59;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n");
for(tmp1=0x5d;tmp1<=0x67;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n");
for(tmp1=0x68;tmp1<=0x6f;tmp1++){
   outb(vgaCRIndex, tmp1);
   ErrorF("CR%02x:%d ",tmp1,inb(vgaCRReg));
   }
   ErrorF("\n\n");
}
