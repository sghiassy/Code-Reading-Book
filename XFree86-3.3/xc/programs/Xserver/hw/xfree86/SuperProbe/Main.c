/* $XConsortium: Main.c /main/11 1996/10/25 11:33:16 kaleb $ */
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

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Main.c,v 3.17.2.4 1997/05/18 12:00:05 dawes Exp $ */

#include "Probe.h"
#include "PatchLevel.h"

char MyName[20];
Word vgaIOBase;
Bool Verbose = FALSE;
Long Chip_data = (Long)~0;
Byte *Bios_Base = (Byte *)0;
Bool NoBIOS = FALSE;
Bool NoPCI = FALSE;
Bool PCIProbed = FALSE;
struct pci_config_reg *pcrp;

static Bool No16Bits = FALSE;
static Bool Exclusions = FALSE;
static Bool Mask10 = FALSE;
static Range *Excl_List = NULL;
static char *NoProbe = (char *)0;

static char *StdStuff = "MDA, Hercules, CGA, MCGA, EGA, VGA";

static Chip_Descriptor *SVGA_Descriptors[] = {
    &WD_Descriptor,
    &Video7_Descriptor,
    &MX_Descriptor,
    &Genoa_Descriptor,
    &UMC_Descriptor,
    &Trident_Descriptor,
    &SiS_Descriptor,
    &Matrox_Descriptor,
    &ATI_Descriptor,
    &Ahead_Descriptor,
    &NCR_Descriptor,
    &S3_Descriptor,
    &AL_Descriptor,
    &Cirrus54_Descriptor,
    &Cirrus64_Descriptor,
    &Tseng_Descriptor,
    &RealTek_Descriptor,
    &Primus_Descriptor,
    &Yamaha_Descriptor,
    &Oak_Descriptor,
    &Cirrus_Descriptor,
    &Compaq_Descriptor,
    &HMC_Descriptor,
    &Weitek_Descriptor,
    &ARK_Descriptor,
    &Alliance_Descriptor,
    &SigmaDesigns_Descriptor,
    &CT_Descriptor, /* I think this is screwing people up, so put it last */
    NULL
};

static Chip_Descriptor *CoProc_Descriptors[] = {
    &ATIMach_Descriptor,
    &IBM8514_Descriptor,	/* Make this the last 8514-type entry */
    &I128_Descriptor,
    &GLINT_Descriptor,
    NULL
};

static int num_tokens __STDCARGS((char *, char));
static void ParseExclusionList __STDCARGS((Range *, char *));
static Bool TestChip __STDCARGS((Chip_Descriptor *, int *));
static void PrintInfo __STDCARGS((void));
static Byte *FindBios __STDCARGS((void));

#ifdef __STDC__
static int num_tokens(char *list, char delim)
#else
static int num_tokens(list, delim)
char *list;
char delim;
#endif
{
    char *p = list;
    int cnt = 1;

    while ((p = strchr(p, delim)) != NULL)
    {
    	p++;
    	cnt++;
    }
    return(cnt);
}

static void ParseExclusionList(excl_list, list)
Range *excl_list;
char *list;
{
    char *p = list, *p1, c;
    Bool done = FALSE, lo = TRUE;
    int i = 0;

    while (!done)
    {
    	p1 = strpbrk(p, "-,");
    	if (p1)
    	{
    	    c = *p1;
    	    *p1 = '\0';
    	    if ((c ==  '-') && (p == p1))
    	    {
    	    	fprintf(stderr, "%s: Unbounded range in exclusion\n", MyName);
    	    	exit(1);
    	    }
    	    else if (p == p1)
    	    {
    	    	p++;
    	    	continue;
    	    }
    	}
    	else
    	{
    	    if ((!lo) && (p == p1))
    	    {
    	    	fprintf(stderr, "%s: Unbounded range in exclusion\n", MyName);
    	    	exit(1);
    	    }
    	    done = TRUE;
    	}
    	if (lo)
    	{
    	    excl_list[i].lo = (Word)StrToUL(p);
    	}
    	else
    	{
    	    excl_list[i].hi = (Word)StrToUL(p);
    	}
    	if (!done)
    	{
    	    if (c == '-')
    	    {
    	    	lo = FALSE;
    	    }
    	    else
    	    {
    	    	if (lo)
    	    	{
    	    	    excl_list[i].hi = (Word)-1;
    	    	}
    	    	else
    	    	{
    	    	    lo = TRUE;
    	    	}
    	    	i++;
    	    }
    	    p = p1 + 1;
    	}
    	else
    	{
    	    if (lo)
    	    {
    	    	excl_list[i].hi = (Word)-1;
    	    }
    	    i++;
    	}
    }
    excl_list[i].lo = (Word)-1;
}

static Bool TestChip(chip_p, Chipset)
Chip_Descriptor *chip_p;
int *Chipset;
{
    char *p, *p1, name[64];

    if ((No16Bits) && (chip_p->bit16))
    {
    	if (Verbose)
    	{
    	    printf("\tSkipping %s (16-bit registers)...\n", chip_p->name);
    	    fflush(stdout);
    	}
    	return(FALSE);
    }
    if ((NoBIOS) && (chip_p->uses_bios))
    {
	if (Verbose)
	{
	    printf("\tSkipping %s (needs BIOS read)...\n", chip_p->name);
	    fflush(stdout);
	}
	return(FALSE);
    }
    if ((Exclusions) && (Excluded(Excl_List, chip_p, Mask10)))
    {
    	if (Verbose)
    	{
    	    printf("\tskipping %s (exclusion list)...\n", chip_p->name);
    	    fflush(stdout);
    	}
    	return(FALSE);
    }
    if (NoProbe != (char *)0)
    {
	p1 = p = NoProbe;
    	while (p)
    	{
    	    p1 = strchr(p1, ',');
    	    if (p1 != NULL)
    	    {
    	    	(void)strncpy(name, p, (p1-p) > sizeof(name) ? sizeof(name) : (p1-p));
		name[sizeof(name)] = '\0';
		p1++;
    	    }
	    else
	    {
		(void)strncpy(name, p, sizeof(name) - 1);
		name[sizeof(name)] = '\0';
	    }
	    if (StrCaseCmp(name, chip_p->name) == 0)
	    {
		if (Verbose)
    		{
    	    	    printf("\tskipping %s (noprobe list)...\n", chip_p->name);
    	    	    fflush(stdout);
    		}
    		return(FALSE);
	    }
    	    p = p1;
	}
    }
    if (Verbose)
    {
    	printf("\tProbing %s...\n", chip_p->name);
    	fflush(stdout);
    }
    if (chip_p->f(Chipset))
    {
    	return(TRUE);
    }
    return(FALSE);
}

static void PrintInfo()
{
    Chip_Descriptor *chip_p;
    int i, len;

    PRINT_VERSION;
    putchar('\n');
    printf("%s can detect the following standard video hardware:\n", MyName);
    printf("\t%s\n", StdStuff);
    printf("%s can detect the following SVGA chipsets/vendors:\n", MyName);
    len = 0;
    putchar('\t');
    for (i=0; SVGA_Descriptors[i] != NULL; i++)
    {
        chip_p = SVGA_Descriptors[i];
	len += strlen(chip_p->name) + 2;
	if (len > 70)
	{
	    printf("\n\t");
	    len = strlen(chip_p->name) + 2;
	}
        printf("%s, ", chip_p->name);
    }
    putchar('\n');
    printf("%s can detect the following graphics coprocessors/vendors:\n", 
	   MyName);
    len = 0;
    putchar('\t');
    for (i=0; CoProc_Descriptors[i] != NULL; i++)
    {
        chip_p = CoProc_Descriptors[i];
	len += strlen(chip_p->name) + 2;
	if (len > 70)
	{
	    printf("\n\t");
	    len = strlen(chip_p->name) + 2;
	}
        printf("%s, ", chip_p->name);
    }
    putchar('\n');
    printf("%s can detect the following RAMDACs:\n", MyName);
    len = 0;
    putchar('\t');
    for (i=0; i < DAC_MAX; i++)
    {
	len += strlen(RamDac_Names[i+1].Short) + 2;
	if (len > 70)
	{
	    printf("\n\t");
	    len = strlen(RamDac_Names[i+1].Short) + 2;
	}
        printf("%s, ", RamDac_Names[i+1].Short);
    }
    putchar('\n');
}

static Byte *FindBios()
{
    int i, score[7];
    Byte buf[3];
    Byte *base = (Byte *)0;

    for (i=0; i < 7; i++)
    {
	score[i] = 1;
    }
    for (i=0; i < 7; i++)
    {
	buf[0] = buf[1] = buf[2] = (Byte)0;
	if (ReadBIOS((unsigned)(0xC0000+(i<<15)), buf, 3) != 3)
	{
	    score[i] = 0;
	}
	else if ((buf[0] != 0x55) || (buf[1] != 0xAA) || (buf[2] < 48))
	{
	    score[i] = 0;
	}
    }
    for (i=6; i >= 0; i--)
    {
	if (score[i] != 0)
	{
	    base = (Byte *)(0xC0000+(i<<15));
	}
    }
    return(base);
}

int main(argc, argv)
int argc;
char *argv[];
{
    char *p, *order = NULL;
    Byte copyright[3];
    Word Ports[10];		/* For whatever we need */
    int Primary = -1;
    int Secondary = -1;
    int RamDac = -1;
    int CoProc = -1;
    int MemVGA = -1;
    int MemCoProc = -1;
    int i, cnt;
    Chip_Descriptor *chip_p, *matched;
    Bool Probe_DAC = TRUE;
    Bool Probe_Mem = TRUE;
    Bool NoEGA = FALSE;
    Bool PCIVGA = FALSE;
    Bool Check_CoProc;
    Bool flag;

    p = strrchr(argv[0], '/');
    if (p == NULL)
    	p = argv[0];
    else
    	p++;
    (void)strncpy(MyName, p, sizeof(MyName) - 1);
    MyName[sizeof(MyName)-1] = '\0';

    for (i=1; i < argc; i++)
    {
    	if (strncmp(argv[i], "-v", 2) == 0)
    	{
    	    Verbose = TRUE;
    	    continue;
    	}
    	else if (strncmp(argv[i], "-no16", 5) == 0)
    	{
    	    No16Bits = TRUE;
    	    continue;
    	}
    	else if ((strncmp(argv[i], "-excl", 5) == 0) || 
    		 (strncmp(argv[i], "-x", 2) == 0))
    	{
	    if (Excl_List != NULL)
	    {
		fprintf(stderr, 
			"%s: Warning - only one exclusion list allowed. ",
			MyName);
		fprintf(stderr, " The last one will be used\n");
	    }
    	    i++;
	    if (i == argc)
	    {
		fprintf(stderr, "%s: Exclusion list not specified\n", MyName);
		return(1);
	    }
    	    cnt = num_tokens(argv[i], ',');
    	    Excl_List = (Range *)calloc(cnt+1, sizeof(Range));
    	    ParseExclusionList(Excl_List, argv[i]);
    	    Exclusions = TRUE;
    	    continue;
    	}
    	else if ((strncmp(argv[i], "-mask10", 7) == 0) ||
    	 	 (strncmp(argv[i], "-msk10", 6) == 0) ||
    	 	 (strncmp(argv[i], "-10", 3) == 0))
    	{
    	    Mask10 = TRUE;
    	}
    	else if (strncmp(argv[i], "-o", 2) == 0)
    	{
	    if (order != NULL)
	    {
		fprintf(stderr, 
			"%s: Warning - only one order list allowed. ",
			MyName);
		fprintf(stderr, " The last one will be used\n");
	    }
	    i++;
	    if (i == argc)
	    {
		fprintf(stderr, "%s: Order list not specified\n", MyName);
		return(1);
	    }
    	    order = argv[i];
    	}
	else if (strncmp(argv[i], "-nopr", 5) == 0)
	{
	    if (NoProbe != NULL)
	    {
		fprintf(stderr,
			"%s: Warning - only one noprobe list allowed. ",
			MyName);
		fprintf(stderr, " The last one will be used\n");
	    }
	    i++;
	    if (i == argc)
	    {
		fprintf(stderr, "%s: noprobe list not specified\n", MyName);
		return(1);
	    }
	    NoProbe = argv[i];
	}
	else if (strncmp(argv[i], "-bios", 5) == 0)
	{
	    i++;
	    if (i == argc)
	    {
		fprintf(stderr, "%s: BIOS base address not specified\n", 
			MyName);
		return(1);
	    }
	    Bios_Base = (Byte *)StrToUL(argv[i]);
	}
	else if (strncmp(argv[i], "-no_pci", 7) == 0)
	{
	    NoPCI = TRUE;
	}
	else if (strncmp(argv[i], "-no_bios", 8) == 0)
	{
	    NoBIOS = TRUE;
	}
	else if (strncmp(argv[i], "-no_dac", 7) == 0)
	{
	    Probe_DAC = FALSE;
	}
	else if (strncmp(argv[i], "-no_mem", 7) == 0)
	{
	    Probe_Mem = FALSE;
	}
	else if (strncmp(argv[i], "-in", 3) == 0)
	{
	    PrintInfo();
	    return(0);
	}
	else if ((strncmp(argv[i], "-?", 2) == 0) ||
		 (strncmp(argv[i], "-h", 2) == 0))
	{
	    PRINT_VERSION;
	    printf("\n%s accepts the following options:\n", MyName);
	    printf("\t-verbose\tGive lots of information\n");
	    printf("\t-no16\t\tSkip any test that requires 16-bit ports\n");
	    printf("\t-excl list\tSkip any test that requires port on list\n");
	    printf("\t-mask10\t\tMask registers to 10 bits before comparing\n");
	    printf("\t\t\twith the exclusion list\n");
	    printf("\t-order list\tPerform test on the specified chipsets\n");
	    printf("\t\t\tin the specified order\n");
	    printf("\t-noprobe list\tDon't probe for any chipsets specified\n");
	    printf("\t-bios base\tSet BIOS base address to 'base'\n");
	    printf("\t-no_bios\tDon't read BIOS & assume EGA/VGA as primary\n");
	    printf("\t-no_dac\t\tDon't probe for RAMDAC type\n");
	    printf("\t-no_mem\t\tDon't probe for video memory\n");
	    printf("\t-no_pci\t\tDon't probe the PCI bus\n");
	    printf("\t-info\t\tPrint a list of the capabilities of %s\n",
		   MyName);
	    printf("\nRefer to the manual page '%s.1' for complete details\n",
		   MyName);
	    return(0);
	}
    	else
    	{
    	    fprintf(stderr, "%s: Unknown option '%s'\n", MyName, argv[i]);
    	    return(1);
    	}
    }

    PRINT_VERSION;
    printf("\t(c) Copyright 1993,1994 by David Wexelblat <dwex@xfree86.org>\n");
    printf("\n\tThis work is derived from the 'vgadoc2.zip' and\n");
    printf("\t'vgadoc3.zip' documentation packages produced by Finn\n");
    printf("\tThoegersen, and released with all appropriate permissions\n");
    printf("\thaving been obtained.  Additional information obtained from\n");
    printf("\t'Programmer's Guide to the EGA and VGA, 2nd ed', by Richard\n");
    printf("\tFerraro, and from manufacturer's data books\n\n");
    printf("The author welcomes bug reports and other comments mailed to\n");
    printf("the electronic mail address above.  In particular, reports of\n");
    printf("chipsets that this program fails to correctly detect are\n");
    printf("appreciated.\n\n");
    printf("Before submitting a report, please make sure that you have the\n");
    printf("latest version of SuperProbe (see http://www.xfree86.org/FAQ).\n\n");
{
    FILE *f;

    f = fopen("/dev/tty", "w");
#ifdef __EMX__
    /* note: don't remove the fopen above ! */
    fclose(f);
    f = stderr;	/* OS/2 does not know /dev/tty, OTOH you cannot run this
                 * program in a window.
		 */
#endif
    if (f != (FILE *)NULL)
    {
        putc('\007', f);
        putc('\007', f);
        putc('\007', f);
        fprintf(f, "WARNING - THIS SOFTWARE COULD HANG YOUR MACHINE.\n");
        fprintf(f, "          READ THE SuperProbe.1 MANUAL PAGE BEFORE\n");
        fprintf(f, "          RUNNING THIS PROGRAM.\n\n");
        fprintf(f, "          INTERRUPT WITHIN FIVE SECONDS TO ABORT!\n\n");
        putc('\007', f);
        putc('\007', f);
        putc('\007', f);
        fflush(f);
        sleep(5);
    }
}

    if (OpenVideo() < 0)
    {
    	fprintf(stderr, "%s: Cannot open video\n", MyName);
    	return(-1);
    }

    if (!NoBIOS)
    {
	if (Bios_Base == (Byte *)0)
	{
	    Bios_Base = FindBios();
	    if (Bios_Base == (Byte *)0)
	    {
		if (Verbose)
		{
		    printf("%s: Could not determine BIOS base address; %s",
			   MyName, " assuming no EGA/VGA present\n");
		    fflush(stdout);
		}
		NoEGA = TRUE;
	    }
	}
    }
    if (Verbose)
    {
	if (NoBIOS)
	{
	    printf("Assuming an EGA/VGA is present\n\n");
	}
	else
	{
	    printf("BIOS Base address = 0x%X\n\n", (int)Bios_Base);
	}
	fflush(stdout);
    }

    if (!NoPCI)
    {
	xf86scanpci();

        /* Now scan for VGA compatible cards in the PCI config space.
         * This will avoid that the BIOS checking fails to detect a VGA,
         * making SuperProbe report "MCGA =" ... [kmg]
         */
        i=0;
        while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
            if ((pcrp->_base_class == PCI_CLASS_PREHISTORIC &&
                   pcrp->_sub_class == PCI_SUBCLASS_PREHISTORIC_VGA) ||
                  (pcrp->_base_class == PCI_CLASS_DISPLAY &&
                   pcrp->_sub_class == PCI_SUBCLASS_DISPLAY_VGA))
            {
                    PCIVGA = TRUE;
            }
            i++;
        }
    }

    if ((!NoBIOS) && (!NoEGA))
    {
        /*
         * Look for 'IBM' at Bios_Base+0x1E of the BIOS.  It will be there for
         * an EGA or VGA.
         */
        if (ReadBIOS(COPYRIGHT_BASE, copyright, 3) < 0)
        {
    	    fprintf(stderr, "%s: Failed to read BIOS\n", MyName);
    	    CloseVideo();
    	    return(1);
        }
    }
    if (PCIVGA ||
       ((!NoEGA) &&
	((NoBIOS) ||
         ((copyright[0] == 'I') && 
          (copyright[1] == 'B') &&
          (copyright[2] == 'M')))))
    {
    	/*
    	 * It's an EGA or VGA
    	 */
    	Ports[0] = 0x3CC;
    	EnableIOPorts(1, Ports);
    	vgaIOBase = (inp(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
	DisableIOPorts(1, Ports);
    	if (vgaIOBase == 0x3D0)
    	{
    	    /*
	     * Color - probe for secondary mono.
    	     */
    	    if (Probe_MDA(&Secondary))
    	    {
    	    	(void)Probe_Herc(&Secondary);
    	    }
    	}
    	else
    	{
    	    /*
    	     * Mono - probe for secondary color.
    	     */
    	    (void)Probe_CGA(&Secondary);
    	}
    
    	if (!Probe_VGA(&Primary))
    	    Primary = CHIP_EGA;
    
    	/*
     	 * Do SVGA probing, even if no VGA is detected.  Some SVGA can emulate
	 * pre-VGA adapters (such as EGA) and be left in such a state by BIOS
	 * initialization.
     	 */
	if (Verbose)
	{
	    printf("Doing Super-VGA Probes...\n");
	    fflush(stdout);
	}
	matched = &VGA_Descriptor;

	if (order == NULL)
	{
	    /* 
	     * Use default ordering.
	     */
	    for (i=0; SVGA_Descriptors[i] != NULL; i++)
	    {
		chip_p = SVGA_Descriptors[i];
		if (TestChip(chip_p, &Primary))
		{
		    matched = chip_p;
		    break;
		}
	    }
	}
	else
	{
	    /*
	     * Use user specified order
	     */
	    p = order;
	    while (p)
	    {
		order = strchr(order, ',');
		if (order != NULL)
		{
		    *order = '\0';
		    order++;
		}
		flag = FALSE;
		for (i=0; SVGA_Descriptors[i] != NULL; i++)
		{
		    chip_p = SVGA_Descriptors[i];
		    if (StrCaseCmp(p, chip_p->name) == 0)
		    {
			flag = TRUE;
			break;
		    }
		}
		if (flag)
		{
		    if (TestChip(chip_p, &Primary))
		    {
			matched = chip_p;
			break;
		    }
		}
		else
		{
		    fprintf(stderr, "%s: Chip class '%s' not known\n",
			MyName, p);
		}
		p = order;
	    }
	}

	Check_CoProc = matched->check_coproc;

	if (Probe_Mem)
	{
	    if (matched->memcheck != (MemCheckFunc)NULL)
	    {
		MemVGA = (*matched->memcheck)(Primary);
	    }
	    else if (Verbose)
	    {
		printf("Memory probe not supported for this chipset.\n");
		fflush(stdout);
	    }
	}
	else if (Verbose)
	{
	    printf("Skipping memory probe\n");
	    fflush(stdout);
	}

	/*
	 * If this chipset doesn't exclude probing for a coprocessor,
	 * then look for one.
	 */
	if (Check_CoProc)
	{
	    if (Verbose)
	    {
		printf("\nDoing Graphics CoProcessor Probes...\n");
		fflush(stdout);
	    }
    	    for (i=0; CoProc_Descriptors[i] != NULL; i++)
    	    {
    	        chip_p = CoProc_Descriptors[i];
    	        if (TestChip(chip_p, &CoProc))
    	        {
		    matched = chip_p;
    	    	    break;
    	    	}
	    }
	    if (CoProc_Descriptors[i] != NULL)
	    {
	    	if (Probe_Mem)
	    	{
	    	    if (matched->memcheck != (MemCheckFunc)NULL)
	    	    {
		        MemCoProc = (*matched->memcheck)(CoProc);
	    	    }
		    else if (Verbose)
		    {
		        printf("Memory probe not supported ");
			printf("for this chipset.\n");
		        fflush(stdout);
		    }
	        }
	        else if (Verbose)
	        {
		    printf("Skipping memory probe\n");
		    fflush(stdout);
	        }
	    }
	}
    }
    else if (Probe_MDA(&Primary))
    {
    	(void)Probe_Herc(&Primary);
    	(void)Probe_CGA(&Secondary);
    }
    else if (Probe_CGA(&Primary))
    {
    	if (Probe_MDA(&Secondary))
    	{
    	    (void)Probe_Herc(&Secondary);
    	}
    }

    putchar('\n');
    if (Primary == -1)
    {
    	printf("Could not identify any video\n");
    }
    else 
    {
    	printf("First video: ");
    	if (IS_MDA(Primary))
    	{
    	    printf("MDA\n");
    	}
    	else if (IS_HERC(Primary))
    	{
    	    printf("Hercules\n");
    	    Print_Herc_Name(Primary);
    	}
    	else if (IS_CGA(Primary))
    	{
    	    printf("CGA\n");
    	}
    	else if (IS_MCGA(Primary))
    	{
    	    printf("MCGA\n");
    	}
    	else if (IS_EGA(Primary))
    	{
    	    printf("EGA\n");
    	}
    	else if (IS_VGA(Primary))
    	{
    	    printf("Generic VGA (or unknown SVGA)\n");
    	}
    	else if (IS_SVGA(Primary))
    	{
    	    printf("Super-VGA\n");
    	    Print_SVGA_Name(Primary);
	    if (MemVGA > -1)
	    {
	       if (MemVGA >> 16)
	       {
		  printf("\tMemory:  %d Kbytes (plus %d Kbytes off-screen)\n", 
			 MemVGA & 0xffff, MemVGA >> 16);
	       }
	       else 
		  printf("\tMemory:  %d Kbytes\n", MemVGA);
	    }
	    if (Probe_DAC)
	    {
    	        Probe_RamDac(Primary, &RamDac);
    	        Print_RamDac_Name(RamDac);
	    }
	    else if (Verbose)
	    {
		printf("Skipping RAMDAC probe\n");
	    }
    	}
	if (CoProc != -1)
	{
	    printf("\tAttached graphics coprocessor:\n");
	    Print_CoProc_Name(CoProc);
	    if (MemCoProc > -1)
	    {
		if (MemCoProc > 0x00010000)
		{
		    /*
		     * for some copros we store the local buffer in
		     * the high order 16bits
		     */
		    
		    printf("\t\tMemory:  %d Kbytes Framebuffer, %d Kbytes Localbuffer\n", 
			MemCoProc & 0xffff, (MemCoProc & 0xffff0000)>>16);
		}
		else
		{
		    printf("\t\tMemory:  %d Kbytes\n", MemCoProc);
		}
	    }
	}
    }
    if (Secondary != -1)
    {
    	printf("Second video: ");
    	if (IS_MDA(Secondary))
    	{
    	    printf("MDA\n");
    	}
    	else if (IS_HERC(Secondary))
    	{
    	    printf("Hercules\n");
    	    Print_Herc_Name(Secondary);
    	}
    	else if (IS_CGA(Secondary))
    	{
    	    printf("CGA\n");
    	}
    	else if (IS_MCGA(Secondary))
    	{
    	    printf("MCGA\n");
    	}
    }

    CloseVideo();
    return(0);
}
