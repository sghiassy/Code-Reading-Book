/* $XConsortium: Scale.h,v 1.6 94/04/17 20:24:59 gildea Exp $ */
/*

Copyright (c) 1993  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#ifndef _XawScale_h
#define _XawScale_h

/***********************************************************************
 *
 * Scale Widget
 *
 ***********************************************************************/

#include <X11/Xaw/Simple.h>
#include <X11/Xmu/Converters.h>

/* Resources:

 All the SimpleWidget resources plus...
 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 aspectRatio         AspectRatio        Float           1.0
 autoscale           Autoscale          Boolean         True
 bufferSize          BufferSize         Cardinal        1024
 foreground          Foreground         Pixel           XtDefaultForeground
 gravity             Gravity            XtGravity       ForgetGravity
 image               Image              XImage*         NULL
 internalHeight      Height             Dimension       2
 internalWidth       Width              Dimension       2
 pasteBuffer         PasteBuffer        Boolean         False
 precision           Precision          Float           0.001
 proportional        Proportional       Boolean         False
 resize              Resize             Boolean         True
 scaleX              ScaleValue         Float           1.0
 scaleY              ScaleValue         Float           1.0
 userData            UserData           XtPointer       NULL
 visual              Visual             Visual*         CopyFromParent
*/

#ifndef _XtStringDefs_h_
#define XtNforeground "foreground"
#define XtNinternalWidth "internalWidth"
#define XtNinternalHeight "internalHeight"
#define XtNresize "resize"
#define XtCResize "Resize"
#endif

#define XtNaspectRatio "aspectRatio"
#define XtCAspectRatio "AspectRatio"
#define XtNbufferSize "bufferSize"
#define XtCBufferSize "BufferSize"
#define XtNscaleX "scaleX"
#define XtNscaleY "scaleY"
#define XtCScaleFactor "ScaleFactor"
#define XtNautoscale "autoscale"
#define XtCAutoscale "Autoscale"
#define XtNproportional "proportional"
#define XtCProportional "Proportional"
#define XtNprecision "precision"
#define XtCPrecision "Precision"
#define XtNgravity "gravity"
#define XtCGravity "Gravity"
#define XtNpasteBuffer "pasteBuffer"
#define XtCPasteBuffer "PasteBuffer"
#define XtNimage "image"
#define XtCImage "image"
#define XtNexponent "exponent"
#define XtCExponent "Exponent"
#define XtNuserData "userData"
#define XtCuserData "UserData"
#define XtRuserData "UserData"
#define XtRImage "Image"
#ifndef XtNvisual
#define XtNvisual "visual"
#endif
#define XtCvisual "Visual"
#define XtRvisual "Visual"

extern void AWSetImage();
extern void SWSetImage();

/* Class record constants */

extern WidgetClass scaleWidgetClass;

typedef struct _ScaleClassRec *ScaleWidgetClass;
typedef struct _ScaleRec      *ScaleWidget;

#endif /* _XawScale_h */

