/* $XFree86: xc/programs/Xserver/hw/xfree98/accel/s3nec/s3pc98.c,v 3.8 1996/12/23 07:05:51 dawes Exp $ */




/* $XConsortium: s3pc98.c /main/7 1996/10/23 13:22:34 kaleb $ */

#include "misc.h"
#include "cfb.h"
#include "pixmapstr.h"
#include "fontstruct.h"
#include "s3.h"
#include "regs3.h"
#include "xf86_HWlib.h"
#include "xf86_Config.h"
#include "s3linear.h"
#include "s3Bt485.h"

extern int xf86bpp;

#include "pc98_vers.h"
#include "s3pc98.h"

#define MasterClock		52000000l	/* 52MHz */	 
#undef chipID
#undef _port_tbl
short		chipID = S3_UNKNOWN;
unsigned short	_port_tbl[0x3e0];
unsigned int	necIndex,necData;
pointer		mmioBase = NULL;
int		pc98BoardType;
static pointer	PWLBctrlPort = NULL;

/* MCLK = 62.56 MHz   m = 0x7f, n = 0x2d */
static unsigned char sdac_data[ 30 ] = {
	0x28, 0x61, 0x3d, 0x62, 0x28, 0x22, 0x77, 0x4a,	0x79, 0x49,
	0x62, 0x46, 0x6b, 0x2a, 0x52, 0x26, 0x52, 0x26, 0x52, 0x26,
	0x7f, 0x2d, 0x7f, 0x2d,	0x7f, 0x2d, 0x7f, 0x2d, 0x22, 0x22
};

/* MCLK = 65.63 MHz   m = 0x35, n = 0x41 */
static unsigned char sdac_data805[ 30 ] = {
	0x28, 0x61, 0x3d, 0x62, 0x2a, 0x43, 0x77, 0x4a,	0x79, 0x49,
	0x62, 0x46, 0x6b, 0x2a, 0x50, 0x42, 0x50, 0x42, 0x50, 0x42,
	0x35, 0x41, 0x35, 0x41,	0x35, 0x41, 0x35, 0x41, 0x27, 0x27
};

static unsigned char seqreg_data[ 0x05 ] = {
	0x03, 0x01, 0x0f, 0x00, 0x0e
};

static unsigned char grctrl_data[ 0x09 ] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff
};

static unsigned char vgareg_data[ 0x70 ] = {
	0xa1, 0x7f, 0x80, 0x84, 0x84, 0x95, 0x24, 0xe5, /* 00 - 07 */
	0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, /* 08 - 0F */
	0x02, 0x88, 0xff, 0x80, 0x60, 0xff, 0x24, 0xe3, /* 10 - 17 */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 18 - 1F */
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 20 - 27 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 28 - 2F */
	0xc0, 0x8d, 0x00, 0x00, 0x10, 0x30, 0x8d, 0xfd, /* 30 - 37 */
	0x79, 0xbd, 0x95, 0x9a, 0x00, 0x00, 0x00, 0x00, /* 38 - 3F */
	0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0xff, /* 40 - 47 */
	0xfc, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xc0, 0xc0, /* 48 - 4F */
	0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, /* 50 - 57 */
	0x0a, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, /* 58 - 5F */
	0xff, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, /* 60 - 67 */
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* 68 - 6F */
};

static unsigned char vgareg_data805[ 0x70 ] = {
	0x98, 0x7F, 0x7F, 0x99, 0x80, 0x19, 0x9D, 0x1F, /* 00 - 07 */
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* 08 - 0F */
	0x80, 0x2B, 0x7F, 0x80, 0x00, 0x80, 0x81, 0xE3, /* 10 - 17 */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* 18 - 1F */
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 20 - 27 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 28 - 2F */
	0xA8, 0x8D, 0x00, 0x20, 0x10, 0x0C, 0x83, 0x0F, /* 30 - 37 */
	0x4B, 0xAD, 0xB5, 0x8C, 0x4C, 0x4C, 0x4C, 0x4C, /* 38 - 3F */
	0x01, 0xF9, 0x27, 0x00, 0x00, 0x01, 0x01, 0x83, /* 40 - 47 */
	0x81, 0x8E, 0x00, 0xC0, 0xC3, 0x00, 0x00, 0x00, /* 48 - 4F */
	0x00, 0x00, 0x00, 0x20, 0xF8, 0x40, 0x00, 0x00, /* 50 - 57 */
	0x00, 0x00, 0x0A, 0x00, 0x07, 0x00, 0x40, 0x00, /* 58 - 5F */
	0xFF, 0x81, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, /* 60 - 67 */
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F  /* 68 - 6F */
};
	
void crtswitch(short);
int  pwboardinit(void);
int  pw805iboardinit(void);
void skbboardinit(void);
void skb4boardinit(void);
void hkbboardinit(void);
void ga968boardinit(void);
Bool necboardinit(void);
Bool BoardInit();
int  pwlbboardinit(void);
void PWLBctrl(unsigned char);
static void Wait0(void);
static void unlocks3regs(void);
static void _pwportconvert(void);
static void _hkbportconvert(void);
static void _necportconvert(void);
static void _skbportconvert(void);
static void _skb4portconvert(void);
static int  PWBoardCheck(unsigned char PW_Switch);
static int  PWLBBoardCheck(unsigned char PW_Switch);
static void SetSDACClock( unsigned char *sdac );
static void GetSDACClock( void );
static void SetRegisters( unsigned char *seqreg,unsigned char *grctrl,unsigned char *vgareg);
static void GetRegisters( void );

static void Wait0(void)
{
	inb(vgaCRIndex);
	inb(vgaCRIndex);
	inb(vgaCRIndex);
	inb(vgaCRIndex);
}

static void unlocks3regs(void)
{
	wrinx(vgaCRIndex, 0x38, 0x48);
	wrinx(vgaCRIndex, 0x39, 0xa5);
/*	wrinx(vgaCRIndex, 0x39, 0xa0); */
}

void crtswitch(short crtmod)
{
	unsigned char	tmp;
	UNLOCK_SYS_REGS;

	switch(pc98BoardType){
	case PW:
	case PW805I:
	case PW968:
		if(crtmod)
			tmp = rdinx(vgaCRIndex, 0x41) | 1;
		else
			tmp = rdinx(vgaCRIndex, 0x41) & 0xfe;
		wrinx(vgaCRIndex, 0x41, tmp);
		_outb(0x0dc|PW_PORT, tmp);
		break;
	case PWLB:
		if(crtmod)
			tmp = rdinx(vgaCRIndex, 0x41) | 1;
		else
			tmp = rdinx(vgaCRIndex, 0x41) & 0xfe;
		wrinx(vgaCRIndex, 0x41, tmp);
		PWLBctrl(tmp);
		break;
	case PCSKB:
	case PCHKB:
	case PCSKB4:
		if(crtmod == 0)
			outb(0x6e,0);
		else
			outb(0x6e,0x3b);
		break;
	case NECWAB928_E_C:
	case NECWAB928_E_L:
	case NECWAB928_I:
		if(crtmod == 0){
			_outb(necIndex,3);
			_outb(necData,1);
		}else{
			_outb(necIndex,3);
			_outb(necData,3);
		}	
		break;
	case NECWAB864_I:
		if(crtmod == 0){
			_outb(0x6a, 0x07);
			_outb(0x6a, 0x8e);
			_outb(0x6a, 0x06);
			_outb(0x7c, 0x00); /* Is this needed? */
			_outb(0x68, 0x0f);
			_outb(necIndex, 3);
			_outb(necData , 0x01);
			_outb(necIndex, 6);
			_outb(necData , 0xff);
			_outb(necIndex, 7);
			_outb(necData , 0x00);
		}else{
			_outb(0x68, 0x0e);
			_outb(0x7c, 0x80); /* Is this needed? */
			_outb(0x6a, 0x07);
			_outb(0x6a, 0x8f);
			_outb(0x6a, 0x06);
			_outb(necIndex, 3);
			_outb(necData , 0x03);
			_outb(necIndex, 6);
			_outb(necData , 0x00);
			_outb(necIndex, 7);
			_outb(necData , 0xff);
		}
		break;
	case GA968:
		if (crtmod) {
			_outb(0xfac, 1);
		} else {
			_outb(0xfac, 0);
		}	
		break;
	default:
		ErrorF("PC98: Unknown board type.\n");
	}	
}


Bool BoardInit()
{
	switch(pc98BoardType & 0xf0){
	case PW:
		if(pwboardinit()) {
      			ErrorF("PW: chipset unknown\n");
      			xf86DisableIOPorts(s3InfoRec.scrnIndex);
      			return(FALSE);
   		}
		break;

	case PW805I:
		ErrorF("   PC98: Changing PW805i mode\n");
		if(pw805iboardinit()) {
      			ErrorF("PW805i: chipset unknown\n");
      			xf86DisableIOPorts(s3InfoRec.scrnIndex);
      			return(FALSE);
   		}
		pw805iboardinit(); /* Initializing PW805i 2 times */
		break;

	case PW968:
		if(pw968boardinit()) {
      			ErrorF("PW968: chipset unknown\n");
      			xf86DisableIOPorts(s3InfoRec.scrnIndex);
      			return(FALSE);
   		}
		break;

	case PWLB:
		ErrorF("   PC98: Changing PW localbus mode\n");
		if(pwlbboardinit()) {
      			ErrorF("PWLB: chipset unknown\n");
      			xf86DisableIOPorts(s3InfoRec.scrnIndex);
      			return(FALSE);
   		}
		break;
	case PCHKB:		
		ErrorF("   PC98: Changing PCHKB mode\n");
		hkbboardinit();
		break;
	case PCSKB:
		ErrorF("   PC98: Changing PCSKB mode\n");
		skbboardinit();
		break;
	case PCSKB4:
		ErrorF("   PC98: Changing PCSKB4 mode\n");
		skb4boardinit();
		break;
	case NECWAB:
		ErrorF("   PC98: Changing NEC-WAB mode\n");
		necboardinit();
		break;
	case GA968:
		ga968boardinit();
		break;
	default:
		ErrorF("   PC98: Unknown Board Type. \n");
		return(FALSE);
	}
	return(TRUE);
}
	

Bool s3EnterLeaveMachdep(int dir)
{
	Bool error = TRUE;

 	if (dir == S3PC98SERVER_ENTER)
	{
#ifdef PC98_GA968
		s3ConnectPCI(PCI_S3_VENDOR_ID, PCI_968);
#endif
#ifdef PC98_NEC
		if (mmioBase != NULL && pc98BoardType == NECWAB864_I)
			error = BoardInit();
#endif
	}
	else
	{
#ifdef PC98_GA968
		   s3DisconnectPCI(PCI_S3_VENDOR_ID, PCI_968);
#endif
	}

	return error;
}

static int PWBoardCheck(unsigned char PW_Switch)
{
	unsigned int tmp;

	_outb(0x0dc|PW_PORT, PW_Switch);
	Wait0();
	outb(ROM_PAGE_SEL, 0x1e);
	Wait0();
	_outb(0x0d2|PW_PORT, 1);
	Wait0();
	outb(ROM_PAGE_SEL, 0x0e);
	Wait0();
	outw(ADVFUNC_CNTL, 0000);
	Wait0();
	outb(0x3c2, 0xa3);
	Wait0();
	unlocks3regs();
	tmp = rdinx(vgaCRIndex, 0x30);
	switch(tmp & 0xf0) {
		case 0x80:
			chipID = S3_924;
			return -1;	
		case 0x90:
			chipID = S3_928;
			_outb(0x0da|PW_PORT, 3);
			if(_inb(0x0de|PW_PORT) == 1) {
				_port_tbl[0x3c7] = 0x0d6|PW_PORT;
				_port_tbl[0x3c9] = 0x0d8|PW_PORT;
			} else {
				_port_tbl[0x3c7] = 0x10d7|PW_PORT;
				_port_tbl[0x3c9] = 0x10d9|PW_PORT;
			}
			break;
		case 0xa0:
			chipID = S3_801;
			_port_tbl[0x3c7] = 0x0d6|PW_PORT;
			_port_tbl[0x3c9] = 0x0d8|PW_PORT;
			if(tmp >= 0xa8){
				chipID = S3_805;
			}
			break;
		case 0xe0:
		case 0xf0:
			outb(vgaCRIndex, 0x2e);
			tmp |= (inb(vgaCRReg) << 8);
			ErrorF("PC98 : chipID = %X \n",tmp);
			if((tmp & 0xfff0) == 0xf0e0 ) {
			  chipID = S3_968;
			  _port_tbl[0x3c7] = 0x0d6|PW_PORT;
			  _port_tbl[0x3c9] = 0x0d8|PW_PORT;
			}else{
			  chipID = S3_UNKNOWN;
			  return -1;
			}
			break;
		default:
			chipID = S3_UNKNOWN;
			return -1;
	}
	return 0;
}

int pwboardinit()
{
	unsigned char comm;
	unsigned char PW_Switch = 0xf0; /* 0xf0 | Quicksaver=4 */
	vgaCRIndex = 0x3d4;
	if(chipID == S3_UNKNOWN) {
		_pwportconvert();
		_outb(0x0dc|PW_PORT, 0);
		AltICD2061SetClock(MasterClock, 1);
		usleep(150000);
		if(PWBoardCheck(PW_Switch)) {
			crtswitch(0);
			return -1;
		}
	}
	if(OFLG_ISSET(OPTION_EPSON_MEM_WIN, &s3InfoRec.options)) {
		unsigned char tmp = PW_WinAdd;
		if(tmp >= 0xf0)
			tmp = _inb(0x43b);
			_outb(0x43b, tmp & 0xfd);
		}
 	if(!OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
		_outb(0x0da|PW_PORT, 3);
		if(_inb(0x0de|PW_PORT) == 1) {
			_outb(0x0da|PW_PORT, 4);
			_outb(0x0de|PW_PORT, PW_WinAdd | 1);		    
		} else
		    if(chipID == S3_928) {
		    	_outb(0xfa2, 1);
		      	_outb(0xfa3, PW_WinAdd);		      
		    }
		PW_Switch |= 8;
	} 
	wrinx(vgaCRIndex, 0x41, PW_Switch);
	_outb(0x0dc|PW_PORT, PW_Switch);
	usleep(150000);

      switch (xf86bpp) { /* Following part was got from s3init.c. Why need? */
      case 8: 
         comm = 0;  /* repack mode 0, color mode 0 */
         break;
      case 15:
         comm = 0x80;  /* repack mode 1a using both clock edges */
	 break;
      case 16:
         comm = 0xc0;  /* repack mode 1a using both clock edges */
         break;
      case 32:
         comm = 0x40;  /* repack mode 3a using both clock edges */
         break;
      default:
         comm = 0;  /* repack mode 0, color mode 0 */
      }

      comm |= 0x08;  /* enable LUT for gamma correction */

	inb(0x3c7);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6);
 	inb(0x3c6); 
  	outb(0x3c6, comm);
	inb(0x3c7);
	return 0;
}

int pw805iboardinit()
{
	unsigned char PW_Switch = 0xf0; /* 0xf0 | Quicksaver=4 */
	vgaCRIndex = 0x3d4;
		_pwportconvert();
		_port_tbl[0x3c7] = 0x0d6|PW_PORT;
		_port_tbl[0x3c9] = 0x0d8|PW_PORT;
		SetSDACClock(sdac_data805);
/*		GetRegisters(); */
	        SetRegisters(seqreg_data,grctrl_data,vgareg_data805);
		usleep(150000);
		if(PWBoardCheck(PW_Switch)) {
			crtswitch(0);
			return -1;
		}
	if(OFLG_ISSET(OPTION_EPSON_MEM_WIN, &s3InfoRec.options)) {
		unsigned char tmp = PW_WinAdd;
		if(tmp >= 0xf0)
			tmp = _inb(0x43b);
			_outb(0x43b, tmp & 0xfd);
		}
 	if(!OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
			_outb(0x0da|PW_PORT, 4);
			_outb(0x0de|PW_PORT, PW_WinAdd | 1);		    
		PW_Switch |= 0x88;
	}
	wrinx(vgaCRIndex, 0x41, PW_Switch);
	_outb(0x0dc|PW_PORT, PW_Switch);
	usleep(150000);
}



static int PWLBBoardCheck(unsigned char PW_Switch)
{
	unsigned char tmp;

	PWLBctrl(PW_Switch);
	usleep(1000);
	outb(ROM_PAGE_SEL, 0x1e);
	usleep(100);
	outb(0x102, 1);
	usleep(100);
	outb(ROM_PAGE_SEL, 0x0e);
	usleep(100);
	outw(ADVFUNC_CNTL, 0000);
	usleep(100);
	outb(0x3c2, 0xa3);
	usleep(100);
	unlocks3regs();
	wrinx(vgaCRIndex, 0x40, 0x95);
	tmp = rdinx(vgaCRIndex, 0x30);
	switch(tmp & 0xf0) {
		case 0x90:
			chipID = S3_928;
			break;
		case 0xD0:
			chipID = S3_964;
			break;
		default:
			chipID = S3_UNKNOWN;
			return -1;
	}
	return 0;
}

int pwlbboardinit()
{
	unsigned char comm;
	unsigned char PW_Switch = 0xf0; /* 0xf0 | Quicksaver=4 */

	mmioBase = xf86MapVidMem(0, LINEAR_REGION,
			(pointer)((PWLB_WinAdd << 16) + 0x800000), 65536);
	PWLBctrlPort = xf86MapVidMem(0, LINEAR_REGION,
			(pointer)((PWLB_WinAdd << 16) + 0xC00000), 1);
	vgaCRIndex = 0x3d4;
	if(chipID == S3_UNKNOWN) {
		PWLBctrl(0);
		AltICD2061SetClock(MasterClock, 1);
		usleep(150000);
		if(PWLBBoardCheck(PW_Switch)) {
			crtswitch(0);
			return -1;
		}
	}
 	if(!OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
		PW_Switch |= 8;
	} 
	wrinx(vgaCRIndex, 0x41, PW_Switch);
	PWLBctrl(PW_Switch);
	usleep(150000);

	switch (xf86bpp) { /* Following part was got from s3init.c. Why need? */
	case 8: 
		comm = 0;  /* repack mode 0, color mode 0 */
		break;
	case 15:
		comm = 0x80;  /* repack mode 1a using both clock edges */
		break;
	case 16:
		comm = 0xc0;  /* repack mode 1a using both clock edges */
		break;
	case 32:
		comm = 0x40;  /* repack mode 3a using both clock edges */
		break;
	default:
		comm = 0;  /* repack mode 0, color mode 0 */
	}

	comm |= 0x08;  /* enable LUT for gamma correction */

	inb(0x3c7);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6); 
	outb(0x3c6, comm);
	inb(0x3c7);
	return 0;
}

void PWLBctrl(unsigned char a)
{
	*(volatile unsigned char *)PWLBctrlPort = a;
}

int pw968boardinit()
{
	unsigned char PW_Switch = 0xf0; /* 0xf0 | Quicksaver=4 */
	vgaCRIndex = 0x3d4;
	if(chipID == S3_UNKNOWN) {
		_pwportconvert();
		_outb(0x0dc|PW_PORT, 0);

		usleep(150000);
		if(PWBoardCheck(PW_Switch)) {
			crtswitch(0);
			return -1;
		}
	}
	if(OFLG_ISSET(OPTION_EPSON_MEM_WIN, &s3InfoRec.options)) {
		unsigned char tmp = PW_WinAdd;
		if(tmp >= 0xf0)
			tmp = _inb(0x43b);
			_outb(0x43b, tmp & 0xfd);
		}
 	if(!OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
		_outb(0x0da|PW_PORT, 4);
		_outb(0x0de|PW_PORT, PW_WinAdd | 1);		    
		PW_Switch |= 8;
	} 
	wrinx(vgaCRIndex, 0x41, PW_Switch);
	_outb(0x0dc|PW_PORT, PW_Switch);
	usleep(150000);

	return 0;
}


void skbboardinit()
{
  int i,WAIT;
  WAIT = 20000;
   _skbportconvert();		
   outb(0x6E,0x39);
   for(i=1;i< (WAIT * 3);i++) inb(0x03DA);
   outb(0x46,0x10);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x102,0x01);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x46,0x08);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03C2,0xED);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outw(0x03D4,0xA039);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03D4,0x40);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03D5,0x41);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outw(0x4A,0x0007);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x6E,0x3B);
   for(i=1;i< (WAIT * 3);i++) inb(0x03DA);
/* outw(0x03D4,0x0742);
   outw(0x03D4,0x9C3B);
   outw(0x03D4,0xFF3C); */
/* for(i=1;i< 50000;i++) inb(0x03DA); */
}

void hkbboardinit()
{
  int i,WAIT;
  WAIT = 20000;
   _hkbportconvert();		
   outb(0x6E,0x39);
   for(i=1;i< (WAIT * 3);i++) inb(0x03DA);
   outb(0x46,0x10);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x102,0x01);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x46,0x08);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03C2,0xED);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outw(0x03D4,0xA039);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03D4,0x40);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03D5,0x41);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outw(0x4A,0x0007);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x6E,0x3B);
   for(i=1;i< (WAIT * 3);i++) inb(0x03DA);
/* outw(0x03D4,0x0742);
   outw(0x03D4,0x9C3B);
   outw(0x03D4,0xFF3C); */
/* for(i=1;i< 50000;i++) inb(0x03DA); */
}

void skb4boardinit()
{
  int i,WAIT;
  WAIT = 20000;
   _skb4portconvert();		
   outb(0x3C2,0x0);
   _outb(0x8c70,0x4);
   outb(0x6E,0x39);
   for(i=1;i< (WAIT * 3);i++) inb(0x03DA);
   outb(0x46,0x10);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x102,0x01);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x46,0x08);
/*    for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03C2,0x2D); */
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03C2,0xED);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outw(0x03D4,0xA039);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03D4,0x40);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outb(0x03D5,0xA1);
   for(i=1;i< WAIT;i++) inb(0x03DA);
   outw(0x4A,0x0006);
   for(i=1;i< WAIT;i++) inb(0x03DA);
    outb(0x6E,0x3B);
   for(i=1;i< (WAIT * 3);i++) inb(0x03DA);
/* outw(0x03D4,0x0742);
   outw(0x03D4,0x9C3B);
   outw(0x03D4,0xFF3C); */
/*    outw(0x03D4,0x8A31);
   outw(0x03D4,0x4032);
   outw(0x03D4,0x853A); 
   outb(0x03C0,0x30);  */

/*   _outb(0x8C72,0x0); 
   _outb(0x8C73,0x02); 
   _outb(0x8C7b,0x04); 
   _outb(0x8C7D,0x00); 
   _outb(0x8C7E,0x00); 
   _outb(0x8C70,0xC4);  */
}


Bool necboardinit()
{
	unsigned char tmp;
	int	i;

	vgaCRIndex = 0x3d4;
	vgaCRReg   = 0x3d5;
	if (mmioBase == NULL)
		mmioBase = xf86MapVidMem(0, VGA_REGION,(pointer)(0xdf000), 4096);
   	_necportconvert();		

 	_outb(PC98_NEC_INDX1, 0);
	_outb(PC98_NEC_DATA1, 0x0FF);
	tmp = _inb(PC98_NEC_DATA1);
	if(tmp != 0xff){
		necIndex = PC98_NEC_INDX1;
		necData  = PC98_NEC_DATA1;
	}else{
		necIndex = PC98_NEC_INDX2;
		necData  = PC98_NEC_DATA2;
	}

	_outb(necIndex, 0);
	_outb(necData, 0xff);
	tmp = _inb(necData);
	switch(tmp){
		case 0x10: /* WAB-A */
		case 0x11: /* FC-WAB-A */
			pc98BoardType = NECWAB928_E_L;
			break;

		case 0x12: /* Internel 928 */
			pc98BoardType = NECWAB928_I;
			break;

		case 0x18: /* Internel 864 */
			pc98BoardType = NECWAB864_I;
			_outb(0x68, 0x0e);
			_outb(0x7c, 0x80); /* Is this needed? */
			_outb(0x6a, 0x07);
			_outb(0x6a, 0x8f);
			_outb(0x6a, 0x06);
			break;

		case 0x20: /* WAB-B */
		case 0x21: /* FC-WAB-B */
			pc98BoardType = NECWAB928_E_C;
			break;

		default:
			ErrorF("   PC98:Unknown return code ... %02X\n",tmp);
			return(FALSE);
	}

	_outb(necIndex, 1);    
	_outb(necData , 0x5f);    /* VGA registers was mapped at 0xdf000 */
/*	_outb(necData , 0x80); */ /* VGA registers was mapped at 0xf80000 */

	_outb(necIndex, 2);
	_outb(necData , 0x00);
	_outb(necIndex, 3);
	if(pc98BoardType == NECWAB864_I){
		_outb(necData , 0x03); /* chip enable & relay on? */
	}else{
		_outb(necData , 0x01); /* chip enable */
	}

	outb(ROM_PAGE_SEL,0x1e);
	outb(0x102,0x01);
	outb(ROM_PAGE_SEL,0x0e);
	outw(ADVFUNC_CNTL, 0x0005);
	outb(0x3c2,0xcd);
	outb(0x3da,0x00);
	wrinx(vgaCRIndex, 0x38, 0x48);
	wrinx(vgaCRIndex, 0x39, 0xa0);
	wrinx(vgaCRIndex, 0x40, 0xa5);

/*	GetRegisters(); */
	if (OFLG_ISSET(CLOCK_OPTION_S3GENDAC, &s3InfoRec.clockOptions)){
		if(!OFLG_ISSET(OPTION_NOINIT, &s3InfoRec.options)){
/*			GetSDACClock(); */
			SetSDACClock(sdac_data);
/* 			GetSDACClock(); */
		        SetRegisters(seqreg_data,grctrl_data,vgareg_data);
/*			GetRegisters(); */
		} 
	}
	return (TRUE);	
}

void ga968boardinit(void)
{
	vgaCRIndex = 0x3d4;

	_outb(0x37, 8);
	outb(ROM_PAGE_SEL, 0x10);
	outb(0x102, 1);
	outb(ROM_PAGE_SEL, 0x08);
	outw(ADVFUNC_CNTL, 0000);
	outb(0x3c6, 0xFF);
	outb(0x3c2, 0x23);
	unlocks3regs();
	wrinx(vgaCRIndex, 0x40, 0xD1);

	outw(0x3D4, 0xD140);
	outw(0x3D4, 0x0055);
	outw(0x3D4, 0x705C);

	outw(0x3D4, 0x0F60);
	outw(0x3D4, 0x8061);
	outw(0x3D4, 0xA162);
	outw(0x3D4, 0x0063);
	outw(0x3D4, 0x0064);
	outw(0x3D4, 0x0065);
	outw(0x3D4, 0x0066);
	outw(0x3D4, 0x1067);
	outw(0x3D4, 0xEC68);
	outw(0x3D4, 0x0069);
	outw(0x3D4, 0x006A);
	outw(0x3D4, 0x806B);
	outw(0x3D4, 0x006C);
	outw(0x3D4, 0x006D);

	outw(0x3CE, 0x0000);
	outw(0x3CE, 0x0001);
	outw(0x3CE, 0x0002);
	outw(0x3CE, 0x0003);
	outw(0x3CE, 0x0004);
	outw(0x3CE, 0x4005);
	outw(0x3CE, 0x0506);
	outw(0x3CE, 0x0F07);
	outw(0x3CE, 0xFF08);

	outb(0x3C0, 0x30);
	outb(0x3C0, 0x41);
	outb(0x3C0, 0x31);
	outb(0x3C0, 0x00);
	outb(0x3C0, 0x32);
	outb(0x3C0, 0x0F);
	outb(0x3C0, 0x33);
	outb(0x3C0, 0x00);
	outb(0x3C0, 0x34);
	outb(0x3C0, 0x00);

	outw(0x3D4, 0x0155);

	outb(0x3C9, 0x00);	/* IBM RAMDAC index high-8bit */

	outb(0x3C8, 0x02); outb(0x3C6, 0x01);
	outb(0x3C8, 0x03); outb(0x3C6, 0x00);
	outb(0x3C8, 0x04); outb(0x3C6, 0x00);
	outb(0x3C8, 0x05); outb(0x3C6, 0x00);
	outb(0x3C8, 0x06); outb(0x3C6, 0x02);
	outb(0x3C8, 0x07); outb(0x3C6, 0x00);
	outb(0x3C8, 0x08); outb(0x3C6, 0x01);
	outb(0x3C8, 0x0A); outb(0x3C6, 0x03);
	outb(0x3C8, 0x0B); outb(0x3C6, 0x00);
	outb(0x3C8, 0x0C); outb(0x3C6, 0xC2);
	outb(0x3C8, 0x0D); outb(0x3C6, 0x00);
	outb(0x3C8, 0x0E); outb(0x3C6, 0x00);
	outb(0x3C8, 0x10); outb(0x3C6, 0x00);
	outb(0x3C8, 0x11); outb(0x3C6, 0x02);
	outb(0x3C8, 0x14); outb(0x3C6, 0x08);
	outb(0x3C8, 0x15); outb(0x3C6, 0x08);
	outb(0x3C8, 0x16); outb(0x3C6, 0x77);
	outb(0x3C8, 0x20); outb(0x3C6, 0x24);
	outb(0x3C8, 0x21); outb(0x3C6, 0x30);
	outb(0x3C8, 0x22); outb(0x3C6, 0x3D);
	outb(0x3C8, 0x30); outb(0x3C6, 0x00);
	outb(0x3C8, 0x70); outb(0x3C6, 0x01);
	outb(0x3C8, 0x71); outb(0x3C6, 0x43);
	outb(0x3C8, 0x72); outb(0x3C6, 0x00);

	outw(0x3D4, 0x0055);
	outb(0x3C8, 0x00);

	outb(0x3C6, 0xFF);
	outb(0x3C2, 0xEF);
}

static void SetSDACClock( unsigned char *sdac )
{
	int i;
	unsigned char saveCR55;

	outb(vgaCRIndex, 0x55);
	saveCR55 = inb(vgaCRReg);
	outb(vgaCRReg, saveCR55 | 1);

	if(pc98BoardType == PW805I)
	  outb(0x3C7, 0); /* Write MCLK & etc for PW805i. Can't use 0x3C8 */
	else
	  outb(0x3C8, 0); /* Write MCLK & etc */

	for( i=0; i<30; i++){
		outb( 0x3c9, sdac[ i ] );
	}
	return;
}

static void GetSDACClock( void )
{
	int m, n, n1, n2, mclk,i;
	unsigned char saveCR55;

	outb(vgaCRIndex, 0x55);
	saveCR55 = inb(vgaCRReg);
	outb(vgaCRReg, saveCR55 | 1);

	outb(0x3C7, 10); /* read MCLK */
	m = inb(0x3C9);
	n = inb(0x3C9);
	ErrorF( "MCLK is m=%d , n=%d", m, n );
	m &= 0x7f;
	n1 = n & 0x1f;
	n2 = (n>>5) & 0x03;
	mclk = ((1431818 * (m+2)) / (n1+2) / (1 << n2) + 50) / 100;
	ErrorF( "(%1.3f MHz)\n", mclk / 1000.0 );
	ErrorF( "3c6 = %02X \n",inb(0x3C6));
	ErrorF( "SDAC= " );
	outb(0x3C7, 0);
	for(i=0;i<=30;i++){
		ErrorF( "%02X ",inb(0x3C9));
	}
	ErrorF( "\n");
	return;
}

static void SetRegisters( unsigned char *seqreg,unsigned char *grctrl, unsigned char *vgareg)
{
	int i;

	/* Sequence Registers */
	for( i=0; i<=4; i++ ){
		outb( 0x3c4, i );
		outb( 0x3c5, seqreg[ i ] );
		_inb( 0x5f );
		_inb( 0x5f );
	}

	/* Set Graphic Control Data */
	for( i=0; i<=8; i++ ){
		outb( 0x3ce, i );
		outb( 0x3cf, grctrl[ i ] );
		_inb( 0x5f );
		_inb( 0x5f );
	}

	/* Set VGA Registers */
	for( i=0; i<=0x6f; i++ ){
		outb( vgaCRIndex, i );
		outb( vgaCRReg, vgareg[ i ] );
		_outb( 0x5f, 0 );
		_outb( 0x5f, 0 );
	} 
	return;
}

static void GetRegisters( void )
{
	int i, tmp;

	tmp = inb( 0x3cc );
	ErrorF( "[6.1] 3C2(Miscellaneous Output Registes) : 0x%02X\n", tmp );
	tmp = inb( 0x3da );
	ErrorF( "      3CA(Feature Control Registes) : 0x%02X\n", tmp );

	ErrorF( "      3C4/3C5(Sequencer Registers)\n" );
	for( i=0; i<=4; i++ ){
		outb( 0x3c4, i );
		tmp = inb( 0x3c5 );
		ErrorF( "        Index %02X : 0x%02X\n", i, tmp );
	}
	ErrorF( "[6.4] 3CE/3CF(Graphics Controller Registers)\n" );
	for( i=0; i<=8; i++ ){
		outb( 0x3ce, i );
		tmp = inb( 0x3cf );
		ErrorF( "        Index %02X : 0x%02X\n", i, tmp );
	}
/*
	ErrorF( "[6.5] 3C0/3C1(Attribute Controller Registers)\n" );
	for( i=0; i<=0x14; i++ ){
		outb( 0x3c0, i );
		tmp = inb( 0x3c1 );
		ErrorF( "        Index %02X : 0x%02X\n", i, tmp );
	}
*/
	ErrorF( "      3x4/3x5(VGA Registers)\n" );
	for( i=0; i<=0x6f; i++ ){
		outb( vgaCRIndex, i );
		tmp = inb( vgaCRReg );
		ErrorF( "        Index %02X : 0x%02X\n", i, tmp );
	}
	return;
}


static void _pwportconvert()
{
	_port_tbl[0x3c0] = 0x10d0|PW_PORT;
	_port_tbl[0x3c1] = 0x10d1|PW_PORT;
	_port_tbl[0x3c2] = 0x10d2|PW_PORT;
	_port_tbl[0x3c4] = 0x10d4|PW_PORT;
	_port_tbl[0x3c5] = 0x10d5|PW_PORT;
	_port_tbl[0x3c6] = 0x10d6|PW_PORT;
	_port_tbl[0x3c8] = 0x10d8|PW_PORT;
	_port_tbl[0x3cc] = 0x10dc|PW_PORT;
	_port_tbl[0x3ce] = 0x10de|PW_PORT;
	_port_tbl[0x3cf] = 0x10df|PW_PORT;
	_port_tbl[0x3d0] = 0x20d0|PW_PORT;
	_port_tbl[0x3d4] = 0x20d4|PW_PORT;
	_port_tbl[0x3d5] = 0x20d5|PW_PORT;
	_port_tbl[0x3da] = 0x20da|PW_PORT;
	_port_tbl[0x42] = 0x40d2|PW_PORT;
	_port_tbl[0x46] = 0x40d6|PW_PORT;
	_port_tbl[0x4a] = 0x40da|PW_PORT;
	_port_tbl[0x82] = 0x80d2|PW_PORT;
	_port_tbl[0x86] = 0x80d6|PW_PORT;
	_port_tbl[0x8a] = 0x80da|PW_PORT;
	_port_tbl[0x8e] = 0x80de|PW_PORT;
	_port_tbl[0x92] = 0x90d2|PW_PORT;
	_port_tbl[0x96] = 0x90d6|PW_PORT;
	_port_tbl[0x9a] = 0x90da|PW_PORT;
	_port_tbl[0x9e] = 0x90de|PW_PORT;
	_port_tbl[0xa2] = 0xa0d2|PW_PORT;
	_port_tbl[0xa6] = 0xa0d6|PW_PORT;
	_port_tbl[0xaa] = 0xa0da|PW_PORT;
	_port_tbl[0xae] = 0xa0de|PW_PORT;
	_port_tbl[0xb2] = 0xb0d2|PW_PORT;
	_port_tbl[0xb6] = 0xb0d6|PW_PORT;
	_port_tbl[0xba] = 0xb0da|PW_PORT;
	_port_tbl[0xbe] = 0xb0de|PW_PORT;
	_port_tbl[0xe2] = 0xe0d2|PW_PORT;
}

static void _hkbportconvert()
{
	_port_tbl[0x102] = 0xD02;
	_port_tbl[0x3c0] = 0xF40;
	_port_tbl[0x3c1] = 0xF41;
	_port_tbl[0x3c2] = 0xF42;
	_port_tbl[0x3c4] = 0xF44;
	_port_tbl[0x3c5] = 0xF45;
	_port_tbl[0x3c6] = 0xF46;
	_port_tbl[0x3c7] = 0xF47;
	_port_tbl[0x3c8] = 0xF48;
	_port_tbl[0x3c9] = 0xF49;
	_port_tbl[0x3cc] = 0xF4C;
	_port_tbl[0x3ce] = 0xF4E;
	_port_tbl[0x3cf] = 0xF4F;
	_port_tbl[0x3d0] = 0xF50;
	_port_tbl[0x3d4] = 0xF54;
	_port_tbl[0x3d5] = 0xF55;
	_port_tbl[0x3da] = 0xF5A;
	_port_tbl[0x42] = 0x4e68;
	_port_tbl[0x46] = 0x4a68;
	_port_tbl[0x4a] = 0x4c68;
	_port_tbl[0x82] = 0x8e68;
	_port_tbl[0x86] = 0x8a68;
	_port_tbl[0x8a] = 0x8c68;
	_port_tbl[0x8e] = 0x8868;
	_port_tbl[0x92] = 0x9e68;
	_port_tbl[0x96] = 0x9a68;
	_port_tbl[0x9a] = 0x9c68;
	_port_tbl[0x9e] = 0x9868;
	_port_tbl[0xa2] = 0xae68;
	_port_tbl[0xa6] = 0xaa68;
	_port_tbl[0xaa] = 0xac68;
	_port_tbl[0xae] = 0xa868;
	_port_tbl[0xb2] = 0xbe68;
	_port_tbl[0xb6] = 0xba68;
	_port_tbl[0xba] = 0xbc68;
	_port_tbl[0xbe] = 0xb868;
	_port_tbl[0xe2] = 0xee68;
	_port_tbl[0x6e] = 0x6e68;
}



static void _skbportconvert()
{
	_port_tbl[0x102] = 0xD02;
	_port_tbl[0x3c0] = 0xFC0;
	_port_tbl[0x3c1] = 0xFC1;
	_port_tbl[0x3c2] = 0xFC2;
	_port_tbl[0x3c4] = 0xFC4;
	_port_tbl[0x3c5] = 0xFC5;
	_port_tbl[0x3c6] = 0xFC6;
	_port_tbl[0x3c7] = 0xFC7;
	_port_tbl[0x3c8] = 0xFC8;
	_port_tbl[0x3c9] = 0xFC9;
	_port_tbl[0x3cc] = 0xFCC;
	_port_tbl[0x3ce] = 0xFCE;
	_port_tbl[0x3cf] = 0xFCF;
	_port_tbl[0x3d0] = 0xFD0;
	_port_tbl[0x3d4] = 0xFD4;
	_port_tbl[0x3d5] = 0xFD5;
	_port_tbl[0x3da] = 0xFDA;
	_port_tbl[0x42] = 0x4ee8;
	_port_tbl[0x46] = 0x4ae8;
	_port_tbl[0x4a] = 0x46e8;
	_port_tbl[0x82] = 0x8ee8;
	_port_tbl[0x86] = 0x8ae8;
	_port_tbl[0x8a] = 0x86e8;
	_port_tbl[0x8e] = 0x82e8;
	_port_tbl[0x92] = 0x9ee8;
	_port_tbl[0x96] = 0x9ae8;
	_port_tbl[0x9a] = 0x96e8;
	_port_tbl[0x9e] = 0x92e8;
	_port_tbl[0xa2] = 0xaee8;
	_port_tbl[0xa6] = 0xaae8;
	_port_tbl[0xaa] = 0xa6e8;
	_port_tbl[0xae] = 0xa2e8;
	_port_tbl[0xb2] = 0xbee8;
	_port_tbl[0xb6] = 0xbae8;
	_port_tbl[0xba] = 0xb6e8;
	_port_tbl[0xbe] = 0xb2e8;
	_port_tbl[0xe2] = 0xeee8;
	_port_tbl[0x6e] = 0x6ee8;
}

static void _skb4portconvert()
{
	_port_tbl[0x102] = 0xD02;
	_port_tbl[0x3c0] = 0xF40;
	_port_tbl[0x3c1] = 0xF41;
	_port_tbl[0x3c2] = 0xF42;
	_port_tbl[0x3c4] = 0xF44;
	_port_tbl[0x3c5] = 0xF45;
	_port_tbl[0x3c6] = 0xF46;
	_port_tbl[0x3c7] = 0xF47;
	_port_tbl[0x3c8] = 0xF48;
	_port_tbl[0x3c9] = 0xF49;
	_port_tbl[0x3cc] = 0xF4C;
	_port_tbl[0x3ce] = 0xF4E;
	_port_tbl[0x3cf] = 0xF4F;
	_port_tbl[0x3d0] = 0xF50;
	_port_tbl[0x3d4] = 0xF54;
	_port_tbl[0x3d5] = 0xF55;
	_port_tbl[0x3da] = 0xF5A;
	_port_tbl[0x42] = 0x4e68;
	_port_tbl[0x46] = 0x4a68;
	_port_tbl[0x4a] = 0x4c68;
	_port_tbl[0x82] = 0x8e68;
	_port_tbl[0x86] = 0x8a68;
	_port_tbl[0x8a] = 0x8c68;
	_port_tbl[0x8e] = 0x8868;
	_port_tbl[0x92] = 0x9e68;
	_port_tbl[0x96] = 0x9a68;
	_port_tbl[0x9a] = 0x9c68;
	_port_tbl[0x9e] = 0x9868;
	_port_tbl[0xa2] = 0xae68;
	_port_tbl[0xa6] = 0xaa68;
	_port_tbl[0xaa] = 0xac68;
	_port_tbl[0xae] = 0xa868;
	_port_tbl[0xb2] = 0xbe68;
	_port_tbl[0xb6] = 0xba68;
	_port_tbl[0xba] = 0xbc68;
	_port_tbl[0xbe] = 0xb868;
	_port_tbl[0xe2] = 0xee68;
	_port_tbl[0x6e] = 0x6e68;
}

void _necportconvert()
{
	int i;
	for(i=0x3b0; i<0x3e0; i++){
		_port_tbl[i] = i - 0x200;
	}
	_port_tbl[0x102] = 0x002;
	_port_tbl[0x42] =  0x240;
	_port_tbl[0x46] =  0x244;
	_port_tbl[0x4a] =  0x248;
	_port_tbl[0x82] =  0x280;
	_port_tbl[0x86] =  0x284;
	_port_tbl[0x8a] =  0x288;
	_port_tbl[0x8e] =  0x28c;
	_port_tbl[0x92] =  0x290;
	_port_tbl[0x96] =  0x294;
	_port_tbl[0x9a] =  0x298;
	_port_tbl[0x9e] =  0x29c;
	_port_tbl[0xa2] =  0x2a0;
	_port_tbl[0xa6] =  0x2a4;
	_port_tbl[0xaa] =  0x2a8;
	_port_tbl[0xae] =  0x2ac;
	_port_tbl[0xb2] =  0x2b0;
	_port_tbl[0xb6] =  0x2b4;
	_port_tbl[0xba] =  0x2b8;
	_port_tbl[0xbe] =  0x2bc;
	_port_tbl[0xe2] =  0x2e0;
}
