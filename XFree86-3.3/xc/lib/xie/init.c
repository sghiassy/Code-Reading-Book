/* $XConsortium: init.c,v 1.11 95/04/28 18:01:11 mor Exp $ */

/*

Copyright (c) 1993, 1994  X Consortium

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

#define NEED_EVENTS	/* so XErrorEvent will get pulled in */

#include "XIElibint.h"
#include "globals.h"
#include "init.h"
#include <stdio.h>



Status
XieInitialize (display, extinfo_ret)

Display			*display;
XieExtensionInfo	**extinfo_ret;

{
    XieExtInfo			*xieExtInfo;
    XExtCodes			*extCodes;
    xieQueryImageExtensionReq	*req;
    xieQueryImageExtensionReply	rep;
    char			*pBuf;

    int				_XieCloseDisplay();
    void			_XiePrintError();

    int				_XieColorAllocEvent();
    int				_XieDecodeNotifyEvent();
    int				_XieExportAvailableEvent();
    int				_XieImportObscuredEvent();
    int				_XiePhotofloDoneEvent();

    int				_XieFloError();

    LockDisplay (display);
    GET_EXTENSION_INFO (display, xieExtInfo);

    if (xieExtInfo)
    {
	*extinfo_ret = xieExtInfo->extInfo;

	return (1);
    }

    *extinfo_ret = NULL;

    if ((extCodes = XInitExtension (display, xieExtName)) == NULL)
    {
        UnlockDisplay (display);
	SYNC_HANDLE (display);

	return (0);
    }

    xieExtInfo = (XieExtInfo *)	Xmalloc (sizeof (XieExtInfo));
    xieExtInfo->extCodes = extCodes;
    xieExtInfo->extInfo = *extinfo_ret =
	(XieExtensionInfo *) Xmalloc (sizeof (XieExtensionInfo));

    ADD_EXTENSION_INFO (display, xieExtInfo);

    GET_REQUEST (QueryImageExtension, pBuf);

    BEGIN_REQUEST_HEADER (QueryImageExtension, pBuf, req);

    STORE_REQUEST_HEADER (QueryImageExtension, req);
    req->majorVersion = xieMajorVersion;
    req->minorVersion = xieMinorVersion;

    END_REQUEST_HEADER (QueryImageExtension, pBuf, req);

    if (_XReply (display, (xReply *)&rep, 0, xFalse) == 0)
    {
        UnlockDisplay (display);
	SYNC_HANDLE (display);

	return (0);
    }

    (*extinfo_ret)->server_major_rev = rep.majorVersion;
    (*extinfo_ret)->server_minor_rev = rep.minorVersion;
    (*extinfo_ret)->service_class    = rep.serviceClass;
    (*extinfo_ret)->alignment        = rep.alignment;
    (*extinfo_ret)->uncnst_mantissa  = rep.unconstrainedMantissa;
    (*extinfo_ret)->uncnst_min_exp   = rep.unconstrainedMinExp;
    (*extinfo_ret)->uncnst_max_exp   = rep.unconstrainedMaxExp;
    (*extinfo_ret)->n_cnst_levels    = rep.length;
    (*extinfo_ret)->major_opcode     = extCodes->major_opcode;
    (*extinfo_ret)->first_event      = extCodes->first_event;
    (*extinfo_ret)->first_error      = extCodes->first_error;


    (*extinfo_ret)->cnst_levels = (unsigned long *)
	Xmalloc (rep.length * sizeof (unsigned long));

    _XRead32 (display, (*extinfo_ret)->cnst_levels, rep.length << 2);


    /*
     * Tell Xlib which function to call when the display is closed.
     */

    XESetCloseDisplay (display, extCodes->extension, _XieCloseDisplay);


    /*
     * Tell Xlib how to convert wire events to host format.
     */

    XESetWireToEvent (display, extCodes->first_event + xieEvnNoColorAlloc,
	_XieColorAllocEvent);

    XESetWireToEvent (display, extCodes->first_event + xieEvnNoDecodeNotify,
	_XieDecodeNotifyEvent);

    XESetWireToEvent (display, extCodes->first_event + xieEvnNoExportAvailable,
	_XieExportAvailableEvent);

    XESetWireToEvent (display, extCodes->first_event + xieEvnNoImportObscured,
	_XieImportObscuredEvent);

    XESetWireToEvent (display, extCodes->first_event + xieEvnNoPhotofloDone,
	_XiePhotofloDoneEvent);


    /*
     * Tell Xlib how to convert Flo errors from wire to host format.
     * Xlib can convert the other errors for us.
     */

    XESetWireToError (display, extCodes->first_error + xieErrNoFlo,
	_XieFloError);


    /*
     * Tell Xlib how to print XIE errors.
     */

    XESetPrintErrorValues (display, extCodes->extension, _XiePrintError);


    /*
     * If this is the first time XieInitialize has been called,
     * we must register the standard technique functions.
     */

    if (!_XieTechFuncsInitialized)
    {
	_XieInitTechFuncTable ();
	_XieTechFuncsInitialized = 1;
    }


    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (1);
}



Status
XieQueryTechniques (display, technique_group, ntechniques_ret, techniques_ret)

Display			*display;
XieTechniqueGroup	technique_group;
int			*ntechniques_ret;
XieTechnique		**techniques_ret;

{
    xieQueryTechniquesReq	*req;
    xieQueryTechniquesReply	rep;
    char			*pBuf, *pBufStart;
    XieTechnique		*techRet;
    xieTypTechniqueRec		*techRec;
    int				i;

    LockDisplay (display);

    GET_REQUEST (QueryTechniques, pBuf);

    BEGIN_REQUEST_HEADER (QueryTechniques, pBuf, req);

    STORE_REQUEST_HEADER (QueryTechniques, req);
    req->techniqueGroup = technique_group;

    END_REQUEST_HEADER (QueryTechniques, pBuf, req);

    if (_XReply (display, (xReply *)&rep, 0, xFalse) == 0)
    {
        UnlockDisplay (display);
	SYNC_HANDLE (display);

	*ntechniques_ret = 0;
	*techniques_ret = NULL;

	return (0);
    }

    XREAD_INTO_SCRATCH (display, pBuf, rep.length << 2);
    pBufStart = pBuf;

    *ntechniques_ret = rep.numTechniques;

    *techniques_ret = techRet = (XieTechnique *) Xmalloc (
	sizeof (XieTechnique) * rep.numTechniques);

    for (i = 0; i < (int) rep.numTechniques; i++)
    {
	GET_TECHNIQUE_REC (pBuf, techRec);
	pBuf += SIZEOF (xieTypTechniqueRec);

	techRet[i].needs_param = techRec->needsParam;
	techRet[i].group = techRec->group;
	techRet[i].number = techRec->number;
	techRet[i].speed = techRec->speed;

	techRet[i].name = (char *) Xmalloc (
	    (unsigned) techRec->nameLength + 1);
	memcpy (techRet[i].name, pBuf, (unsigned) techRec->nameLength);
	techRet[i].name[techRec->nameLength] = '\0';
	pBuf += PADDED_BYTES (techRec->nameLength);
    }

    FINISH_WITH_SCRATCH (display, pBufStart, rep.length << 2);
    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (1);
}



/*
 * Routine called when a display is closed via XCloseDisplay.
 * The extension information attached to this display is freed.
 * The callback is set up in XieInitialize.
 */

int
_XieCloseDisplay (display, codes)

Display		*display;
XExtCodes	*codes;

{
    XieExtInfo	*xieExtInfo;

    REMOVE_EXTENSION_INFO (display, xieExtInfo);

    if (xieExtInfo == NULL)
	return (0);
    else
    {
	Xfree ((char *) xieExtInfo->extInfo->cnst_levels);
	Xfree ((char *) xieExtInfo->extInfo);
	Xfree ((char *) xieExtInfo);
	return (1);
    }
}



/*
 * Routine called when Xlib wants to print an error.  This callback
 * is setup in XieInitialize.
 */

void
_XiePrintError (display, error, fp)

Display		*display;
XErrorEvent	*error;
#if NeedFunctionPrototypes
void		*fp;
#else
FILE		*fp;
#endif
{
    XieExtInfo		 *xieExtInfo;
    XieFloAccessError    *flo_error      = (XieFloAccessError *) error;
    XieFloResourceError	 *res_error      = (XieFloResourceError *) error;
    XieFloDomainError	 *domain_error   = (XieFloDomainError *) error;
    XieFloOperatorError	 *operator_error = (XieFloOperatorError *) error;
    XieFloTechniqueError *tech_error     = (XieFloTechniqueError *) error;
    XieFloValueError	 *value_error    = (XieFloValueError *) error;

    /*
     * Xlib bug - extension codes should be passed to this function,
     * but they're not.  We must get them ourselves.
     */

    GET_EXTENSION_INFO (display, xieExtInfo);

    if (error->error_code ==
	xieExtInfo->extCodes->first_error + xieErrNoFlo)
    {
	/*
	 * Print the flo error type.
	 */

	fprintf (fp, "  Flo error:  ");

	switch (flo_error->flo_error_code)
	{
	case xieErrNoFloAccess:
	    fprintf (fp, "FloAccess\n");
	    break;
	case xieErrNoFloAlloc:
	    fprintf (fp, "FloAlloc\n");
	    break;
	case xieErrNoFloElement:
	    fprintf (fp, "FloElement\n");
	    break;
	case xieErrNoFloID:
	    fprintf (fp, "FloID\n");
	    break;
	case xieErrNoFloMatch:
	    fprintf (fp, "FloMatch\n");
	    break;
	case xieErrNoFloSource:
	    fprintf (fp, "FloSource\n");
	    break;
	case xieErrNoFloColormap:
	    fprintf (fp, "FloColormap\n");
	    break;
	case xieErrNoFloColorList:
	    fprintf (fp, "FloColorList\n");
	    break;
	case xieErrNoFloDrawable:
	    fprintf (fp, "FloDrawable\n");
	    break;
	case xieErrNoFloGC:
	    fprintf (fp, "FloGC\n");
	    break;
	case xieErrNoFloLUT:
	    fprintf (fp, "FloLUT\n");
	    break;
	case xieErrNoFloPhotomap:
	    fprintf (fp, "FloPhotomap\n");
	    break;
	case xieErrNoFloROI:
	    fprintf (fp, "FloROI\n");
	    break;
	case xieErrNoFloDomain:
	    fprintf (fp, "FloDomain\n");
	    break;
	case xieErrNoFloOperator:
	    fprintf (fp, "FloOperator\n");
	    break;
	case xieErrNoFloTechnique:
	    fprintf (fp, "FloTechnique\n");
	    break;
	case xieErrNoFloValue:
	    fprintf (fp, "FloValue\n");
	    break;
	case xieErrNoFloImplementation:
	    fprintf (fp, "FloImplementation\n");
	    break;
	default:
	    break;
	}


	/*
	 * All flo errors have this basic info to print.
	 */

	fprintf (fp, "  Name-space:  0x%lx\n", flo_error->name_space);
	fprintf (fp, "  Phototag:  %d\n", flo_error->phototag);
	fprintf (fp, "  Element type:  %d\n", flo_error->elem_type);


	/*
	 * Now handle the particularites of each flo error.
	 */

	switch (flo_error->flo_error_code)
	{
	case xieErrNoFloAccess:
	case xieErrNoFloAlloc:
	case xieErrNoFloElement:
	case xieErrNoFloID:
	case xieErrNoFloMatch:
	case xieErrNoFloSource:
	case xieErrNoFloImplementation:
	    break;

	case xieErrNoFloColormap:
	    fprintf (fp, "  Colormap:  0x%lx\n", res_error->resource_id);
	    break;

	case xieErrNoFloColorList:
	    fprintf (fp, "  ColorList:  0x%lx\n", res_error->resource_id);
	    break;

	case xieErrNoFloDrawable:
	    fprintf (fp, "  Drawable:  0x%lx\n", res_error->resource_id);
	    break;

	case xieErrNoFloGC:
	    fprintf (fp, "  GC:  0x%lx\n", res_error->resource_id);
	    break;

	case xieErrNoFloLUT:
	    fprintf (fp, "  LUT:  0x%lx\n", res_error->resource_id);
	    break;

	case xieErrNoFloPhotomap:
	    fprintf (fp, "  Photomap:  0x%lx\n", res_error->resource_id);
	    break;

	case xieErrNoFloROI:
	    fprintf (fp, "  ROI:  0x%lx\n", res_error->resource_id);
	    break;

	case xieErrNoFloDomain:
	    fprintf (fp, "  Phototag of domain src:  %d\n",
		domain_error->domain_src);
	    break;

	case xieErrNoFloOperator:
	    fprintf (fp, "  Operator:  0x%lx\n", operator_error->operator);
	    break;

	case xieErrNoFloTechnique:
	    fprintf (fp, "  Technique number:  0x%lx\n",
		tech_error->technique_number);
	    fprintf (fp, "  Number of technique params:  %d\n",
		tech_error->num_tech_params);
	    fprintf (fp, "  Technique group:  %d\n",
		tech_error->tech_group);
	    break;

	case xieErrNoFloValue:
	    fprintf (fp, "  Bad value:  0x%lx\n", value_error->bad_value);
	    break;

	default:
	    break;
	}
    }
}
