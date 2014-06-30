/* $XFree86: xc/programs/Xserver/hw/xfree98/vga256/drivers/cir_pc98.c,v 3.6 1996/06/29 09:09:54 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"
#include "region.h"

#ifdef XF86VGA16
#define MONOVGA
#endif

#include "cir_driver.h"
#include "cir_alloc.h"
#ifndef MONOVGA
#include "vga256.h"
#endif

#if defined(PC98_GANB_WAP) || defined(PC98_WAB)
static void init_wabs_ganbwap( void );
#ifdef PC98_GANB_WAP
static void enter_ganbwap( void );
#endif
#ifdef PC98_WAB
static void enter_wabs( void );
#endif
#endif

#ifdef PC98_WSNA
static void init_wsna( void );
static void enter_wsna( void );
#endif

#ifdef PC98_WABEP
static void init_wabep( void );
static void enter_wabep( void );
#endif

#ifdef PC98_NKVNEC
static void init_nkvnec( void );
static void enter_nkvnec( void );
static void leave_nkvnec( void );
#endif

void crtswitch( short );

#if defined(PC98_GANB_WAP) || defined(PC98_WAB)
static void
init_wabs_ganbwap(void)
{
    static unsigned char  inidt1[20]={0x00,0x00,0x01,0x02,0x04,0x06,0x07,0x08,
				      0x0b,0x0c,0x0d,0x0e,0x0f,0x16,0x18,0x1b,
				      0x1c,0x1d,0x1e,0x1f};
    static unsigned char  inidt2[28]={0x5f,0x4f,0x50,0x82,0x54,0x80,0x0b,0x3e,
				      0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
				      0xea,0x0c,0xdf,0x50,0x00,0xe1,0x0a,0xe3,
				      0xff,0x32,0x90,0x22};
    static unsigned char  inidt3[10]={0x10,0x41,0x11,0x00,0x12,0x0f,0x13,0x00,
				      0x14,0x00};
    static unsigned short inidt4[13]={0x000b, 0x4005, 0x0000, 0x0001, 0x0002,
				      0x0003, 0x0004, 0x0506, 0x0f07,0x0ff08,
				      0x0010, 0xff11, 0x0431};
    unsigned int tmp;
    /* Init Sync. */
    _outb(0x46e8, 0x18); _outb(0x42e0, 0x01);
    _outb(0x46e8, 0x08); _outb(0x42e0, 0xe3);
    _outb(0x5ae0, 0x00);

    /* ---- step 2 ---- */
    _inb(0x5ae0); _outb(0x46e0, 0xff); _inb(0x5ae0);
    _inb(0x46e0); _inb(0x46e0); _inb(0x46e0); _inb(0x46e0);
    _outb(0x46e0, 0x20); _outb(0x46e0, 0xff);
    /* ---- step 3 ---- */
    for(tmp=0;tmp<20;tmp++)       _outb(0x44e0, inidt1[tmp]);
    /* ---- step 4 ---- */
    for(tmp=0;tmp<28;tmp++)       _outb(0x54e0, inidt2[tmp]);
    /* ---- step 5 ---- */
    _outb(0x54e0, 0x24);
    if(_inb(0x55e0) & 0x80){
	tmp = _inb(0x41e0);
	_outb(0x40e0, tmp);
    }
    for(tmp=0;tmp<10;tmp++)       _outb(0x40e0, inidt3[tmp]);
    _inb(0x5ae0);
    _outb(0x40e0, 0x20);
    _inb(0x5ae0);
    /* ---- step 6 ---- */
    for(tmp=0;tmp<13;tmp++)       outw(0x3ce, inidt4[tmp]);
    return;
}

#ifdef PC98_GANB_WAP
static void
enter_ganbwap(void)
{
    static int wap_initialized = 0;
    unsigned char temp;

    /* allow VRAM mapping above 0xf00000 on EPSON machines */
    if(OFLG_ISSET(OPTION_EPSON_MEM_WIN, &vga256InfoRec.options)) {
	temp = _inb(0x43b);
	_outb(0x43b, temp & 0xfd);
    }
    /* Initialize GA-98NB.  X_MODE_ON -> 8colors mode.*/
    _outb(0x6A,0x00); /* Do 8 colors mode */
    _outb(0x7C,0x00); /* GRCG OFF */
    outb(0x3C4,0x06);
    outb(0x3C5,0x12); /* unlock cirrus special */

    if(OFLG_ISSET(OPTION_WAP, &vga256InfoRec.options)){
	/* WAP */
	_outb(0x40E1,0xFB); /* switch display. normal --> WAP-2000/4000 */
	_outb(0x40E1,0xFA); /* WAP-2000/4000 -> normal */
	_outb(0x40E1,0xFB); /* switch display. normal --> WAP-2000/4000 */
	if (wap_initialized==0){
	    temp  = _inb(0x42E1);
	    /* videoram mapping address to 0xf00000 */
	    _outb(0x42E1,(0x18 | temp));
	    _outb(0x46E8, 0x18); /* Setup */
	    outb(0x3c2,  0x01);  /* test. Is this need? */
	    _outb(0x46E8, 0x08); /* Setup */
	    vgaIOBase = 0x3D0;
	    if (_inb(0x5BE1) & 0x08) {
		temp = 0x3D;	/* VRAM 2M BYTES for WAP-2000 */
	    } else {
		temp = 0xBD;	/* VRAM 4M BYTES for WAP-4000 */
	    }
	    outb(0x3C4,0x0F);
	    outb(0x3C5,temp);
	    outb(0x3C4,0x06);
	    outb(0x3C5,0x12);	 /* unlock cirrus special */
	    wap_initialized = 1;
	}
    } else {
	/* GA-NB 1/2/4 */
	_outb(0x40E1,0xC3); /* switch display. normal --> GA-98NB */
	_outb(0x40E1,0xC2); /* GA-98NB -> normal */
	_outb(0x40E1,0xC3); /* switch display. normal --> GA-98NB */
	_outb(0x46E8, 0x18); /* Setup */
	outb(0x3c2,  0x01);  /* test. Is this need? */
	_outb(0x46E8, 0x08); /* Setup */
	vgaIOBase = 0x3D0;
	outb(0x3C4,0x0F);
	if(OFLG_ISSET(OPTION_GA98NB4, &vga256InfoRec.options)) {
	    outb(0x3C5,0xBD);      /* VRAM 4M BYTES for GA-98NB4*/
	} else if(OFLG_ISSET(OPTION_GA98NB2, &vga256InfoRec.options)) {
	    outb(0x3C5,0x3D);      /* VRAM 2M BYTES for GA-98NB2*/
	} else if(OFLG_ISSET(OPTION_GA98NB1, &vga256InfoRec.options)) {
	    outb(0x3C5,0x35);      /* VRAM 1M BYTES for GA-98NB1*/
	}
	outb(0x3C4,0x06);
	outb(0x3C5,0x12);	 /* unlock cirrus special */
    }
    return;
}
#endif /* PC98_GANB_WAP */

#ifdef PC98_WAB
static void
enter_wabs(void)
{
    unsigned char temp;

    /* Initialize WAB-S.  X_MODE_ON -> 8colors mode.*/
    _outb(0x6A,0x00); /* Do 8 colors mode */
    _outb(0x7C,0x00); /* GRCG OFF */

    outb(0x3C4,0x06);
    outb(0x3C5,0x12);	 /* unlock cirrus special */

    _outb(0x40E1,0xFB); /* switch display. normal --> WAB */
    _outb(0x40E1,0xFA); /* WAB -> normal */
    _outb(0x40E1,0xFB); /* switch display. normal --> WAB */

    _outb(0x46E8, 0x18); /* Setup */
    outb(0x3c2,  0x01); /* test. Is this need? */
    _outb(0x46E8, 0x08); /* Setup */

    outb(0x3C4,0x0F);
    temp = inb(0x3C5);
    temp = 0x17;
    outb(0x3C4,0x0F);
    outb(0x3C5,temp);
    outb(0x3C4,0x0F);
    temp = inb(0x3C5);
    return;
}
#endif /*PC98_WAB */
#endif /* PC98_GANB_WAP || PC98_WAB */

#ifdef PC98_WSNA
static void
init_wsna(void)
{
    static union {
	struct { short x; } w;
	struct { char l, h; } b;
    } data;

    unsigned short initdt1[] = {
	0x0107, /* Extended Sequencer Mode */
	0x0008, /* EEPROM Control */
	0x0009, /* Scratch Pad 0 */
	0x000a, /* Scratch Pad 1 */
	0x660b, /* VCLK0 Numerator */
	0x510c, /* VCLK1 Numerator */
	0x6e0d, /* VCLK2 Numerator */
	0x550e, /* VCLK3 Numerator */
	0xb40f, /* DRAM Control */
	0xf016, /* Performance Tuning */
	0x0218, /* Signature Generator Control */
	0x0119, /* Signature Generator Result Low-Byte */
	0x3b1b, /* VCLK Denominator and Post-Scalar */
	0x3a1c, /* VCLK Denominator and Post-Scalar */
	0x2a1d, /* VCLK Denominator and Post-Scalar */
	0x361e, /* VCLK Denominator and Post-Scalar */
	0x201f, /* MCLK Select */
	0xffff
    };

    unsigned short initdt2[] = {
	0x0101, 0x0f02, 0x0003, 0x0e04, 0xffff
    };

    unsigned char initdt3[] = {
	0xa1, 0x7f, 0x80, 0x85, 0x85, 0x96, 0x24, 0xfd,
	0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x08, 0xff, 0x80, 0x00, 0x00, 0x23, 0xe3,
	0xff, 0x32, 0xe0, 0x32
    };

    unsigned char initdt4[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x41, 0x00, 0x0f, 0x00, 0x00
    };

    unsigned char initdt5[] = {
	0x00, 0xff, 0x0f, 0x00, 0x00, 0x40, 0x05, 0x0f,
	0xff, 0x1c, 0x00, 0x24, 0x00
    };

    unsigned int tmp;

    if(_inb(0x51e3) == 0xc2){
	tmp = _inb(0x57e3);
	_outb(0x57e3, tmp & 0x7f);
    } else {
	_outb(0x46e8, 0x18); outb(0x3c2, 0x01);
	_outb(0x46e8, 0x08);
    }

    outb(0x3c2, 0xe3); outb(0x3da, 0x00);
    _outb(0x42e3, 0x18);
    inb(0x3c6); inb(0x3c6);
    inb(0x3c6); inb(0x3c6);
    outb(0x3c6, 0x20); outw(0x3c4, 0x1206);
    outw(0x3c4, 0x0200); outw(0x3c4, 0x0300);

    for(tmp = 0; initdt1[tmp] != 0xffff; tmp++){
	outw(0x3c4, initdt1[tmp]);
    }

    inb(0x3cc); inb(0x3da);
    outb(0x3c0, 0x00);

    for(tmp = 0; initdt2[tmp] != 0xffff; tmp++){
	outw(0x3c4, initdt2[tmp]);
    }

    outb(0x3c2, 0xeb); inb(0x3cc);
    data.b.l = 0x11; outb(0x3d4, data.b.l);
    data.b.h = inb(0x3d5) & 0x7f;
    outw(0x3d4, data.w.x);
    outw(0x3d4, 0x001c);

    for(data.b.l = 0; data.b.l < 0x1c; data.b.l++){
	data.b.h = initdt3[data.b.l];
	outw(0x3d4, data.w.x);
    }

    outb(0x3d4, 0x24); data.b.l = _inb(0x3d5);

    if((unsigned char)data.b.l != 0x80){
	tmp = inb(0x3c1);
	outb(0x3c0, tmp);
    }

    for(data.b.l = 0; data.b.l < 0x15; data.b.l++){
	outb(0x3c0, data.b.l);
	outb(0x3c0, initdt4[data.b.l]);
    }

    for(data.b.l = 0; data.b.l < 0x0d; data.b.l++){
	data.b.h = initdt5[data.b.l];
	outw(0x3ce, data.w.x);
    }
    return;
}

static void
enter_wsna(void)
{
    static int wsn_initialized;
    unsigned char temp;

    /* allow VRAM mapping above 0xf00000 on EPSON machines */
    if(OFLG_ISSET(OPTION_EPSON_MEM_WIN, &vga256InfoRec.options)){
	temp = _inb(0x43b);
	_outb(0x43b, temp & 0xfd);
    }

    /* Initialize WSN-A2F/A4F. X_MODE_ON -> 8colors mode. */
    _outb(0x6A,0x00); /* Do 8 colors mode */
    _outb(0x7C,0x00); /* GRCG OFF */
    outb(0x3c4,0x06);
    outb(0x3c5,0x12); /* unlock cirrus special */

    if(wsn_initialized == 0){
	_outb(0x43e3, 0x18);
	_outb(0x6a, 0x00);
	vgaIOBase = 0x3D0;
                                
	outb(0x3d4, 0x24);
	temp = inb(0x3d5);
	if(temp == 0x80){
	    temp = inb(0x3c1);
	    outb(0x3c0, temp);
	}

	outb(0x3c0, 0x00);
	_outb(0x40e3, 0x7a);  /* switch display. WSN-A2F --> normal */
	temp = inb(0x3cc) | 0x02;
	outb(0x3c2, temp);
	_outb(0x40e3, 0x7b);  /* switch display. normal --> WSN-A2F */

	outb(0x3c0, 0x20);

	temp = _inb(0x5BE3) & 0x08;
	if (!temp){
	    temp = 0xfd; /* VRAM 4M BYTES for WSN-A4F */
	} else {
	    temp = 0x7d; /* VRAM 2M BYTES for WSN-A2F */
	}
	outb(0x3c4,0x0f);
	outb(0x3c5,temp);

	outb(0x3c4,0x06);
	outb(0x3c5,0x12);        /* unlock cirrus special */
	wsn_initialized = 1;
    }
    return;
}
#endif

#ifdef PC98_WABEP
static void
init_wabep(void)
{
    static unsigned short inidt1[20]={0x0200,0x0300,0x0101,0x0f02,0x0e04,
				      0x1206,0x0107,0x0008,0x6e0b,0x4e0c,
				      0x750d,0x550e,0x540f,0xf016,0x0218,
				      0x2a1b,0x3a1c,0x341d,0x361e,0x251f};
    static unsigned short inidt2[28]={0x5f01,0x4f02,0x5003,0x8204,0x5405,
				      0x8006,0x0b07,0x3e08,0x0009,0x400a,
				      0x000b,0x000c,0x000d,0x000e,0x000f,
				      0x0010,0xea11,0x0c12,0xdf13,0x5014,
				      0x0015,0xe116,0x0a17,0xe318,0xff19,
				      0x321a,0x901b,0x221c};
    static unsigned char  inidt3[10]={0x10,0x41,0x11,0x00,0x12,0x0f,0x13,
				      0x00,0x14,0x00};
    static unsigned short inidt4[13]={0x000b,0x4005,0x0000,0x0001,0x0002,
				      0x0003,0x0004,0x0506,0x0f07,0xff08,
				      0x0010,0xff11,0x0431};
    unsigned int tmp;
    /* Init Sync.(sub_20) */
    _outb(0x46e8, 0x18); _outb(0x0d02, 0x01);
    _outb(0x46e8, 0x08); _outb(0x0f42, 0xe3);
    _outb(0x0f5a, 0x00);
    _outw(0x0f44, 0x1206);
    _outw(0x0f44, 0x3017);
    _outw(0x0f54, 0x0011);
    _outb(0x0f54, 0x27);
    _inb(0x0f55);

    /* ---- step 2 ---- (sub_23) */
    _outb(0x0f46, 0xff); 
    _inb(0x0f46); _inb(0x0f46); _inb(0x0f46); _inb(0x0f46);
    _outb(0x0f46, 0x20); 
    /* ---- step 3 ---- (sub_24) */
    for(tmp=0;tmp<20;tmp++)       _outw(0x0f44, inidt1[tmp]);
    /* ---- step 4 ---- (sub_26) */
    for(tmp=0;tmp<28;tmp++)       _outw(0x0f54, inidt2[tmp]);
    /* ---- step 5 ---- (sub_27) */
    _outb(0x0f54, 0x24);
    if((_inb(0x0f55) & 0x80)==0x80){
	tmp = _inb(0x0f41);
        _outb(0x0f40, tmp); 
    }
    for(tmp=0;tmp<10;tmp++)       _outb(0x0f40, inidt3[tmp]);
    /* ---- step 6 ---- */
    for(tmp=0;tmp<13;tmp++)       outw(0x3ce, inidt4[tmp]);
    /* ---- step 7 ---- (sub_28)*/
    _inb(0x0f5a);
    _outb(0x0f40, 0x20);
    _inb(0x0f5a);
    _inb(0x0f46);_inb(0x0f46);_inb(0x0f46);_inb(0x0f46);
    _outb(0x0f46,0x20);
    _outb(0x0f46,0xff);
    /* ---- step 8 ---- (sub_29)*/
    _outb(0x0f54,0x24);
    if((_inb(0x0f55) & 0x80)==0x80){
	tmp = _inb(0x0f41);
        _outb(0x0f40, tmp );
    }

    return;
}

static void
enter_wabep(void)
{
    unsigned char temp;

    /* Initialize WAB-S.  X_MODE_ON -> 8colors mode.*/
    _outb(0x6A,0x00); /* Do 8 colors mode */
    _outb(0x7C,0x00); /* GRCG OFF */

    outb(0x3C4,0x06);
    outb(0x3C5,0x12);	 /* unlock cirrus special */

    _outb(0xf5c,0xFB); /* switch display. normal --> WAB */
    _outb(0xf5d,0xFA); /* WAB -> normal */
    _outb(0xf5c,0xFB); /* switch display. normal --> WAB */

    outb(0x3C4,0x0F);
    temp = inb(0x3C5);
    temp = 0x17;
    outb(0x3C4,0x0F);
    outb(0x3C5,temp);

    outb(0x3C4,0x0F);
    temp = inb(0x3C5);

    return;
}
#endif /* PC98_WAB */

#ifdef PC98_NKVNEC
static void
init_nkvnec(void)
{
	int i;
	static int rst_SR[] = {0x1206, 0x0200, 0x0300, 0x2101, 0x0012};
	static int set_SR[] = {0x0300, 0x0101, 0x0F02, 0x0E04, 
				0x1206, 0x0107, 0x0008, 0x300F, 
				0x0012, 0xD316, 0x0018, 0x1c1F,
				0x660B, 0x3B1B, 0x480C, 0x231C, 
				0x560D, 0x3D1D, 0x5B0E, 0x3F1E};
	static int set_CR[] = {0x6500, 0x4F01, 0x5002, 0x8A03, 
				0x5904, 0x8105, 0xB606, 0x1F07,
				0x0008, 0x4009, 0x000A, 0x000B,
				0x000C, 0x000D, 0x800E, 0x200F,
				0x9610, 0x8E11, 0x8F12, 0x8013,
				0x0014, 0x8F15, 0x9516, 0xE317,
				0xFF18, 0x3219, 0x501A, 0x221B};
	static int set_GR[] = {0x0000,0x0001,0x0002,0x0003,0x0004,
			       0x4005,0x0506,0x0F07,0xFF08};
	static char set_AR[]= {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
			       0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			       0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00,
			       0x0F, 0x00, 0x00};
	static int set_GR2[] = {0x0009, 0x000a, 0x000b};
	for(i=0; i<5; i++)
		outw(0x3c4, rst_SR[i]);
	for(i=0; i<20; i++)
		outw(0x3c4, set_SR[i]);
	outw(0x3c4, 0x340f);
	outb(0x3c2, 0xEF);
	outw(0x3ce, 0x0506);
	outw(0x3c4, 0x0300);
	outw(0x3d4, 0x2011);
	for(i=0;i<28;i++)
		outw(0x3d4, set_CR[i]);
	outw(0x3d4,0x001d);
	outw(0x3c4,0x0117);
	for(i=0;i<9;i++)
		outw(0x3ce,set_GR[i]);
	inb(0x3da);
	for(i=0; i<21; i++){
		outb(0x3c0, i);
		outb(0x3c0, set_AR[i]);
	}
	outb(0x3c0, 0x20);
	outb(0x3c6, 0xff);
	for(i=0; i<3; i++)
		outw(0x3ce, set_GR2[i]);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6);
}

static void
enter_nkvnec(void)
{
	_outb(0x68,0x0e);
	if (OFLG_ISSET(OPTION_NEC_CIRRUS, &vga256InfoRec.options)){
	    _outb(0x7c,0x80);
	}
	_outb(0x6a,0x07);
	_outb(0x6a,0x8f);
	_outb(0x6a,0x06);
	_outb(0x5f, 0);
	_outw(0xc40, 0x0061);
	_outw(0xc44, 0xf0);	/* VRAM mapping at 0xf0(0000) */
	_outw(0xc42, 0xf01c);
	_outb(0xfaa, 0x03);
	_outb(0xfab, 0x03);
	_outb(0x5f, 0);
	while(_inw(0x0c42) & 0x0400) ;
	_outb(0x904 , 0x01);
	_outb(0x102, 0x01);
	outb(0x3c2, 0x01);
	_outb(0x904 , 0x20);
	outb(0x3c2, 0xe1);
	outb(0x3da, 0x00);
}

static void
leave_nkvnec(void)
{
	_outb(0x68, 0x0e);
	outb(0x3c0,0x1f);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6);
	inb(0x3c6);
	outb(0x3c6,0x80);
	_outb(0xfaa,0x03);
	_outb(0xfab,0x00);
	_outb(0x43f,0x42);
	_outw(0xc44,0xffff);
	_outw(0xc42,0x0000);
	_outb(0x43f,0x40);
	_outb(0x5f, 0);
	_outb(0x6a,0x07);
	_outb(0x6a,0x8e);
	_outb(0x6a,0x06);
	if (OFLG_ISSET(OPTION_NEC_CIRRUS, &vga256InfoRec.options)){
	    _outb(0x7c,0x00);
	}
	_outb(0x5f, 0);
	_outb(0x68,0x0f);
	_outb(0x62, 0x6b);
	_outb(0xa2, 0x0c);
}
#endif /* PC98_NKVNEC */

void
#ifdef NeedFunctionPrototypes
crtswitch(short swtch)
#else
crtswitch(swtch)
short swtch;
#endif
{
    static int init = 0;

    if( swtch == 1){
	if( init == 0 ){
	    /* Initialize board */
#if defined(PC98_GANB_WAP) || defined(PC98_WAB)
		init_wabs_ganbwap();
#endif
#ifdef PC98_WSNA
		init_wsna();
#endif
#ifdef PC98_WABEP
		init_wabep();
#endif
#ifdef PC98_NKVNEC
	 	enter_nkvnec();
		init_nkvnec();
#endif
	    init = 1;
	}

	vgaIOBase = 0x3d0;

	/* switch normal -> X */
#ifdef PC98_GANB_WAP
	enter_ganbwap();
#endif
#ifdef PC98_WSNA
	enter_wsna();
#endif
#ifdef PC98_NKVNEC
	enter_nkvnec();
	outw(0x3c4, 0x1206); /* unlock cirrus special */
#endif
#ifdef PC98_WAB
	enter_wabs();
#endif
#ifdef PC98_WABEP
	enter_wabep();
#endif
    } else {
	/* switch X -> normal */
#ifdef PC98_GANB_WAP
	if(OFLG_ISSET(OPTION_GA98NB1, &vga256InfoRec.options) ||
	   OFLG_ISSET(OPTION_GA98NB2, &vga256InfoRec.options) ||
	   OFLG_ISSET(OPTION_GA98NB4, &vga256InfoRec.options)){
	    _outb(0x40E1,0xC2);
	}
	if(OFLG_ISSET(OPTION_WAP, &vga256InfoRec.options)){
	    _outb(0x40E1,0xFA); /* WAP-2000/4000 -> normal */
	}
#endif
#ifdef PC98_WSNA
	_outb(0x40E3,0xFA); /* WSN-A2F -> normal */
#endif
#ifdef PC98_NKVNEC
	leave_nkvnec();
#endif
#ifdef PC98_WAB
	_outb(0x40E1,0xFA);
#endif
#ifdef PC98_WABEP
	_outb(0xf5d,0xFA);
#endif
    }
    return;
}
