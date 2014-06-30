/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/ATI.c,v 3.11.2.2 1997/05/22 14:00:33 dawes Exp $ */
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

/* $XConsortium: ATI.c /main/10 1996/10/25 07:00:11 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x01CE, 0x01CF};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_ATI __STDCARGS((int));

Chip_Descriptor ATI_Descriptor = {
	"ATI",
	Probe_ATI,
	Ports,
	NUMPORTS,
	TRUE,
	TRUE,
	TRUE,
	MemProbe_ATI,
};

Bool Crippled_Mach32 = FALSE,
     Crippled_Mach64 = FALSE,
     Mach64xTB = FALSE;

extern Chip_Descriptor ATIMach_Descriptor;

#ifdef __STDC__
void Probe_ATI_ChipID(int chip, int *Chipset)
#else
void Probe_ATI_ChipID(chip, Chipset)
int chip, *Chipset;
#endif
{
	if (chip == CHIP_MACH64)
	{
		extern Word ATIMach64CONFIG_CHIP_ID;

		EnableIOPorts(1, &ATIMach64CONFIG_CHIP_ID);
		chip = inpl(ATIMach64CONFIG_CHIP_ID);
		switch (chip & 0xFFFF)
		{
		case 0x00D7:
		case 0x4758:
			switch ((chip & 0xFF000000) >> 24)
			{
			case 0x00:
				*Chipset = CHIP_ATI88800GXC;
				break;
			case 0x01:
				*Chipset = CHIP_ATI88800GXD;
				break;
			case 0x02:
				*Chipset = CHIP_ATI88800GXE;
				break;
			case 0x03:
				*Chipset = CHIP_ATI88800GXF;
				break;
			default:
				Chip_data = chip;
				*Chipset = CHIP_ATI_UNK;
				break;
			}
			break;
		case 0x0057:
		case 0x4358:
			*Chipset = CHIP_ATI88800CX;
			break;
		case 0x0053:
		case 0x4354:
			*Chipset = CHIP_ATI264CT;
			break;
		case 0x0093:
		case 0x4554:
			*Chipset = CHIP_ATI264ET;
			break;
		case 0x0166:
		case 0x4C47:
			*Chipset = CHIP_ATI264LT;
			Mach64xTB = TRUE;
			break;
		case 0x02B3:
		case 0x5654:
			if (chip & 0x40000000)
			{
				Mach64xTB = TRUE;
				*Chipset = CHIP_ATI264VTB;
			}
			else
				*Chipset = CHIP_ATI264VT;
			break;
		case 0x00D3:
		case 0x4754:
			if (chip & 0x40000000)
			{
				Mach64xTB = TRUE;
				*Chipset = CHIP_ATI264GTB;
			}
			else
				*Chipset = CHIP_ATI264GT;
			break;
		default:
			Chip_data = chip;
			*Chipset = CHIP_ATI_UNK;
			break;
		}
		DisableIOPorts(1, &ATIMach64CONFIG_CHIP_ID);
	}
	else /* if (chip == CHIP_MACH32) */
	{
		Word IOPort = CHIP_ID;

		EnableIOPorts(1, &IOPort);

		chip = inpw(CHIP_ID);
		if (chip == 0xFFFF)
			chip = 0;
		switch (chip & 0x03FF)
		{
		case 0x0000:
			*Chipset = CHIP_ATI68800_3;
			break;
		case 0x02F7:
			*Chipset = CHIP_ATI68800_6;
			break;
		case 0x0177:
			*Chipset = CHIP_ATI68800LX;
			break;
		case 0x0017:
			*Chipset = CHIP_ATI68800AX;
			break;
		default:
			Chip_data = chip;
			*Chipset = CHIP_ATI_UNK;
			break;
		}
		DisableIOPorts(1, &IOPort);
	}
}

#ifdef __STDC__
Bool Probe_ATI(int *Chipset)
#else
Bool Probe_ATI(Chipset)
int *Chipset;
#endif
{
	Bool result = FALSE;
	Byte bios[10];
	Byte *signature = (Byte *)"761295520";
	int chip;

	/*
	 * First, look for a Mach32 or a Mach64.
	 */
	if (ATIMach_Descriptor.f(&chip) &&
	   ((chip == CHIP_MACH32) || (chip == CHIP_MACH64)))
	{
		Probe_ATI_ChipID(chip, Chipset);
		return (TRUE);
	}

	if (ReadBIOS(0x31, bios, 9) != 9)
	{
		fprintf(stderr, "%s: Failed to read ATI signature\n", MyName);
		return(FALSE);
	}
	if (memcmp(bios, signature, 9) == 0)
	{
		if (ReadBIOS(0x40, bios, 4) != 4)
		{
			fprintf(stderr, "%s: Failed to read ATI BIOS data\n",
				MyName);
			return(FALSE);
		}

		result = TRUE;

		if ((bios[0] == '3') && (bios[1] == '1'))
		{
			/* Set up Ports array */
			if (ReadBIOS(0x10, bios, sizeof(Word)) != sizeof(Word))
			{
				fprintf(stderr,
					"%s: Failed to read ATI BIOS data\n",
					MyName);
				return (FALSE);
			}

			switch (bios[3])
			{
			case '1':
				*Chipset = CHIP_ATI18800;
				break;
			case '2':
				*Chipset = CHIP_ATI18800_1;
				break;
			case '3':
				*Chipset = CHIP_ATI28800_2;
				break;
			case '4':
				*Chipset = CHIP_ATI28800_4;
				break;
			case '5':
				*Chipset = CHIP_ATI28800_5;
				break;
			case '6':
				*Chipset = CHIP_ATI28800_6;
				break;
			case 'a':
			case 'b':
			case 'c':
				Crippled_Mach32 = TRUE;
				Probe_ATI_ChipID(CHIP_MACH32, Chipset);
				break;
			case ' ':
				Crippled_Mach64 = TRUE;
				Probe_ATI_ChipID(CHIP_MACH64, Chipset);
				break;
			default:
				Chip_data = (((((bios[0] << 8) |
						 bios[1]) << 8) |
						 bios[2]) << 8) | bios[3];
				*Chipset = CHIP_ATI_UNK;
				break;
			}

			if (!Crippled_Mach64)
			{
				Ports[0] = *((Word *)bios);
				Ports[1] = Ports[0] + 1;
			}

			/*
			 * Sometimes, the BIOS lies about the chip.
			 */
			if ((*Chipset >= CHIP_ATI28800_4) &&
			    (*Chipset <= CHIP_ATI28800_6))
			{
				EnableIOPorts(NUMPORTS, Ports);
				chip = rdinx(Ports[0], 0xAA) & 0x0F;
				if (chip < 7)
				{
					chip = SVGA_TYPE(V_ATI, chip);
					if (chip > *Chipset)
						*Chipset = chip;
				}
				DisableIOPorts(NUMPORTS, Ports);
			}
		}
		else
		{
			Chip_data = (((((bios[0] << 8) | bios[1]) << 8) |
					 bios[2]) << 8) | bios[3];
			*Chipset = CHIP_ATI_UNK;
		}
	}
	return(result);
}

static int MemProbe_ATI(Chipset)
int Chipset;
{
	int Mem = 0;

	if ((Chipset >= CHIP_ATI88800GXC) && !Crippled_Mach64)
		return (ATIMach_Descriptor.memcheck(CHIP_MACH64));
	if ((Chipset >= CHIP_ATI68800_3) && !Crippled_Mach32)
		return (ATIMach_Descriptor.memcheck(CHIP_MACH32));

	/* Ports array should already be set up */
	EnableIOPorts(NUMPORTS, Ports);

	if (Crippled_Mach32)
		Chipset = CHIP_MACH32;
	else if (Crippled_Mach64)
		Chipset = CHIP_MACH64;

	switch (Chipset)
	{
	case CHIP_ATI18800:
	case CHIP_ATI18800_1:
		if (rdinx(Ports[0], 0xBB) & 0x20)
			Mem = 512;
		else
			Mem = 256;
		break;
	case CHIP_ATI28800_2:
	case CHIP_ATI28800_4:
	case CHIP_ATI28800_5:
	case CHIP_ATI28800_6:
	case CHIP_MACH32:
	case CHIP_MACH64:
		switch (rdinx(Ports[0], 0xB0) & 0x18)
		{
		case 0x00:
			Mem = 256;
			break;
		case 0x10:
			Mem = 512;
			break;
		case 0x08:
		case 0x18:
			Mem = 1024;
			break;
		}
		break;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);

}
