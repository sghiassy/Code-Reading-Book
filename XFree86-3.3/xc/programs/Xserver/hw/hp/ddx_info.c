/* $XConsortium: ddx_info.c,v 1.3 95/01/24 01:43:05 dpw Exp $ */

/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
 *                          RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the U.S. Government is subject to
 * restrictions as set forth in sub-paragraph (c)(1)(ii) of the Rights in
 * Technical Data and Computer Software clause in DFARS 252.227-7013.
 * 
 *                          Hewlett-Packard Company
 *                          3000 Hanover Street
 *                          Palo Alto, CA 94304 U.S.A.
 * 
 * Rights for non-DOD U.S. Government Departments and Agencies are as set
 * forth in FAR 52.227-19(c)(1,2).
 *
 *************************************************************************/


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/framebuf.h>
#include <fcntl.h>

#include <X.h>
#include <Xproto.h>
#include <scrnintstr.h>
#include <pixmapstr.h>
#include <resource.h>
#include <misc.h>
#include <mi.h>
#include <servermd.h>
#include <windowstr.h>
#include <colormap.h>

#include "hppriv.h"

#if defined(__hp9000s700) || defined(__hp9000s300)
/* Until <graphics.h> (s400) and <framebuf.h> (s700) are updated,
 * define graphics IDs returned by ioctl(GCDESCRIBE).  On both
 * platforms, this ID equals the upper 4 bytes of the STI ROM ID.
 *
 * (s400 won't support 24-plane color (Rattler); that's handled below).
 */
/* Until <framebuf.h> (s700) is updated */
#   ifndef S9000_ID_A1659A          /* CRX (8-plane Color) */
#       define S9000_ID_A1659A          0x26D1482A
#   endif
#   ifndef S9000_ID_A1924A          /* GRX (Gray Scale) */
#       define S9000_ID_A1924A          0x26D1488C
#   endif
#   ifndef S9000_ID_A1439A          /* CRX+ (24-plane Color */
#       define S9000_ID_A1439A          0x26D148EE
#   endif
#   ifndef S9000_ID_TIMBER          /* Bushmaster (710) Graphics */
#       define S9000_ID_TIMBER          0x27F12392
#   endif
#   ifndef S9000_ID_TOMCAT          /* 2-headed ELK; sorry, no P/N yet */
#       define S9000_ID_TOMCAT          0x27FCCB6D
#   endif
#   ifndef S9000_ID_ARTIST          /* Artist 712/715 mother board graphics */
#       define S9000_ID_ARTIST          0x2B4DED6D
#   endif
#   ifndef S9000_ID_HCRX            /* Hyperdrive A4071A */
#       define S9000_ID_HCRX          0x2BCB015A
#   endif
#endif

/* Declare the external initialization functions */

extern Bool	ngleScreenInit();


        /*  --------------------- ddx_driver_info -------------------------- */
        /*  This is an inital entry point into the hpux ddx drivers.  This 
	    procedure is called to determine if ddx driver support exists 
	    for a given display and configuration.  The display and 
	    configuration are defined by the hp private structure passed 
	    into this routine.  A null pointer is returned if no ddx driver 
	    support is available.  If a ddx driver does exist to support the 
	    given configuration, then a pointer to the driver's initialization
	    routine, which is later called by InitOutput(), routine is
	    returned.
        */

Bool (*ddx_driver_info( php))()
    hpPrivPtr php;
{
    struct stat 		statInfo;
    int 			result;
    crt_frame_buffer_t 		desc;
    char 			*map_origin = NULL;
    Bool			(*return_value)();


    /* Stat the file descriptor to see if the device file exists: */
    if((result = stat( php->StandardDevice, &statInfo)) < 0)
    {
	return(NULL);
    }

    /* Stat the file descriptor to see if the device exists: */
    if(!( statInfo.st_mode & S_IFCHR))
    {
	return(NULL);
    }

    /* Let's open the device: */
    if(( php->StandardFd = open(php->StandardDevice, O_RDWR)) < 0)
    {
	return(NULL);
    }

    /* Map the device: */
    if( ioctl( php->StandardFd, GCMAP, &map_origin) < 0)
    {
	close(php->StandardFd);
	return(NULL);
    }

    /* Does the kernel know what's out there?: */
    if( ioctl( php->StandardFd, GCDESCRIBE, &desc) < 0)
    {
	ioctl(php->StandardFd, GCUNMAP, &map_origin );
	close(php->StandardFd );
	return(NULL);
    }

    /* Let's give the driver a chance to look at the device: */
    switch(desc.crt_id)
    {
	/* Devices supported on s700 */
	case S9000_ID_A1924A:	    /* GRX (8-plane Gray Scale) */
	case S9000_ID_A1659A:	    /* CRX (8-plane Color) */
	case S9000_ID_TOMCAT:	    /* 2-Headed CRX */
	case S9000_ID_TIMBER:	    /* Bushmaster (710) Graphics */
	case S9000_ID_A1439A:	    /* CRX24 (24-plane Color) */
	case S9000_ID_ARTIST:	    /* 712 (8-plane Color) Graphics */
	case S9000_ID_HCRX:         /* Hyperdrive (8 or 24 plane) */

	    return_value = ngleScreenInit;
	    break;

	default:
	    return_value = NULL;
	    break;
    }

    /* We've got our info, so lets close things down so the driver can
     * re-open them:
     */
    ioctl(php->StandardFd, GCUNMAP, &map_origin);
    close(php->StandardFd );

    return(return_value);
}
