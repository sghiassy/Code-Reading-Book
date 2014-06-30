/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000Bt485.c,v 3.6 1996/12/23 06:40:31 dawes Exp $ */
/*
 * Copyright 1993 By David Wexelblat <dwex@aib.com>
 *
 * Modified 1994 for P9000 Server By Erik Nygren <nygren@mit.edu>
 * Modified for Viper PCI by Matt Thomas <thomas@lkg.dec.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the names of David
 * Wexelblat, Matt Thomas, and Erik Nygren not be used in advertising
 * or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat and Erik
 * Nygren make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * DAVID WEXELBLAT, MATT THOMAS, AND ERIK NYGREN DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DAVID
 * WEXELBLAT OR ERIK NYGREN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * */
/* $XConsortium: p9000Bt485.c /main/4 1996/02/21 17:31:56 kaleb $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"
#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "p9000.h"
#include "p9000reg.h"
#include "p9000Bt485.h"

extern xrgb xf86weight;


#ifndef __GNUC__
# define __inline__ /**/
#endif

/*
 * The Bt485 has 16 command registers.  The low-order two bits of the
 * register address follow normal VGA DAC addressing conventions (which 
 * for some reason aren't in numeric order, so we remap them through 
 * an array).  The P9000 provides access by remapping to 
 * 0x03C[6789], 0x43C[6789], 0x83C[6789], and 0xC3C[6789].
 * The Viper PCI is a special case where addresses are remapped
 * 0x03C[6789] and then the rest are mapped into IOBase+0x[48C][6789].
 * How the Viper PCI is handled is a hack and should be changed!
 * Maybe have a table of DAC functions.  *TO*DO* (ELN)
 * If you change this, make sure to change p9000BtCurs as well!
 */
#ifdef __STDC__
void p9000OutBtReg(unsigned short reg, unsigned char mask, unsigned char data)
#else
void p9000OutBtReg(reg, mask, data)
unsigned short reg;
unsigned char mask;
unsigned char data;
#endif
{
  unsigned char tmp;
  if ((p9000VendorPtr->Label == P9000_VENDOR_VIPERPCI)
      && (0xf000 & reg))
    {
      reg = ((unsigned long)((reg & 0x00ff) | ((reg & 0xf000)>>4)))
	+ (unsigned long)p9000InfoRec.IObase;
    }
  tmp = inb(reg) & mask;
  outb(reg, tmp | data);
}

#ifdef __STDC__
unsigned char p9000InBtReg(unsigned short reg)
#else
unsigned char p9000InBtReg(reg)
unsigned short reg;
#endif
{
  if ((p9000VendorPtr->Label == P9000_VENDOR_VIPERPCI)
      && (0xf000 & reg))
    {
      reg = ((unsigned long)((reg & 0x00ff) | ((reg & 0xf000)>>4)))
	+ (unsigned long)p9000InfoRec.IObase;
    }
  return(inb(reg));
}

/*
 * The Command Register 3 register in the Bt485 must be accessed through
 * a very odd sequence, as the older RAMDACs had already defined 16
 * registers.  Apparently this overlays the Status register.
 */
#ifdef __STDC__
void p9000OutBtRegCom3(unsigned char mask, unsigned char data)
#else
void p9000OutBtRegCom3(mask, data)
unsigned char mask;
unsigned char data;
#endif
{
   p9000OutBtReg(BT_COMMAND_REG_0, 0x7F, BT_CR0_ENABLE_CR3);
   p9000OutBtReg(BT_WRITE_ADDR, 0x00, 0x01);
   p9000OutBtReg(BT_COMMAND_REG_3, mask, data);
}

unsigned char p9000InBtRegCom3()
{
   unsigned char ret;

   p9000OutBtReg(BT_COMMAND_REG_0, 0x7F, BT_CR0_ENABLE_CR3);
   p9000OutBtReg(BT_WRITE_ADDR, 0x00, 0x01);
   ret = p9000InBtReg(BT_COMMAND_REG_3);
   return(ret);
}

/*
 * Status register may be hidden behind Command Register 3; need to check
 * both possibilities.
 */
unsigned char p9000InBtStatReg()
{
   unsigned char tmp, ret;

   tmp = p9000InBtReg(BT_COMMAND_REG_0);
   if ((tmp & 0x80) == 0x80) {
      /* Behind Command Register 3 */
      tmp = p9000InBtReg(BT_WRITE_ADDR);
      p9000OutBtReg(BT_WRITE_ADDR, 0x00, 0x00);
      ret = p9000InBtReg(BT_STATUS_REG);
      p9000OutBtReg(BT_WRITE_ADDR, 0x00, tmp);
   } else {
      ret = p9000InBtReg(BT_STATUS_REG);
   }
   return(ret);
}

/* For saving the old values in */
static unsigned char bt_cr0_save, bt_cr1_save, bt_cr2_save, bt_cr3_save;
static unsigned bt_saved = FALSE;

/*
 * p9000BtEnable --
 *    Enable the bt485 for use
 */
void
p9000BtEnable(crtcRegs)
     p9000CRTCRegPtr crtcRegs;
{
  Bool weight555;  /* Is the 16 bpp weighting 555 (as opposed to 565)? */

  if (!bt_saved)
    {
      bt_cr0_save = p9000InBtReg(BT_COMMAND_REG_0);
      bt_cr1_save = p9000InBtReg(BT_COMMAND_REG_1);
      bt_cr2_save = p9000InBtReg(BT_COMMAND_REG_2);
      bt_cr3_save = p9000InBtRegCom3();
      bt_saved = TRUE;
    }

  p9000OutBtReg(BT_COMMAND_REG_0, 0x0, BT_CR0_8BIT | 
		(p9000DACSyncOnGreen ? BT_CR0_SYNC_G : 0x0));
  p9000OutBtRegCom3(0x0, BT_CR3_1X_CLOCKMUL | BT_CR3_32SQ_CURSOR);
  p9000OutBtReg(BT_COMMAND_REG_2, 0x0,
		BT_CR2_PORTSEL_NONMASK | BT_CR2_PCLK1 | BT_CR2_CURSOR_DISABLE |
		(crtcRegs->interlaced ? BT_CR2_INTERLACED : 0x0));

  if (crtcRegs->BytesPerPixel == 1)       /* 8 bit color */
    p9000OutBtReg(BT_COMMAND_REG_1, 0x0, BT_CR1_BP8); /* 4:1 mux */

  else if (crtcRegs->BytesPerPixel == 2)  /* 16 bit color */
    {      
      weight555 = (xf86weight.red == 5 && xf86weight.green == 5
		   && xf86weight.blue == 5);

      p9000OutBtReg(BT_COMMAND_REG_1, 0x0,
		    BT_CR1_BP16 | BT_CR1_BYPASS_PAL
		    | (weight555 ? BT_CR1_555RGB : BT_CR1_565RGB)
		    | BT_CR1_16B_21MUX );
    }
  else                        /* sparse 32 bit color (24 bit depth) */
    p9000OutBtReg(BT_COMMAND_REG_1, 0x0, BT_CR1_BP24 | BT_CR1_BYPASS_PAL);
}

/*
 * p9000BtRestore --
 *    Restore the bt485 in preparation for shutting down the p9000
 */
void
p9000BtRestore()
{
#ifdef DEBUG
  ErrorF("Restoring DAC:  CR0:0x%x  CR1:0x%x CR2:0x%x CR3:0x%x\n",
	 bt_cr0_save, bt_cr1_save, bt_cr2_save, bt_cr3_save);
#endif
  p9000OutBtRegCom3(0x0, bt_cr3_save);
  p9000OutBtReg(BT_COMMAND_REG_1, 0x0, bt_cr1_save);
  p9000OutBtReg(BT_COMMAND_REG_2, 0x0, bt_cr2_save);
  p9000OutBtReg(BT_COMMAND_REG_0, 0x0, bt_cr0_save);
}





