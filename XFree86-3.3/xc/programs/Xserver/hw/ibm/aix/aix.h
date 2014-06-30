/*
 * $XConsortium: AIX.h,v 1.3 91/11/22 17:06:53 eswu Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#ifndef _AIX_H
#define _AIX_H

#define AIX_GLYPHS_PER_KEY   2
#define AIX_MIN_KEY       0x08
#define AIX_MAX_KEY       0x85+AIX_MIN_KEY

#define XDEV_IBM_K101		  0	/* American keyboard (101 keys)  */
#define XDEV_IBM_K102		  1	/* European keyboard (102 keys)  */
#define XDEV_IBM_K106		  2	/* Japanese keyboard (106 keys)  */

#define XDEV_IBM_APA8		6153
#define XDEV_IBM_APA8C		6154
#define XDEV_IBM_APA16		6155
#define XDEV_IBM_MPEL		5081
#define XDEV_IBM_EGA		5154
#define XDEV_IBM_8514A		8514
#define XDEV_IBM_GSL		4444

#define XDEV_IBM_GAI		9999
#define XDEV_IBM_SKYWAY		1111	
#define XDEV_IBM_SABINE		2222
#define XDEV_IBM_GEMINI		3333
#define XDEV_IBM_SKYMONO	5555
#define XDEV_IBM_PEDER		6666

#define AIXMappingDevice	1
#define AIXMappingDial		2
#define AIXMappingLpfk		3
#ifdef AIXTABLET
#define AIXMappingTablet        4
#endif

#define DEVDIAL		1
#define DEVVAL		1
#define DEVLPFK		2
#define DEVKBD		3
#define DEVMOUSE	4
#define DEVPOINTER	4
#define DEVTABLET	5

#define AIXBadDevice    128
#define AIXBadMode      129

#define AIXInputFocus	1

#define AIXDeviceEventMode 		1
#define AIXPointerType 			2

#define AIXDeviceAutoLoadoff		0
#define AIXDeviceAutoLoadon		1 
#define AIXDeviceDisable		0
#define AIXDeviceEnable	 		1

#ifdef AIXTABLET
#define TabletEnglish           0
#define TabletMetric            1
#define TabletLowerLeft         0
#define TabletCenter            1
#endif

#define AIXDIALNAME "AixDialExtension" /* dial extensions */
#define AIXLPFKNAME "AixLpfkExtension" /* lpfk extensions */
#ifdef AIXTABLET
#define AIXTABLETNAME "AixTabletExtension" /* tablet extensions */
#endif
#define AIXDEVICECONTROLNAME "AixDeviceControlExtension" /* device control */
#define XAIXEXTENSIONNAME "XAixExtension" /* misc extensions */
#define AIXEVENTCONTROLNAME "AixEventControlExtension" /* event control */
#define AIXSTATNAME "AixStatExtension"  /* profile X window system */
#define AIXDIRECTRESOURCENAME "AixDirectResource" /* direct resource access */

#define AIXDIRECTNAME  "aixDirectAccessExtension"
#define DIRECTACCESSNAME "xDirectAccessExtension"
#define AIXCURSOREXTENSION "aixCursorExtension"	/* crosshair, 3colors, etc */
#define COLORMAPEXTNAME "xColormapExtension"	/* blink, etc */

/* generic input extensions */

#define X_SelectDevice		1
#define X_SetDeviceControl	2
#define X_GetDeviceControl	3
#define X_SetDeviceFocus	4
#define X_GetDeviceFocus	5
#define X_GrabDeviceKey		6
#define X_UngrabDeviceKey	7
#define X_GrabDevice		8
#define X_UngrabDevice		9
#define X_GrabDeviceButton	10
#define X_UngrabDeviceButton	11

/* dial extensions */

#define X_GetDialControl        1
#define X_SetDialControl     	2
#define X_GetDialFocus     	3
#define X_SetDialFocus     	4
#define X_GrabDial     		5	/* non-support */
#define X_UngrabDial     	6	/* non-support */
#define X_SelectDial		7
#define X_SetDialAttributes	8
#define X_GetDialAttributes	9

/* lpfk extensions */

#define X_GetLpfkControl        1
#define X_SetLpfkControl     	2
#define X_GetLpfkFocus     	3
#define X_SetLpfkFocus     	4
#define X_GrabLpfk     		5	/* non-support */
#define X_UngrabLpfk     	6	/* non-support */
#define X_SelectLpfk		7
#define X_GrabLpfkKey	 	8	/* non-support */
#define X_UngrabLpfkKey		9	/* non-support */
#define X_GetLpfkAttributes     10
#define X_SetLpfkAttributes    	11

#ifdef AIXTABLET
/* tablet extensions */

#define X_GetTabletControl      1
#define X_SetTabletControl      2
#define X_GetTabletFocus        3
#define X_SetTabletFocus        4
#define X_GrabTablet            5       /* non-support */
#define X_UngrabTablet          6       /* non-support */
#define X_SelectTablet          7
#define X_GetTabletAttributes   8
#define X_SetTabletAttributes   9
#endif

/* xwindow system statistics */
 
#define X_StartXStat		1
#define X_StopXStat		2
#define X_ContXStat		3
#define X_SetXStat		4
#define X_GetXStat		5
#define X_ResetXStat		6
#define X_GetXStats		7
#define X_ExitXStat		8

/* device control extensions */

#define X_SetInputDevice	1
#define X_ListInputDevices	2
#define X_QueryInputDevice	3
#define X_GetDPYInfo		4
#define X_ExtChangeMode		5
#define X_SelectDeviceInput	6
#define X_ChangeColors 		7	/* non-support */
#define X_ZZZZZ			8	/* non-support */
#define X_ExtQueryMode		9

/* event control extensions */

#define X_InitInputEvent	1	/* non-support */
#define X_ListInputEvents	2	/* non-support */
#define X_GetInputEvent		3	/* non-support */

/* misc extensions */

#define X_DrawPolyMarker	1
#define X_SetPolyMarker		2
#define X_DrawPolyMarkers	3
#define X_AAAA			4	/* non support */
#define X_BBBB			5	/* non support */
#define X_DrawGeoText		6	/* non support */
#define X_SetGeoText		7	/* non support */
#define X_CCCC              	8       /* non support */

/* direct resource access */

#define X_DirectAdapterAccess		1
#define X_DirectWindowAccess		2
#define X_DirectFontAccess		3
#define X_QueryDirectResourceAccess	4

#define X_CreateCrossHair	1
#define X_QueryCrossHair	2
#define X_CreateMultiColorCur	3	/* for 3-colors cursor */
#define X_RecolorMultiColorCur	4

#define X_ExtBlink		1	/* to support GL only */
#define X_ExtStoreAnyColors	2	/* to support GL only */

/* EventMask  */

#define LPFKeyPressMask		1L
#define DialRotateMask		(1L << 1) 
#define AIXDeviceMapChangeMask	(1L << 2)
#define AIXFocusChangeMask	(1L << 3)
#define AIXButtonPressMask	(1L << 4)  	  
#define AIXButtonReleaseMask	(1L << 5) 
#define AIXKeyPressMask	        (1L << 6)
#define AIXKeyReleaseMask	(1L << 7)
#ifdef AIXTABLET
#define AIXDeviceMotionMask     (1L << 8)
#define AIXDeviceMotionHintMask (1L << 9)
#define AIXButtonMotionMask     (1L << 10)
#define AIXButton1MotionMask    (1L << 11)
#define AIXButton2MotionMask    (1L << 12)
#define AIXButton3MotionMask    (1L << 13)
#define AIXButton4MotionMask    (1L << 14)
#define AIXButton5MotionMask    (1L << 15)
#endif

/* Event Type */

#define DialRotate	  64
#define LPFKeyPress	  65
#define AIXDeviceMappingNotify 66
#define AIXFocusIn	  67
#define AIXFocusOut	  68
#define AIXButtonPress	  69
#define AIXButtonRelease  70
#ifdef AIXTABLET
#define DeviceMotion            71
#define DeviceButtonPress       72
#define DeviceButtonRelease     73
#endif

/* Grab stuff ... */

#define AsyncDial		8
#define SyncDial		9
#define ReplayDial		10	
#define AsyncLpfk		11	
#define SyncLpfk		12	
#define ReplayLpfk		13	
#define AsyncAll		14	
#define SyncAll			15
#ifdef AIXTABLET
#define AsyncTablet             16
#define SyncTablet              17
#define ReplayTablet            18
#endif

/* Dial Mask */

#define	DialMask0	1L
#define	DialMask1	(1L << 1)
#define	DialMask2	(1L << 2)
#define	DialMask3	(1L << 3)
#define	DialMask4	(1L << 4)
#define	DialMask5	(1L << 5)
#define	DialMask6	(1L << 6)
#define	DialMask7	(1L << 7)
#define	DialMask8	(1L << 8)
#define	DialMask9	(1L << 9)
#define	DialMask10	(1L << 10)
#define	DialMask11	(1L << 11)
#define	DialMask12	(1L << 12)
#define	DialMask13	(1L << 13)
#define	DialMask14	(1L << 14)
#define	DialMask15	(1L << 15)
#define	DialMask16	(1L << 16)
#define	DialMask17	(1L << 17)
#define	DialMask18	(1L << 18)
#define	DialMask19	(1L << 19)
#define	DialMask20	(1L << 20)
#define	DialMask21	(1L << 21)
#define	DialMask22	(1L << 22)
#define	DialMask23	(1L << 23)

#define AllDialsMask    (-1) 

/* LPFKey Mask */

#define	LPFKeyMask0	1L
#define	LPFKeyMask1	(1L << 1)
#define	LPFKeyMask2	(1L << 2)
#define	LPFKeyMask3	(1L << 3)
#define	LPFKeyMask4	(1L << 4)
#define	LPFKeyMask5	(1L << 5)
#define	LPFKeyMask6	(1L << 6)
#define	LPFKeyMask7	(1L << 7)
#define	LPFKeyMask8	(1L << 8)
#define	LPFKeyMask9	(1L << 9)
#define	LPFKeyMask10	(1L << 10)
#define	LPFKeyMask11	(1L << 11)
#define	LPFKeyMask12	(1L << 12)
#define	LPFKeyMask13	(1L << 13)
#define	LPFKeyMask14	(1L << 14)
#define	LPFKeyMask15	(1L << 15)
#define	LPFKeyMask16	(1L << 16)
#define	LPFKeyMask17	(1L << 17)
#define	LPFKeyMask18	(1L << 18)
#define	LPFKeyMask19	(1L << 19)
#define	LPFKeyMask20	(1L << 20)
#define	LPFKeyMask21	(1L << 21)
#define	LPFKeyMask22	(1L << 22)
#define	LPFKeyMask23	(1L << 23)
#define	LPFKeyMask24	(1L << 24)
#define	LPFKeyMask25	(1L << 25)
#define	LPFKeyMask26	(1L << 26)
#define	LPFKeyMask27	(1L << 27)
#define	LPFKeyMask28	(1L << 28)
#define	LPFKeyMask29	(1L << 29)
#define	LPFKeyMask30	(1L << 30)
#define	LPFKeyMask31	(1L << 31)

#define AllLpfksMask    (-1) 

#ifdef AIXTABLET
/* Tablet Mask */

#define TabletMask0     1L
#define TabletMask1     (1L << 1)
#define TabletMask2     (1L << 2)
#define TabletMask3     (1L << 3)
#define TabletMask4     (1L << 4)
#define TabletMask5     (1L << 5)
#define TabletMask6     (1L << 6)
#define TabletMask7     (1L << 7)
#define TabletMask8     (1L << 8)
#define TabletMask9     (1L << 9)
#define TabletMask10    (1L << 10)
#define TabletMask11    (1L << 11)
#define TabletMask12    (1L << 12)
#define TabletMask13    (1L << 13)
#define TabletMask14    (1L << 14)
#define TabletMask15    (1L << 15)
#define TabletMask16    (1L << 16)
#define TabletMask17    (1L << 17)
#define TabletMask18    (1L << 18)
#define TabletMask19    (1L << 19)
#define TabletMask20    (1L << 20)
#define TabletMask21    (1L << 21)
#define TabletMask22    (1L << 22)
#define TabletMask23    (1L << 23)

#define AllTabletsMask  (-1)
#endif

#endif /* _AIX_H */
