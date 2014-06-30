/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/MemProbe.c,v 3.4 1996/12/23 06:31:18 dawes Exp $ */
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

/* $XConsortium: MemProbe.c /main/5 1996/02/21 17:10:50 kaleb $ */

#include "Probe.h"

static Word Ports[] = {SEQ_IDX, SEQ_REG, GRC_IDX, GRC_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Ports[0]))

#define SAVESIZE	100
#define BYTE_AT(s,i,j)	(*((s)+((i)*SAVESIZE)+(j)))
#define TAG		"SuperProbe"

int Probe_Mem(Check)
MemCheck_Descriptor Check;
{
	Byte Save256;
	Byte SaveMap;
	Byte SaveSeq;
	Byte SaveMask;
	Byte *Mem;
	Byte *Store;
	int BankNum;
	register int i, j;
	Bool IsMem = TRUE;
	int SaveRBank;
	int SaveWBank;
	int Total;

	Store = (Byte *)malloc((Check.max_bank+1)*SAVESIZE);
	if (Store == (Byte *)0)
	{
		return(0);
	}
	Mem = MapVGA();
	if (Mem == (Byte *)0)
	{
		return(0);
	}
	EnableIOPorts(NUMPORTS, Ports);
	Save256 = rdinx(GRC_IDX, 0x05);
	SaveMap = rdinx(GRC_IDX, 0x06);
	SaveMask = rdinx(SEQ_IDX, 0x02);
	SaveSeq = rdinx(SEQ_IDX, 0x04);
	wrinx(GRC_IDX, 0x05, ((Save256 & 0x0F) | 0x40));
	wrinx(GRC_IDX, 0x06, (SaveMap & 0xF0) | 0x05);
	wrinx(SEQ_IDX, 0x02, (SaveMask & 0xF0) | 0x0F);
	wrinx(SEQ_IDX, 0x04, (SaveSeq | 0x0E));
	Check.get_bank(Check.chipset, &SaveRBank, &SaveWBank);
	for (BankNum = 0; ((BankNum <= Check.max_bank) && (IsMem)); BankNum++)
	{
		Check.set_bank(Check.chipset, BankNum, BankNum);
		/*
		 * See if this bank is tagged.
		 */
		if (strncmp((char *)Mem, TAG, strlen(TAG)) == 0)
		{
			break;
		}
		for (i=0; i < SAVESIZE; i++)
		{
			BYTE_AT(Store,BankNum,i) = Mem[i];
			Mem[i] ^= 0xAA;
		}
		IsMem = TRUE;
		for (i=0; i < SAVESIZE; i++)
		{
			if (Mem[i] != (BYTE_AT(Store,BankNum,i) ^ 0xAA))
			{
				IsMem = FALSE;
				break;
			}
		}
		for (i = 0; i < SAVESIZE; i++)
		{
			Mem[i] = BYTE_AT(Store,BankNum,i);
		}
		if (IsMem)
		{
			for (i=0; i < SAVESIZE; i++)
			{
				Mem[i] ^= 0x55;
			}
			IsMem = TRUE;
			for (i=0; i < SAVESIZE; i++)
			{
				if (Mem[i] != (BYTE_AT(Store,BankNum,i) ^ 0x55))
				{
					IsMem = FALSE;
					break;
				}
			}
			for (i = 0; i < SAVESIZE; i++)
			{
				Mem[i] = BYTE_AT(Store,BankNum,i);
			}
		}
		if (IsMem)
		{
			/*
			 * Tag this bank
			 */
			strcpy((char *)Mem, TAG);
		}
	}
	Total = (BankNum * Check.bank_size) / 1024; 

	for (j=0; j < BankNum; j++)
	{
		Check.set_bank(Check.chipset, j, j);
		for (i=0; i < SAVESIZE; i++)
		{
			Mem[i] = BYTE_AT(Store,j,i);
		}
	}

	Check.set_bank(Check.chipset, SaveRBank, SaveWBank);
	wrinx(SEQ_IDX, 0x04, SaveSeq);
	wrinx(SEQ_IDX, 0x02, SaveMask);
	wrinx(GRC_IDX, 0x06, SaveMap);
	wrinx(GRC_IDX, 0x05, Save256);
	DisableIOPorts(NUMPORTS, Ports);
	UnMapVGA(Mem);

	return(Total);
}
