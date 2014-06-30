/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Utils.c,v 3.5 1996/12/23 06:31:42 dawes Exp $ */
/*
 * (c) Copyright 1993,1994 by David Wexelblat <dwex@xfree86.org>
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
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of David Wexelblat shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from David Wexelblat.
 *
 */

/* $XConsortium: Utils.c /main/7 1996/02/21 17:13:00 kaleb $ */

#include "Probe.h"
#include "AsmMacros.h"

/*
 * Return the byte value of register 'port'
 */
#ifdef __STDC__
Byte inp(Word port)
#else
Byte inp(port)
Word port;
#endif
{
	return(inb(port));
}

/*
 * Return the word value of register 'port'
 */
#ifdef __STDC__
Word inpw(Word port)
#else
Word inpw(port)
Word port;
#endif
{
	return(inw(port));
}

/*
 * Return the long value of register 'port'
 */
#ifdef __STDC__
Long inpl(Word port)
#else
Long inpl(port)
Word port;
#endif
{
	return(inl(port));
}

/*
 * Set the byte register 'port' to 'val'
 */
#ifdef __STDC__
void outp(Word port, Byte val)
#else
void outp(port, val)
Word port;
Byte val;
#endif
{
	outb(port, val);
}

/*
 * Set the word register 'port' to 'val'
 */
#ifdef __STDC__
void outpw(Word port, Word val)
#else
void outpw(port, val)
Word port, val;
#endif
{
	outw(port, val);
}

/*
 * Set the long register 'port' to 'val'
 */
#ifdef __STDC__
void outpl(Word port, Long val)
#else
void outpl(port, val)
Word port;
Long val;
#endif
{
	outl(port, val);
}

/*
 * Read byte register at 'port', index 'index'
 */
#ifdef __STDC__
Byte rdinx(Word port, Byte index)
#else
Byte rdinx(port, index)
Word port;
Byte index;
#endif
{
	Byte tmp;
	Word Port[2] = {MISC_OUT_R, 0x000};

	if (port == ATR_IDX)
	{
		EnableIOPorts(1, Port);
		Port[1] = (inp(MISC_OUT_R) & 0x01) ? 0x3DA : 0x3BA;
		EnableIOPorts(2, Port);
		tmp = inp(Port[1]);	/* Reset Attribute Reg flip-flop */
		DisableIOPorts(2, Port);
	}
	outb(port, index);
	return(inb(port+1));
}

/*
 * Set the byte register 'port', index 'index' to 'val'
 */
#ifdef __STDC__
void wrinx(Word port, Byte index, Byte val)
#else
void wrinx(port, index, val)
Word port;
Byte index;
Byte val;
#endif
{
	Byte tmp;
	Word Port[2] = {MISC_OUT_R, 0x000};

	if (port == ATR_REG_W)
	{
		EnableIOPorts(1, Port);
		Port[1] = (inp(MISC_OUT_R) & 0x01) ? 0x3DA : 0x3BA;
		EnableIOPorts(2, Port);
		tmp = inp(Port[1]);
		DisableIOPorts(2, Port);
		outp(port, index);
		outp(port, val);
	}
	else
	{
		outb(port, index);
		outb(port+1, val);
	}
}

/*
 * Set the word register 'port', index 'index' to 'val'
 */
#ifdef __STDC__
void wrinx2(Word port, Byte index, Word val)
#else
void wrinx2(port, index, val)
Word port;
Byte index;
Word val;
#endif
{
	wrinx(port, index, (val) & 0xFF);
	wrinx(port, index+1, (val >> 8) & 0xFF);
}

/*
 * Set the longword register 'port', index 'index' to 'val' (3 bytes, actually)
 */
#ifdef __STDC__
void wrinx3(Word port, Byte index, Long val)
#else
void wrinx3(port, index, val)
Word port;
Byte index;
Long val;
#endif
{
	wrinx(port, index, (val) & 0xFF);
	wrinx(port, index+1, (val >> 8) & 0xFF);
	wrinx(port, index+2, (val >> 16) & 0xFF);
}

/*
 * Set the word register 'port', index 'index' to 'val' (Motorola (big-endian)
 * byte order)
 */
#ifdef __STDC__
void wrinx2m(Word port, Byte index, Word val)
#else
void wrinx2m(port, index, val)
Word port;
Byte index;
Word val;
#endif
{
	wrinx(port, index, (val >> 8) & 0xFF);
	wrinx(port, index+1, (val) & 0xFF);
}

/*
 * Set the longword register 'port', index 'index' to 'val' (3 bytes, actually)
 * (Motorola (big-endian) byte order)
 */
#ifdef __STDC__
void wrinx3m(Word port, Byte index, Long val)
#else
void wrinx3m(port, index, val)
Word port;
Byte index;
Long val;
#endif
{
	wrinx(port, index, (val >> 16) & 0xFF);
	wrinx(port, index+1, (val >> 8) & 0xFF);
	wrinx(port, index+2, (val) & 0xFF);
}

/*
 * In byte register 'port', index 'index', set the bit that are 1 in 'mask'
 * to the value of the corresponding bits in 'mask'
 */
#ifdef __STDC__
void modinx(Word port, Byte index, Byte mask, Byte newval)
#else
void modinx(port, index, mask, newval)
Word port;
Byte index; 
Byte mask; 
Byte newval;
#endif
{
	Byte tmp;

	tmp = (rdinx(port, index) & ~mask) | (newval & mask);
	wrinx(port, index, tmp);
}

/*
 * In register 'port' sets the bits in 'mask' as in 'nvw'.  Other bits are
 * left unchanged.
 */
#ifdef __STDC__
void modreg(Word port, Byte mask, Byte nvw)
#else
void modreg(port, mask, nvw)
Word port;
Byte mask;
Byte nvw;
#endif
{
	Byte tmp;

	tmp = (inp(port) & (~mask)) | (nvw & mask);
	outp(port, tmp);
}

/*
 * Sets the bits in register 'port', index 'index' that are also set in 'val'
 */
#ifdef __STDC__
void setinx(Word port, Byte index, Byte val)
#else
void setinx(port, index, val)
Word port;
Byte index;
Byte val;
#endif
{
	Byte tmp;

	tmp = rdinx(port, index);
	wrinx(port, index, tmp | val);
}

/*
 * Clears the bits in register 'port', index 'index' that are set in 'val'
 */
#ifdef __STDC__
void clrinx(Word port, Byte index, Byte val)
#else
void clrinx(port, index, val)
Word port;
Byte index;
Byte val;
#endif
{
	Byte tmp;

	tmp = rdinx(port, index);
	wrinx(port, index, tmp & ~val);
}
/*
 * Return TRUE iff the bits in 'mask' of register 'port' are read/write.
 */
#ifdef __STDC__
Bool tstrg(Word port, Byte mask)
#else
Bool tstrg(port, mask)
Word port;
Byte mask;
#endif
{
	Byte old, new1, new2;

	old = inp(port);
	outp(port, (old & ~mask));
	new1 = inp(port) & mask;
	outp(port, (old | mask));
	new2 = inp(port) & mask;
	outp(port, old);
	return((new1==0) && (new2==mask));
}

/*
 * Returns true iff the bits in 'mask' of register 'port', index 'index'
 * are read/write.
 */
#ifdef __STDC__
Bool testinx2(Word port, Byte index, Byte mask)
#else
Bool testinx2(port, index, mask)
Word port;
Byte index;
Byte mask;
#endif
{
	Byte old, new1, new2;

	old = rdinx(port, index);
	wrinx(port, index, (old & ~mask));
	new1 = rdinx(port, index) & mask;
	wrinx(port, index, (old | mask));
	new2 = rdinx(port, index) & mask;
	wrinx(port, index, old);
	return((new1==0) && (new2==mask));
}

/*
 * Returns true iff all bits of register 'port', index 'index' are read/write
 */
#ifdef __STDC__
Bool testinx(Word port, Byte index)
#else
Bool testinx(port, index)
Word port;
Byte index;
#endif
{
	return(testinx2(port, index, 0xFF));
}

/*
 * Force DAC back to PEL mode
 */
void dactopel()
{
	(void)inp(0x3C8);
}

/*
 * Trigger HiColor DACs to change mode.
 */
Byte trigdac()
{
	(void)inp(0x3C6);
	(void)inp(0x3C6);
	(void)inp(0x3C6);
	return(inp(0x3C6));
}

/*
 * Enter command mode of HiColor DACs.
 */
Byte dactocomm()
{
	dactopel();
	return(trigdac());
}

/*
 * Get the HiColor DAC command register and return to PEL mode.
 */
Byte getdaccomm()
{
	Byte ret;

	dactocomm();
	ret = inp(0x3C6);
	dactopel();

	return(ret);
}

/*
 * Wait for vertical retrace interval.
 */
void waitforretrace()
{
	Word Port[2] = {MISC_OUT_R, 0x000};

	EnableIOPorts(1, Port);

	Port[1] = (inp(MISC_OUT_R) & 0x01) ? 0x3DA : 0x3BA;
	EnableIOPorts(2, Port);
	while ((inp(Port[1]) & 0x08) != 0)
	{
		/* Idle */
	}
	while ((inp(Port[1]) & 0x08) == 0)
	{
		/* Idle */
	}

	DisableIOPorts(2, Port);
	return;
}

/*
 * Check chip descriptor against exclusion list
 */
Bool Excluded(ExList, Chip, Mask10)
Range *ExList;
Chip_Descriptor *Chip;
Bool Mask10;
{
	int i, j;
	Word mask = (Mask10 ? 0x3FF : 0xFFFF);

	if (Chip->num_ports == 0)
	{
		return(FALSE);
	}
	if (Chip->ports[0] == 0)
	{
		/* Fill in CRTC */
		Chip->ports[0] = CRTC_IDX;
		Chip->ports[1] = CRTC_REG;
	}
	for (i=0; i < Chip->num_ports; i++)
	{
		for (j=0; ExList[j].lo != (Word)-1; j++)
		{
			if (ExList[j].hi == (Word)-1)
			{
				/* single port */
				if ((Chip->ports[i] & mask) == ExList[j].lo)
				{
					return(TRUE);
				}
			}
			else
			{
				/* range */
				if (((Chip->ports[i]&mask) >= ExList[j].lo) &&
				    ((Chip->ports[i]&mask) <= ExList[j].hi))
				{
					return(TRUE);
				}
			}
		}
	}
	return(FALSE);
}

int StrCaseCmp(s1, s2)
char *s1;
char *s2;
{
	char c1, c2;

	if (*s1 == 0)
		if (*s2 == 0)
			return(0);
		else
			return(1);

	c1 = (isupper(*s1) ? tolower(*s1) : *s1);
	c2 = (isupper(*s2) ? tolower(*s2) : *s2);
	while (c1 == c2)
	{
		if (c1 == '\0')
			return(0);
		s1++; 
		s2++;
		c1 = (isupper(*s1) ? tolower(*s1) : *s1);
		c2 = (isupper(*s2) ? tolower(*s2) : *s2);
	}
	return(c1 - c2);
}

unsigned int StrToUL(str)
CONST char *str;
{
	int base = 10;
	CONST char *p = str;
	unsigned int tot = 0;

	if (*p == '0')
	{
		p++;
		if (*p == 'x')
		{
			p++;
			base = 16;
		}
		else
		{
			base = 8;
		}
	}
	while (*p)
	{
		if ((*p >= '0') && (*p <= ((base == 8)?'7':'9')))
		{
			tot = tot * base + (*p - '0');
		}
		else if ((base == 16) && (*p >= 'a') && (*p <= 'f'))
		{
			tot = tot * base + 10 + (*p - 'a');
		}
		else if ((base == 16) && (*p >= 'A') && (*p <= 'F'))
		{
			tot = tot * base + 10 + (*p - 'A');
		}
		else
		{
			return(tot);
		}
		p++;
	}
	return(tot);
}
