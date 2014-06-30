/* $XConsortium: nv_driver.c /main/3 1996/10/28 05:13:37 kaleb $ */
/*
 * Copyright 1996  David J. McKay
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * DAVID J. MCKAY BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/nv/nv_driver.c,v 3.5.2.2 1997/05/09 07:15:44 hohndel Exp $ */

#include <math.h>
#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "vga.h"

#include "vgaPCI.h"
/*
 * If the driver makes use of XF86Config 'Option' flags, the following will be
 * required
 */
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"


#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

/*
 * This header is required for drivers that implement NVFbInit().
 */
#if !defined(MONOVGA) && !defined(XF86VGA16)
#include "vga256.h"
#endif

#define NV_1764_MAX_CLOCK_IN_KHZ 170000
#define NV_1732_MAX_CLOCK_IN_KHZ 135000

#define NV_MAX_VCLK_PIN_CLOCK_IN_KHZ  50000


#include "nvreg.h"

/* This holds the pointer to the ramdac micro port */
volatile int *nvPort = NULL;

/* Function to read extended register */

static int MapNvRegs(void *base)
{
  nvPort=(int *)xf86MapVidMem(vga256InfoRec.scrnIndex,LINEAR_REGION,
                              (pointer)base,NV_FRAME_BUFFER);
}

/*
 * Driver data structures.
 */
typedef struct {
  vgaHWRec std;			/* good old IBM VGA */
  /* These values hold the params for the programmable PLL */
  unsigned char Nparam, Mparam, Oparam, Pparam;
  unsigned char dacConfReg0;
  unsigned char dacConfReg1;
  unsigned char dacRgbPalCtrl;
  unsigned long confReg0;
  unsigned long memoryTrace;
  unsigned long startAddr; /* Where to start readin out from the buffer */
  /* All the following registers control the display */
  unsigned long prmConfig0;     /* Controls if text mode on or off */
  unsigned long horFrontPorch;  /* Front porch in pixels */
  unsigned long horSyncWidth;   /* Sync Width in pixels */
  unsigned long horBackPorch;   /* horizontal back porch in in pixels */
  unsigned long horDispWidth;   /* Horizontal display width in pixels */
  unsigned long verFrontPorch;  /* Vertical front porch in lines */
  unsigned long verSyncWidth;   /* Vertical sync width in lines */
  unsigned long verBackPorch;   /* Vertical back porch in lines */
  unsigned long verDispWidth;   /* Vertical display width in lines */
} vgaNVRec, *vgaNVPtr;

/*
 * Forward definitions for the functions that make up the driver.  See
 * the definitions of these functions for the real scoop.
 */
static Bool NVProbe(void);
static char *NVIdent(int n);
static void NVEnterLeave(Bool enter);
static Bool NVInit(DisplayModePtr mode);
static int NVValidMode(DisplayModePtr mode,Bool verbose, int flag);
static void *NVSave(void *data);
static void NVRestore(void *data);
static void NVAdjust(int x,int y);


vgaVideoChipRec NV =
{
  NVProbe,
  NVIdent,
  NVEnterLeave,
  NVInit,
  NVValidMode,
  NVSave,
  NVRestore,
  NVAdjust,
  vgaHWSaveScreen,
  (void (*)())NoopDDA,
  (void (*)())NoopDDA,
  (void (*)())NoopDDA,
  (void (*)())NoopDDA,
  (void (*)())NoopDDA,
  0x10000,/* This is the size of the mapped memory window, usually 64k    */
  0x10000,/* This is the size of a video memory bank for this chipset     */
  16,     /* Number of bits to shift addressto determine the bank number  */
  0xFFFF, /* Bitmask used to determine the address within a specific bank */
  0x00000, 0x10000,   /* Bottom and top addresses for reads inside a bank */
  0x00000, 0x10000,   /* Same for writes */         
  FALSE, /* True if chipset seperate read and write bank registers */
  VGA_NO_DIVIDE_VERT,  /* VGA_DIVIDE_VERT if vertical timing numbers 
                          to be divided by two for interlaced modes */
  {0,},  /* Option flags */	
  8,     /* Multiple to which the virtual width rounded */
  TRUE,  /* Support linear-mapped frame buffer */	
  0,     /* Physical base address of the linear-mapped frame buffer */	
  0  ,     /* Size  of the linear-mapped frame buffer */		
  TRUE,  /* Support for 16 bpp */
  FALSE, /* Support for 24 bpp */ 
  FALSE, /* Support for 32 bpp */	
  NULL,  /* Pointer to a list of builtin driver modes */   
  1,     /* Scale factor used to multiply the raw clocks to pixel clocks */
  1      /* Scale factor used to divide the raw clocks to pixel clocks */
};

/*
 * This is a convenience macro, so that entries in the driver structure
 * can simply be dereferenced with 'new->xxx'.
 */
#define new ((vgaNVPtr)vgaNewVideoState)

static char *NVIdent(int n)
{
  static char *chipsets[]={"NV1", "STG2000"};

  if(n + 1 > sizeof (chipsets) / sizeof (char *)) {
      return NULL;
  }else {
    return (chipsets[n]);
  }
}

#define VPLL_INPUT_FREQ 12096.0
#define PLL_LOWER_BOUND 64000
#define PLL_UPPER_BOUND vga256InfoRec.maxClock

/* 
 *  The following equation defines the output frequency of the PLL
 *  fout = ( N/(M*P) ) * fin;
 *
 *  It must also obey the following restraints!
 *  1Mhz <= fin/M <= 2Mhz (This means M in the range 12 -> 7)
 *
 *  64Mhz <= ((N*O)/M)*fin <=135 (or whatever the max ramdac speed is)
 *  
 *  The following function simple does a brute force search I'm afraid.
 *  I know that it is possible to write a much faster function, but 
 *  there are more important things to work on, and at least I know 
 *  this one will always get the right answer!
 */

static int NVClockSelect(float clockIn,float *clockOut,
                         int *Mparam,int *Nparam,int *Oparam,int *Pparam)
{
  int m,n,o,p;
  float bestDiff=1e10; /* Set to silly value for first range */
  float target=0.0;
  float best=0.0;

  *clockOut=0;
  for(p=1;p<=8;p*=2) {
    float fp=(float)p;
    for(n=8;n<=255;n++) {
      float fn=(float)n;
      for(m=12;m>=7;m--) {
        float fm=(float)m;
        for(o=1;o<=15;o++) {
          float fo=(float) o;
          float check= ((fn*fo)/fm)*VPLL_INPUT_FREQ;
          if(check>PLL_LOWER_BOUND && check <= PLL_UPPER_BOUND) break;
	}
        if(o!=16) {
          float diff;                    
          target=(fn/(fm*fp))*VPLL_INPUT_FREQ;
          diff=fabs(target - clockIn);
          if(diff < bestDiff) {
            bestDiff=diff;
            best=target; 
            *Mparam=m;*Nparam=n;*Oparam=o;*Pparam=p;
            *clockOut=best;
	  }
	}
      }
    }
  }
  return (best!=0.0);
}


static void * NVMemBase(void)
{
  int idx=0;
  void *ret=NULL;
  pciConfigPtr pcrp,*pciList;

  pciList=xf86scanpci(vga256InfoRec.scrnIndex);
  if(pciList==NULL) { 
    ErrorF("%s %s: %s: No PCI devices present\n",
	    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
    return NULL;
  }

  while(pcrp = pciList[idx]) {
#ifdef DEBUG
     ErrorF("PCI[%d]: Vendor:0x%X, Device:0x%X, Base:0x%X\n",
	    idx, pcrp->_vendor, pcrp->_device, pcrp->_base0);
#endif
     if((pcrp->_vendor == PCI_VENDOR_NVIDIA		
	&& (pcrp->_device & 0xFFFB) == PCI_CHIP_DAC64) ||
        (pcrp->_vendor == PCI_VENDOR_SGS
	&& (pcrp->_device & 0xFFFB) == PCI_CHIP_STG1764)) {
       if(pcrp->_base0 != 0) {
	 ret = (void*) (pcrp->_base0 & 0xFF800000);
	 break;
       }
     }
     idx++;
  }
  if(ret == NULL) {
    ErrorF("%s %s: %s: Can't find valid PCI Base Address\n",
	    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
  }
 return ret;
}

static int pixelPortWidth=0; /* Holds how big it is,needed in NVInit() */

static int ProbeRamdac(void)
{
  int id=ReadExtReg(NV_DAC_COMPANY_ID);

  if(id!=NV_DAC_SGS_ID) {
    ErrorF("Unsupported RAMDAC (vendor number 0x%02x)\n",id); 
    return 0;
  }
  /* Figure out what sort of RAMDAC we have. I don't know if the RAMDACS
   * have different PCI device numbers, but this test will always work
   */  
  switch(id=ReadExtReg(NV_DAC_DEVICE_ID)) {
    case NV_DAC_1764_ID:
      /* I believe all Diamond Edge3D boards use the 1764 */
      vga256InfoRec.maxClock = NV_1764_MAX_CLOCK_IN_KHZ;
      break;
    case NV_DAC_1732_ID:      
      vga256InfoRec.maxClock = NV_1732_MAX_CLOCK_IN_KHZ;
      break;
    default: 
      ErrorF("Unsupported SGS RAMDAC (id number 0x%02x)\n",id); 
      return 0;
      break;
  }
  /* We need to figure out what the pixel port width is. This depends 
   * on memory configuration etc, so we read it here so the NVInit() 
   * function can put it back in.
   */
   pixelPortWidth=GETBITFIELD(ReadExtReg(NV_DAC_CONF_0),
                              NV_DAC_CONF_0_PORT_WIDTH);
   return 1;
}

static Bool NVProbe(void)
{
  void *base;
  int ramdacId;

  /* Check if the user has forced the chipset */
  if(vga256InfoRec.chipset) {
    int i = 0;
    char *chipName;
    while((chipName = NVIdent (i++)) != NULL) {
      if(!StrCaseCmp(vga256InfoRec.chipset, chipName))
	break;
    }
    if(!chipName) return FALSE;
  }else {
    /* Now we must scan the PCI data structures to test for the NV1/STG200 */

    if(!vgaPCIInfo) return FALSE; /* No PCI - no NV */
     
    if(vgaPCIInfo->Vendor==PCI_VENDOR_NVIDIA && 
      vgaPCIInfo->ChipType==PCI_CHIP_NV1) {
      vga256InfoRec.chipset=NVIdent(0);
    }else if(vgaPCIInfo->Vendor==PCI_VENDOR_SGS && 
             vgaPCIInfo->ChipType==PCI_CHIP_STG2000) {
      vga256InfoRec.chipset=NVIdent(1);
    }else {
      return FALSE;
    }
  }

  /* I/o ports are needed for things like pallete selection etc */
  xf86ClearIOPortList (vga256InfoRec.scrnIndex);
  xf86AddIOPorts(vga256InfoRec.scrnIndex,Num_VGA_IOPorts,VGA_IOPorts);

  if((base=NVMemBase())==NULL) return FALSE;
  NV.ChipLinearBase=(int)base+NV_FRAME_BUFFER;
  /* Now memory map the registers */
  MapNvRegs((pointer)base); 

  /* Calculate how much RAM, unless user supplies */
  if(!vga256InfoRec.videoRam) {
    vga256InfoRec.videoRam = 1024 << 
      GETBITFIELD(nvPort[NV_PFB_BOOT_0],NV_PFB_BOOT_0_RAM_AMOUNT);
  }
  NV.ChipLinearSize=vga256InfoRec.videoRam*1024;

  /* Now force programmable clock */
  OFLG_SET(CLOCK_OPTION_PROGRAMABLE,&vga256InfoRec.clockOptions);
  vga256InfoRec.clocks = 0;

  if(!ProbeRamdac()) return FALSE;

#ifndef MONOVGA
#ifdef XFreeXDGA
  vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif
  vga256InfoRec.bankedMono = FALSE;
  
  NVEnterLeave(ENTER);
  vgaIOBase = (inb (0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
  return (TRUE);
}

/*
 * NVEnterLeave --
 *
 * This function is called when the virtual terminal on which the server
 * is running is entered or left, as well as when the server starts up
 * and is shut down.  Its function is to obtain and relinquish I/O 
 * permissions for the SVGA device.  This includes unlocking access to
 * any registers that may be protected on the chipset, and locking those
 * registers again on exit.
 */
static void NVEnterLeave(Bool enter)
{
  unsigned char temp;

#ifndef MONOVGA
#ifdef XFreeXDGA
  if(vga256InfoRec.directMode&XF86DGADirectGraphics && !enter) return;
#endif
#endif

  if (enter) {
    xf86EnableIOPorts (vga256InfoRec.scrnIndex);
  } else {
    xf86DisableIOPorts (vga256InfoRec.scrnIndex);
  }
}

/*
 * NVRestore --
 *
 * This function restores a video mode.  It basically writes out all of
 * the registers that have previously been saved in the vgaNVRec data 
 * structure.
 *
 * Note that "Restore" is a little bit incorrect.  This function is also
 * used when the server enters/changes video modes.  The mode definitions 
 * have previously been initialized by the Init() function, below.
 */
static void NVRestore(void *data)     
{
  vgaNVPtr restore = data;
  vgaProtect(TRUE);

  /*
   * This function handles restoring the generic VGA registers.
   */
  vgaHWRestore ((vgaHWPtr) restore);

  /* Set the clock registers */
  nvPort[NV_MEMORY_TRACE]=restore->memoryTrace;
  nvPort[NV_PRM_CONFIG_0]=restore->prmConfig0;

  WriteExtReg(NV_DAC_VPLL_M_PARAM,restore->Mparam);
  WriteExtReg(NV_DAC_VPLL_N_PARAM,restore->Nparam);
  WriteExtReg(NV_DAC_VPLL_O_PARAM,restore->Oparam);
  WriteExtReg(NV_DAC_VPLL_P_PARAM,restore->Pparam);

  /* Set the dac conf reg that sets the pixel depth */
  WriteExtReg(NV_DAC_CONF_0,restore->dacConfReg0);
  WriteExtReg(NV_DAC_CONF_1,restore->dacConfReg1);
  WriteExtReg(NV_DAC_RGB_PAL_CTRL,restore->dacRgbPalCtrl);

  /* Write out the registers that control the dumb framebuffer */ 
  nvPort[NV_PFB_CONFIG_0]=restore->confReg0;
  nvPort[NV_PFB_START]=restore->startAddr;

  nvPort[NV_PFB_HOR_FRONT_PORCH]=restore->horFrontPorch;
  nvPort[NV_PFB_HOR_SYNC_WIDTH]=restore->horSyncWidth;
  nvPort[NV_PFB_HOR_BACK_PORCH]=restore->horBackPorch;
  nvPort[NV_PFB_HOR_DISP_WIDTH]=restore->horDispWidth;
  nvPort[NV_PFB_VER_FRONT_PORCH]=restore->verFrontPorch;
  nvPort[NV_PFB_VER_SYNC_WIDTH]=restore->verSyncWidth;
  nvPort[NV_PFB_VER_BACK_PORCH]=restore->verBackPorch;
  nvPort[NV_PFB_VER_DISP_WIDTH]=restore->verDispWidth;

  vgaProtect(FALSE);
}

/*
 * NVSave --
 *
 * This function saves the video state.  It reads all of the SVGA registers
 * into the vgaNVRec data structure.  There is in general no need to
 * mask out bits here - just read the registers.
 */
static void *NVSave(void *data)
{
  vgaNVPtr save = data;
  /*
   * This function will handle creating the data structure and filling
   * in the generic VGA portion.
   */
  save = (vgaNVPtr) vgaHWSave ((vgaHWPtr) save, sizeof (vgaNVRec));

  save->prmConfig0=nvPort[NV_PRM_CONFIG_0];
  save->memoryTrace=nvPort[NV_MEMORY_TRACE];

  save->dacConfReg0=ReadExtReg(NV_DAC_CONF_0);
  save->dacConfReg1=ReadExtReg(NV_DAC_CONF_1);
  save->dacRgbPalCtrl=ReadExtReg(NV_DAC_RGB_PAL_CTRL);
  save->Mparam=ReadExtReg(NV_DAC_VPLL_M_PARAM);
  save->Nparam=ReadExtReg(NV_DAC_VPLL_N_PARAM);
  save->Oparam=ReadExtReg(NV_DAC_VPLL_O_PARAM);
  save->Pparam=ReadExtReg(NV_DAC_VPLL_P_PARAM);

  save->confReg0=nvPort[NV_PFB_CONFIG_0];
  save->startAddr=nvPort[NV_PFB_START];

  save->horFrontPorch=nvPort[NV_PFB_HOR_FRONT_PORCH];
  save->horSyncWidth=nvPort[NV_PFB_HOR_SYNC_WIDTH];
  save->horBackPorch=nvPort[NV_PFB_HOR_BACK_PORCH];
  save->horDispWidth=nvPort[NV_PFB_HOR_DISP_WIDTH];
  save->verFrontPorch=nvPort[NV_PFB_VER_FRONT_PORCH];
  save->verSyncWidth=nvPort[NV_PFB_VER_SYNC_WIDTH];
  save->verBackPorch=nvPort[NV_PFB_VER_BACK_PORCH];
  save->verDispWidth=nvPort[NV_PFB_VER_DISP_WIDTH];

  return ((void *) save);
}


static int VirtualScreenOk(DisplayModePtr mode)
{
 if(mode->CrtcHDisplay!=vga256InfoRec.virtualX) return 0;
 
 if(mode->Flags & V_DBLSCAN) {
   return (2*vga256InfoRec.virtualY==mode->CrtcVDisplay);
 }
  
 return (vga256InfoRec.virtualY==mode->CrtcVDisplay);
}

static Bool NVInit(DisplayModePtr mode)     
{
  int bppShift=(vgaBitsPerPixel==8)  ? 1 : 2;
  int m,n,o,p;
  float clockIn=(float)vga256InfoRec.clock[mode->Clock];
  float clockOut;
  int pclkVclkRatio;
  int i;

  /* Check to see that we are not trying to do a virtual screen */
  if(!VirtualScreenOk(mode)) {
    ErrorF("%s %s: %s: Can't select mode %s : virtual desktop not supported\n",
        XCONFIG_PROBED, vga256InfoRec.name,vga256InfoRec.chipset,mode->name);
    return FALSE;
  }

  if(!NVClockSelect(clockIn,&clockOut,&m,&n,&o,&p)) {
    ErrorF("Unable to set desired clock\n");
    return FALSE;
  
  }
  /* Figure out divide down for clock. The Vclk pin is rated for
   * 25-50Mhz but can actually be driven higher than this on most 
   * silicon
   */
  for(i=1,pclkVclkRatio=0;i<=16;i*=2,pclkVclkRatio++) {
    if((clockOut/(double)i) <= NV_MAX_VCLK_PIN_CLOCK_IN_KHZ) break;
  }

  ErrorF("%s %s: %s: Using %.3fMhz clock\n",XCONFIG_PROBED, vga256InfoRec.name,
          vga256InfoRec.chipset,clockOut/1000);
    

  /*
   * This will allocate the datastructure and initialize all of the
   * generic VGA registers.
   */

  if (!vgaHWInit (mode, sizeof (vgaNVRec))) {
    return (FALSE);
  }

  new->Mparam=m;new->Nparam=n;new->Oparam=o;new->Pparam=p;
  new->dacConfReg0=0;
  SETBITFIELD(new->dacConfReg0,NV_DAC_CONF_0_VGA_STATE,1);
  SETBITFIELD(new->dacConfReg0,NV_DAC_CONF_0_PORT_WIDTH,pixelPortWidth);
  SETBITFIELD(new->dacConfReg0,NV_DAC_CONF_0_VISUAL_DEPTH,bppShift);
  SETBITFIELD(new->dacConfReg0,NV_DAC_CONF_0_IDC_MODE,vgaBitsPerPixel==8);

  new->dacConfReg1=0;
  SETBITFIELD(new->dacConfReg1,NV_DAC_CONF_1_VCLK_IMPEDANCE,1);
  SETBITFIELD(new->dacConfReg1,NV_DAC_CONF_1_PCLK_VCLK_RATIO,pclkVclkRatio);

  new->dacRgbPalCtrl=0;
  new->prmConfig0=0;  
  new->memoryTrace=0;

  new->confReg0=0; 
  /* We should set resolution here, but it doesn't seem to do anything */
  SETBITFIELD(new->confReg0,NV_PFB_CONFIG_0_PIXEL_DEPTH,bppShift);
  SETBITFIELD(new->confReg0,NV_PFB_CONFIG_0_PCLK_VCLK_RATIO,pclkVclkRatio);
  SETBITFIELD(new->confReg0,NV_PFB_CONFIG_0_RESOLUTION,4);
  
  new->startAddr=0;
  /* Calculate the monitor timings */
  new->horFrontPorch=mode->CrtcHSyncStart - mode->CrtcHDisplay+1;
  new->horBackPorch=mode->CrtcHTotal - mode->CrtcHSyncEnd+1;
  new->horSyncWidth=mode->CrtcHSyncEnd - mode->CrtcHSyncStart+1;

  new->horDispWidth=mode->CrtcHDisplay;

  new->verFrontPorch=mode->CrtcVSyncStart - mode->CrtcVDisplay+1;
  new->verBackPorch=mode->CrtcVTotal - mode->CrtcVSyncEnd+1;
  new->verSyncWidth=mode->CrtcVSyncEnd - mode->CrtcVSyncStart+1;
  new->verDispWidth=mode->CrtcVDisplay;

  /* Probably need a bit more in here */
  if(mode->Flags & V_DBLSCAN) {
    SETBITFIELD(new->confReg0,NV_PFB_CONFIG_0_SCANLINE,1);
  }

  return (TRUE);
}

/*
 * NVAdjust --
 *
 * This function is used to initialize the SVGA Start Address - the first
 * displayed location in the video memory.  This is used to implement the
 * virtual window.
 */
static void NVAdjust(int x, int y)
{
  int bppShift=(vgaBitsPerPixel==8)  ? 1 : 2;

  /* Wait for vertical blank */
  while(GETBITFIELD(nvPort[NV_PFB_CONFIG_0],NV_PFB_CONFIG_0_VERTICAL)==0);

  nvPort[NV_PFB_START]=(y * vga256InfoRec.displayWidth + x)*bppShift;
 
}

static int NVValidMode(DisplayModePtr mode,Bool verbose, int flag)
{
  return (MODE_OK);
}

