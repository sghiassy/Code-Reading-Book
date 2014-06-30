/* $XConsortium: ngledevrom.h,v 1.1 93/08/08 12:57:21 rws Exp $ */

/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.
 *
 *************************************************************************/

/******************************************************************************
 *
 *      This header file contains the structure definition
 *      of this device-dependent ROM area, after values have
 *      been packed (actual ROM is only 1-byte wide, so each
 *      long only contains a byte of data).
 *
 ******************************************************************************/


#ifndef DEVDEPROM_H
#define DEVDEPROM_H

/* On HP-UX ioctl(fildes, GCDESCRIBE returns pointer to device-dependent
 * ROM area in crt_region[1]
 */
#define NGLEDEVDEPROM_CRT_REGION	1


typedef char     rom_byte_t;
typedef short    rom_short_t;
typedef long     rom_long_t;
typedef void     *rom_addr_t;

typedef struct {
    rom_long_t      video_config_reg; 
    rom_long_t      misc_video_start;
    rom_long_t      horiz_timing_fmt; 
    rom_long_t      serr_timing_fmt; 
    rom_long_t      vert_timing_fmt; 
    rom_long_t      horiz_state;
    rom_long_t      vert_state;
    rom_long_t      vtg_state_elements;
    rom_long_t      pipeline_delay;    
    rom_long_t      misc_video_end;    
    } video_setup_t;

typedef struct {                  
    rom_short_t     sizeof_ngle_data;
    rom_short_t     x_size_visible;	    /* visible screen dim in pixels  */
    rom_short_t     y_size_visible;

    rom_short_t     pad2[15];

    rom_short_t     cursor_pipeline_delay;
    rom_short_t     video_interleaves;

    rom_long_t      pad3[11];

    } ngle_rom_t;              

typedef ngle_rom_t  NgleDevRomDataRec;
typedef ngle_rom_t  *NgleDevRomDataPtr;
#endif /* DEVDEPROM_H */
