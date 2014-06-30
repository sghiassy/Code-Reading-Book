/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000vga.c,v 3.10 1996/12/23 06:40:56 dawes Exp $ */
/*
 * Copyright 1994, Erik Nygren (nygren@mit.edu)
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * ERIK NYGREN, CHRIS MASON, AND DAVID MOEWS DISCLAIMw ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL ERIK NYGREN, CHRIS MASON, OR DAVID MOEWS BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Code for vga text restoration taken from svgalib and put in by chris mason
 * (mason@mail.csh.rit.edu)
 *
 * Bank switching code added by David Moews (dmoews@xraysgi.ims.uconn.edu)
 *
 */
/* $XConsortium: p9000vga.c /main/6 1996/03/11 10:44:00 kaleb $ */

#include "X.h"
#include "input.h"

#include "xf86.h"
#include "xf86_OSlib.h"

#include "misc.h"
#include "vgaBank.h"

#include "p9000.h"
#include "p9000reg.h"
#include "p9000Bt485.h"

/* Virtual Terminal / VGA values to be saved and restored */
static short vga_dac_mask = 0xff;
static LUTENTRY vga_lut[256];

pointer vgaBase;
int vgaIOBase;


static void p9000VGASlowCopy(
#if NeedFunctionPrototypes
   unsigned char *, unsigned char *, unsigned
#endif
);

static void p9000VGADelay(
#if NeedFunctionPrototypes
   void
#endif
);


/*
 * p9000VGASlowCopy --
 *     Copiess memory from one location to another
 */
void p9000VGASlowCopy(unsigned char *dest, unsigned char *src, unsigned bytes)
{
  while (bytes-- > 0)
    {
      *(dest++) = *(src++);
    }
}

/*
 * p9000VGADelay --
 *     Pauses for a very short time.
 */
void p9000VGADelay()
{
  int ctr;
  for (ctr = 0; ctr <= 10; ctr++)
    outb(0x80, 0);  /* This is the POST register.  The length of this
		     * delay is dependant only on ISA bus speed */
}

/*
 * this is used to put the vga chip into a 16 color graphics mode, which
 * makes the font saving and restoration MUCH easier
 */
/* BIOS mode 10h - 640x350x16 */
static const unsigned char g640x350x16_regs[60] = {
    0x5F,0x4F,0x50,0x82,0x54,0x80,0xBF,0x1F,0x00,0x40,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x83,0x85,0x5D,0x28,0x0F,0x63,0xBA,0xE3, 
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B, 
    0x0C,0x0D,0x0E,0x0F,0x01,0x00,0x0F,0x00,0x00, 
    0x00,0x0F,0x00,0x20,0x00,0x00,0x05,0x0F,0xFF, 
    0x03,0x01,0x0F,0x00,0x06, 
    0xA3
};


static unsigned char text_regs[200] ;  /* holds the textmode register values */
static unsigned char font_buf[FONT_SIZE] ; /* hold the first font buffer     */

int CRT_I = CRT_IC ;	/* bases for color */
int CRT_D = CRT_DC ;
int IS1_R = IS1_RC ;

int p9000VGASaveRegs(unsigned char *regs)
{
  int i;

  /* save VGA registers */
  for (i = 0; i < CRT_C; i++) {
       outb(CRT_I, i); 
       regs[CRT+i] = inb(CRT_D); 
  }
  for (i = 0; i < ATT_C; i++) {
       inb(IS1_R);
       p9000VGADelay();
       outb(ATT_IW, i); 
       p9000VGADelay();
       regs[ATT_+i] = inb(ATT_R); 
       p9000VGADelay();
  }
  for (i = 0; i < GRA_C; i++) {
       outb(GRA_I, i); 
       regs[GRA+i] = inb(GRA_D); 
  }
  for (i = 0; i < SEQ_C; i++) {
       outb(SEQ_INDEX_REG, i); 
       regs[SEQ+i] = inb(SEQ_PORT); 
  }
  /* This save and restore is used for switching into
   * 640x400x16 and does not conflict with the one
   * done in the vendor specific code */
  regs[MIS] = inb(MISC_IN_REG); 
#ifdef DEBUG
  ErrorF("p9000VGASaveRegs: debug info: regs[MIS]=inb(MISC_IN_REG)=0x%X\n" ,
	 (int)regs[MIS]);
#endif
  return CRT_C + ATT_C + GRA_C + SEQ_C + 1 + i;
}

int p9000VGASetRegs(const unsigned char *regs)
{
  int i;
  int tmp;

  /* update misc output register */
  usleep(20000) ;
  /* This save and restore is used for switching into
   * 640x400x16 and does not conflict with the one
   * done in the vendor specific code */
  outb(MISC_OUT_REG, regs[MIS]);         
#ifdef DEBUG
  ErrorF("p9000VGASetRegs: debug info: outb(MISC_OUT_REG, regs[MIS]) regs[MIS]=0x%X\n",
	 (int)regs[MIS]);
#endif

  usleep(30000);  /* Wait at least 10 ms for the clock to change */

  /* synchronous reset on */
  outb(SEQ_INDEX_REG, 0x00); 
  outb(SEQ_PORT, 0x01);               
 
  /* write sequencer registers */
  outb(SEQ_INDEX_REG, 1);
  outb(SEQ_PORT, regs[SEQ+1]|0x20);
  for (i = 2; i < SEQ_C; i++) {       
      outb(SEQ_INDEX_REG, i); 
      outb(SEQ_PORT, regs[SEQ+i]); 
  }

  /* synchronous reset off */
  outb(SEQ_INDEX_REG, 0x00); 
  outb(SEQ_PORT, 0x03);              

  outb(CRT_I, 0x11);            
  tmp = inb(CRT_D)&0x7F;
  outb(CRT_D, tmp);   

  /* write CRT registers */
  for (i = 0; i < CRT_C; i++) {       
      outb(CRT_I, i); 
      outb(CRT_D, regs[CRT+i]); 
  }

  /* write graphics controller registers */
  for (i = 0; i < GRA_C; i++) {       
      outb(GRA_I, i); 
      outb(GRA_D, regs[GRA+i]); 
  }
   
  /* write attribute controller registers */
  for (i = 0; i < ATT_C; i++) {       
      inb(IS1_R);   /* reset flip-flop */
      p9000VGADelay();
      outb(ATT_IW, i);
      p9000VGADelay();
      outb(ATT_IW, regs[ATT_+i]);
      p9000VGADelay();
  }

  return 0;
}

/* 
 * p9000SaveVT --
 *    Restores the LUT and other parts of the VT's state for a virtual terminal
 */
void p9000SaveVT()
{
  /* Save old values */
  p9000ReadLUT(vga_lut);
  vga_dac_mask = inb(BT_PIXEL_MASK);
  p9000VGASaveRegs(text_regs);
  p9000VGASetRegs(g640x350x16_regs);
  outb(GRA_I, 0x04);
  outb(GRA_D, 0x02);
  p9000VGASlowCopy(font_buf, vgaBase, FONT_SIZE);
  p9000VGASetRegs(text_regs);
} 

/* 
 * p9000RestoreVT --
 *    Restores the LUT and other things for a virtual terminal
 */
void p9000RestoreVT()
{
  int tmp;

  p9000VGASetRegs(g640x350x16_regs); /* put us in a 16 color graphics mode */

  /* disable Set/Reset register */
  outb(GRA_I, 0x01);
  outb(GRA_D, 0x00);
  
  /* restore font data from plane 2 */
  outb(GRA_I, 0x02);
  outb(GRA_D, 0x04);
  p9000VGASlowCopy(vgaBase, font_buf, FONT_SIZE);

  p9000VGASetRegs(text_regs);	/* assumes they have already been saved */
  outb(SEQ_INDEX_REG, 0x01);			/* turn screen on */
  tmp = inb(SEQ_PORT)&0xDF;
  outb(SEQ_PORT, tmp);
  outb(ATT_IW, 0x20); 				/* enable display ????? */

  /* Restore old values */
  p9000WriteLUT(vga_lut);
  outb(BT_PIXEL_MASK,vga_dac_mask);
}
