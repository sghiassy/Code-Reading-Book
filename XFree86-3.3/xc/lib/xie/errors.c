/* $XConsortium: errors.c,v 1.5 95/04/28 18:01:11 mor Exp $ */

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

#define NEED_EVENTS	/* so XErrorEvent will get pulled in */

#include "XIElibint.h"


Bool
_XieFloError (display, host, wire)

Display		*display;
XErrorEvent	*host;
xError		*wire;

{
    XieFloAccessError	*flo_host_error = (XieFloAccessError *) host;
    xieFloAccessErr	*flo_wire_error = (xieFloAccessErr *) wire;

    /*
     * All flo errors have this basic info.
     */

/*  these are assigned by Xlib already
    flo_host_error->error_code     = flo_wire_error->code;
    flo_host_error->flo_id         = flo_wire_error->floID;
    flo_host_error->minor_code     = flo_wire_error->minorOpcode;
    flo_host_error->request_code   = flo_wire_error->majorOpcode;
*/
    flo_host_error->flo_error_code = flo_wire_error->floErrorCode;
    flo_host_error->name_space     = flo_wire_error->nameSpace;
    flo_host_error->phototag       = flo_wire_error->phototag;
    flo_host_error->elem_type      = flo_wire_error->type;


    /*
     * Now handle the particularites of each flo error.
     */

    switch (((xieFloAccessErr *) wire)->floErrorCode)
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
    case xieErrNoFloColorList:
    case xieErrNoFloDrawable:
    case xieErrNoFloGC:
    case xieErrNoFloLUT:
    case xieErrNoFloPhotomap:
    case xieErrNoFloROI:

    {
	XieFloResourceError	*host_error = (XieFloResourceError *) host;
	xieFloResourceErr	*wire_error = (xieFloResourceErr *) wire;

	host_error->resource_id = wire_error->resourceID;
	break;
    }

    case xieErrNoFloDomain:
    {
	XieFloDomainError	*host_error = (XieFloDomainError *) host;
	xieFloDomainErr		*wire_error = (xieFloDomainErr *) wire;

	host_error->domain_src = wire_error->domainSrc;
	break;
    }

    case xieErrNoFloOperator:
    {
	XieFloOperatorError	*host_error = (XieFloOperatorError *) host;
	xieFloOperatorErr	*wire_error = (xieFloOperatorErr *) wire;

	host_error->operator = wire_error->operator;
	break;
    }

    case xieErrNoFloTechnique:
    {
	XieFloTechniqueError	*host_error = (XieFloTechniqueError *) host;
	xieFloTechniqueErr	*wire_error = (xieFloTechniqueErr *) wire;

	host_error->technique_number = wire_error->techniqueNumber;
	host_error->num_tech_params  = wire_error->lenTechParams;
	host_error->tech_group = wire_error->techniqueGroup;
	break;
    }

    case xieErrNoFloValue:
    {
	XieFloValueError	*host_error = (XieFloValueError *) host;
	xieFloValueErr		*wire_error = (xieFloValueErr *) wire;

	host_error->bad_value = wire_error->badValue;
	break;
    }

    default:
        return (False);
    }

    return (True);
}
