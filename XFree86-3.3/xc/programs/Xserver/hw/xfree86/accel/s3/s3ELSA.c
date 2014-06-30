/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3ELSA.c,v 3.20.2.1 1997/05/06 13:25:40 dawes Exp $ */
/* 
 * s3ELSA.c 
 * 
 * compile standalone program with   
 *
      rm -f s3ELSA.o
      make DEFINES=-DELSA_MAIN "LDLIBS=../../os-support/libxf86_os.a" s3ELSA 
      rm -f s3ELSA.o
 * 
 * 
 */
/* $XConsortium: s3ELSA.c /main/10 1996/10/28 04:55:10 kaleb $ */

#ifdef linux
#include <stdlib.h>
#endif

#include "s3.h"
#include "s3ELSA.h"
#include "xf86_OSlib.h"

#ifdef ELSA_MAIN
#include <stdio.h>
#ifdef linux
#include <unistd.h>
#endif /* linux */
#ifndef SVR4
#include <getopt.h>
#endif /* SVR4 */
#ifdef xalloc
#undef xalloc
#endif
#define xalloc(_p) malloc(_p)
#ifdef xrealloc
#undef xrealloc
#endif
#define xrealloc(_o,_s) realloc(_o,_s)
#ifdef xfree
#undef xfree
#endif
#define xfree(_p) free(_p)
#endif /* ELSA_MAIN */


#define BIOS_BSIZE 512
#define BIOS_BASE  0xc0000


elsa_board_types_t elsa_board_types[] = {
   { ELSA_WINNER_1000,		"ELSA Winner 1000",		0 },
   { ELSA_WINNER_1000VL,	"ELSA Winner 1000VL",		0 },
   { ELSA_WINNER_1000PCI,	"ELSA Winner 1000PCI",		0 },
   { ELSA_WINNER_1000ISA,	"ELSA Winner 1000ISA",		0 },
   { ELSA_WINNER_2000,		"ELSA Winner 2000",		0 },
   { ELSA_WINNER_2000VL,	"ELSA Winner 2000VL",		0 },
   { ELSA_WINNER_2000PCI,	"ELSA Winner 2000PCI",		0 },
   { ELSA_WINNER_1000PRO,	"ELSA Winner 1000PRO",		0 },
   { ELSA_WINNER_1000PRO_TRIO32,"ELSA Winner 1000PRO Trio32",	0 },
   { ELSA_WINNER_1000PRO_TRIO64,"ELSA Winner 1000PRO Trio64",	0 },
   { ELSA_WINNER_1000AVI,	"ELSA Winner 1000AVI",		0 },
   { ELSA_WINNER_1000PRO_X,	"ELSA Winner 1000PRO/X",	0 },
   { ELSA_WINNER_2000PRO,	"ELSA Winner 2000PRO",		0 },
   { ELSA_WINNER_2000PRO_X,	"ELSA Winner 2000PRO/X",	0 },
   { ELSA_WINNER_2000AVI,	"ELSA Winner 2000AVI",		0 },
   { ELSA_WINNER_2000PRO_X8,	"ELSA Winner 2000PRO/X-8",	0 },
   { ELSA_GLORIA_4,		"ELSA Gloria-4",		0 },
   { ELSA_GLORIA_8,		"ELSA Gloria-8",		0 },
   { 0, 0, 0 }
};


static __inline__ void shift_out(int b) {
   int i,j; 
   outb(0x3d4,0x5c); i = (inb(0x3d5) & 0xaf) | ((b)?0x10:0x00); 
   j = i;  
   outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); 
   j = i | 0x40;  
   outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); 
} 

static __inline__ int shift_in()
{ 
   int i,j; 
   outb(0x3d4,0x5c); i = inb(0x3d5) & 0xaf; 
   j = i;  
   outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); 
   j = i | 0x40;  
   outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); outb(0x3d5,j); 
   return (inb(0x3c2) & 0x10) != 0;
} 

static int read_eeprom_byte(int r)
{
   int i,j;
   
   outb(0x3d4,0x5c); i = inb(0x3d5) |  0x20; outb(0x3d5,i);
   outb(0x3d5,i); outb(0x3d5,i); outb(0x3d5,i);

   shift_out(1);
   shift_out(1);
   shift_out(0);
   
   for(i=5; i>=0; i--)
      shift_out((r>>i) & 1);
   
#if 0
   shift_out(0);
#endif

   j=0;
   for(i=0; i<16; i++) 
      j = (j<<1) | shift_in();

   outb(0x3d4,0x5c); i = inb(0x3d5) & ~0x20; outb(0x3d5,i);
   outb(0x3d5,i); outb(0x3d5,i); outb(0x3d5,i);

   return j;
}


static int read_eeprom_data(unsigned short **pdata)
{
   int i;
   int sr01, cr45, cr50, cr55, cr5c;
   int ndata=6;
   unsigned short *data;
   elsa_eeprom_data_t *eedata;

#ifdef ELSA_MAIN
#ifdef linux
   iopl(3);
#endif
#endif

   outb(0x3d4,0x38); outb(0x3d5,0x48);
   outb(0x3d4,0x39); outb(0x3d5,0xa5);

   outb(0x3c4,0x01); sr01 = inb(0x3c5);
   outb(0x3d4,0x45); cr45 = inb(0x3d5);
   outb(0x3d4,0x50); cr50 = inb(0x3d5);
   outb(0x3d4,0x55); cr55 = inb(0x3d5);
   outb(0x3d4,0x5c); cr5c = inb(0x3d5);


   outb(0x3c4,0x01); i = inb(0x3c5) |  0x20; outb(0x3c5,i);
   outb(0x3d4,0x45); i = inb(0x3d5) & ~0x20; outb(0x3d5,i);
   outb(0x3d4,0x50); i = inb(0x3d5) & ~0x04; outb(0x3d5,i);
   outb(0x3d4,0x55); i = inb(0x3d5) & ~0x20; outb(0x3d5,i);
   outb(0x3d4,0x5c); i = inb(0x3d5) |  0x80; outb(0x3d5,i);
   outb(0x3d5,i); outb(0x3d5,i); outb(0x3d5,i);

   for(i=0; i<64; i++)
      shift_out(0);

   data = (unsigned short*) xalloc(ndata*sizeof(unsigned short));

   for (i=0; i<ndata; i++)
      data[i] = read_eeprom_byte(i);

   eedata = (elsa_eeprom_data_t *) data;

   if (eedata->wnr_type == ('S' | '3'<<8)) {
      if (eedata->eeprom_size > ndata) {
	 ndata = eedata->eeprom_size;
	 data = (unsigned short*) xrealloc(data, ndata*sizeof(unsigned short));
	 for (; i<ndata; i++)
	    data[i] = read_eeprom_byte(i);
      }
   }

   outb(0x3d4,0x5c); outb(0x3d5,cr5c);
   outb(0x3d4,0x55); outb(0x3d5,cr55);
   outb(0x3d4,0x50); outb(0x3d5,cr50);
   outb(0x3d4,0x45); outb(0x3d5,cr45);
   outb(0x3c4,0x01); outb(0x3c5,sr01);

#if 0
   outb(0x3d4,0x39); outb(0x3d5,0x00);
   outb(0x3d4,0x38); outb(0x3d5,0x00);
#endif

#ifdef ELSA_MAIN
#ifdef linux
   iopl(0);
#endif
#endif

   eedata = (elsa_eeprom_data_t *) data;
   if (eedata->wnr_type == ('S' | '3'<<8)) {
      *pdata = data;
   }
   else {
      xfree(data);
      ndata = -1;
   }
   return ndata;
}

static int calc_crc16(int ndata, unsigned short *data)
{
   int i,j,s,crc16;
   unsigned short d;
   
   crc16 = 0;
   for(i=1; i<ndata; i++) {
      d = data[i];
      for (j=0; j<16; j++) {
	 s = (crc16>>1) + (crc16>>14) + (crc16>>15) + d + 1;
	 crc16 = (crc16<<1) | (s&1);
	 d >>= 1;
      }
   }
   return crc16;
}

static int check_ELSA_bios(int BIOSbase)
{
   unsigned char bios[BIOS_BSIZE];
   char *match = " ELSA GmbH";
   int i,l;
   
   if (xf86ReadBIOS(BIOSbase, 0, bios, BIOS_BSIZE) != BIOS_BSIZE)
      return -1;

   if ((bios[0] != 0x55) || (bios[1] != 0xaa))
      return -2;

   l = strlen(match);
   for (i=0; i<BIOS_BSIZE-l; i++) 
      if (bios[i] == match[0] && !memcmp(&bios[i],match,l))
	 return 1;
   return 0;
}


#ifdef ELSA_MAIN

void ErrorF(char *s, ...)
{
}

void main()
{
   int i;
   int ndata;
   unsigned short *data;
   unsigned short crc16;
   elsa_eeprom_data_t *eedata;
   elsa_eeprom_timing_t *eetim;
   unsigned long serno;

   if (check_ELSA_bios(BIOS_BASE) <= 0) {
      printf("no ELSA Bios detected\n");
      exit(1);
   }

   ndata = read_eeprom_data(&data);
   if (ndata<0) {
      printf("no ELSA Winner card detected\n");
      exit(1);
   }

   crc16 = calc_crc16(ndata,data);
   eedata = (elsa_eeprom_data_t *) data;


   printf("EEPROM size %d bytes\n",ndata);
   if (eedata->crc16 == crc16)
      printf("CRC ok (%04x)\n",crc16);
   else
      printf("CRC not ok (%04x != %04x)\n",eedata->crc16,crc16);
   printf("Chip type %s\n"
	  ,(eedata->wnr_type == 'S' | '3'<<8) ? "S3" : "unknown");
   for (i=0; elsa_board_types[i].code; i++)
      if (elsa_board_types[i].code == eedata->board_code) break;
   if (elsa_board_types[i].code)
      printf("%s\n",elsa_board_types[i].name);
   else 
      printf("unknown ELSA board code %x, please report\n", eedata->board_code);

   serno = (eedata->serno_h<<16) | eedata->serno_l;
   printf("Ser.No. %c-%04ld.%03ld.%03ld\n",
	  (char)('A' + ((serno>>27) & 0x0f)),
	  ((serno>>17) & 0x3ff) | ((serno>>21) & 0x400),
	  (serno & 0x1ffff) / 1000,
	  (serno & 0x1ffff) % 1000);
   printf("max pixel clock  %7.3f MHz\n"
	  ,(eedata->max_pixclock,eedata->max_pixclock*4)/1000.0);
   printf("max memory clock %7.3f MHz\n"
	  ,(eedata->max_memclock,eedata->max_memclock*4)/1000.0);
   printf("monitor size %d %d\n",eedata->xsize,eedata->ysize);
   printf("software version %d\n",eedata->sw_vers);
   printf("hardware config %d\n",eedata->hw_conf);

   for (i= 26; i<ndata-9; i+=9) {
      eetim  = (elsa_eeprom_timing_t *) (data + i);
#if 0
      printf("\ntiming  %d:\n",i);
      printf("\tbpp     %d\n",ELSA_TIM_bpp(*eetim));
      printf("\txres    %d\n",ELSA_TIM_xres(*eetim));
      printf("\tyres    %d\n",ELSA_TIM_yres(*eetim));
      printf("\tpixfrq  %1.3f MHz\n",(ELSA_TIM_pixfrq4(*eetim)*4)/1000.0);

      printf("\tflags   %d\n",ELSA_TIM_flags(*eetim));
      printf("\thtot    %d\n",ELSA_TIM_htot(*eetim));
      printf("\thfp     %d\n",ELSA_TIM_hfp(*eetim));
      printf("\thsw     %d\n",ELSA_TIM_hsw(*eetim));
      printf("\tvtot    %d\n",ELSA_TIM_vtot(*eetim));
      printf("\tvfp     %d\n",ELSA_TIM_vfp(*eetim));
      printf("\tvsw     %d\n",ELSA_TIM_vsw(*eetim));
#endif
      if (ELSA_ET_VM_VALID(eetim))
	 printf("\"%dx%dx%d\" \t %7.3f   %4d %4d %4d %4d   %4d %4d %4d %4d\n"
	     ,ELSA_TIM_xres(*eetim),ELSA_TIM_yres(*eetim),ELSA_TIM_bpp(*eetim)
	     ,(ELSA_TIM_pixfrq4(*eetim)*4)/1000.0
	     ,ELSA_TIM_xres(*eetim)
	     ,ELSA_TIM_xres(*eetim)+ELSA_TIM_hfp(*eetim)
	     ,ELSA_TIM_xres(*eetim)+ELSA_TIM_hfp(*eetim)+ELSA_TIM_hsw(*eetim)
	     ,ELSA_TIM_htot(*eetim)
	     ,ELSA_TIM_yres(*eetim)
	     ,ELSA_TIM_yres(*eetim)+ELSA_TIM_vfp(*eetim)
	     ,ELSA_TIM_yres(*eetim)+ELSA_TIM_vfp(*eetim)+ELSA_TIM_vsw(*eetim)
	     ,ELSA_TIM_vtot(*eetim)
	     );
   }   
   xfree(data);
}

#else
int s3DetectELSA(int BIOSbase, char **pcard, char **pserno, 
		 int *max_pix_clock, int *max_mem_clock, int *hwconfig,
		 char **modes)
{
   int i;
   int ndata;
   unsigned short *data;
   unsigned short crc16;
   elsa_eeprom_data_t *eedata;
   elsa_eeprom_timing_t *eetim;
   unsigned long serno;

   if (check_ELSA_bios(BIOSbase>0 ? BIOSbase : BIOS_BASE) <= 0) {
      return -1;
   }

   ndata = read_eeprom_data(&data);
   if (ndata<0) 
      return -2;

   crc16 = calc_crc16(ndata,data);
   eedata = (elsa_eeprom_data_t *) data;

   if (eedata->crc16 != crc16) {
      xfree(data);
      return -3;
   }

   if (eedata->wnr_type != ('S' | '3'<<8)) {
      xfree(data);
      return -4;
   }

   for (i=0; elsa_board_types[i].code; i++)
      if (elsa_board_types[i].code == eedata->board_code) break;

   if (pcard) {
      *pcard  = (char*) xalloc(80);
      if (elsa_board_types[i].code)
	 sprintf(*pcard,"%s detected",elsa_board_types[i].name);
      else 
	 sprintf(*pcard,"unknown ELSA Winner board code %04x detected, please report\n"
		 , eedata->board_code);
   }

   if (modes) {
      char *p;
      p = *modes  = (char*) xalloc(80 * ((ndata-9-26)/9 +1));
      *p = '\0';
      for (i= 26; i<ndata-9; i+=9) {
	 eetim  = (elsa_eeprom_timing_t *) (data + i);
	 if (ELSA_ET_VM_VALID(eetim))
	    sprintf(p,"\t\"%dx%dx%d\" \t %7.3f   %4d %4d %4d %4d   %4d %4d %4d %4d\n"
		    ,ELSA_TIM_xres(*eetim),ELSA_TIM_yres(*eetim),ELSA_TIM_bpp(*eetim)
		    ,(ELSA_TIM_pixfrq4(*eetim)*4)/1000.0
		    ,ELSA_TIM_xres(*eetim)
		    ,ELSA_TIM_xres(*eetim)+ELSA_TIM_hfp(*eetim)
		    ,ELSA_TIM_xres(*eetim)+ELSA_TIM_hfp(*eetim)+ELSA_TIM_hsw(*eetim)
		    ,ELSA_TIM_htot(*eetim)
		    ,ELSA_TIM_yres(*eetim)
		    ,ELSA_TIM_yres(*eetim)+ELSA_TIM_vfp(*eetim)
		    ,ELSA_TIM_yres(*eetim)+ELSA_TIM_vfp(*eetim)+ELSA_TIM_vsw(*eetim)
		    ,ELSA_TIM_vtot(*eetim)
		    );
	 p += strlen(p);
      }   
   }

   if (pserno) {
      *pserno = (char*) xalloc(20);
      serno = (eedata->serno_h<<16) | eedata->serno_l;
      sprintf(*pserno,"%c-%04ld.%03ld.%03ld",
	      (char)('A' + ((serno>>27) & 0x0f)),
	      ((serno>>17) & 0x3ff) | ((serno>>21) & 0x400),
	      (serno & 0x1ffff) / 1000,
	      (serno & 0x1ffff) % 1000);
   }

   if (max_pix_clock) 
      *max_pix_clock = eedata->max_pixclock * 4;
   if (max_mem_clock) 
      *max_mem_clock = eedata->max_memclock * 4;
   if (hwconfig)
      *hwconfig = eedata->hw_conf;
   
   i = eedata->board_code;
   xfree(data);
   return i;
}

#endif
