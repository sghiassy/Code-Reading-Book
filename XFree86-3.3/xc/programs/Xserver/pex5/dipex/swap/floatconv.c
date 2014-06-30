/* $XConsortium: floatconv.c,v 5.6 94/04/17 20:36:32 hersh Exp $ */

/***********************************************************

Copyright (c) 1989, 1990, 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. 

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Sun Microsystems,
not be used in advertising or publicity pertaining to distribution of 
the software without specific, written prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* 
 * floatconv.c - Code which converts between ieee and vax float types.
 * 
 * Copyright 1988-1991
 * Center for Information Technology Integration (CITI)
 * Information Technology Division
 * University of Michigan
 * Ann Arbor, Michigan
 *
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that copyright notice and this permission
 * notice appear in supporting documentation, and that the names of
 * CITI or THE UNIVERSITY OF MICHIGAN not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS." CITI AND THE UNIVERSITY OF
 * MICHIGAN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL CITI OR THE UNIVERSITY OF MICHIGAN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include"floatconv.h"

#define BITMASK(n) ((((unsigned long)1)<<n)-1)
#define VAX_EXPONENT_BIAS	0x00000081
#define IEEE_EXPONENT_BIAS	0x0000007f

#define VAX_TO_IEEE_BIAS ((CARD32)-VAX_EXPONENT_BIAS + IEEE_EXPONENT_BIAS)
#define IEEE_TO_VAX_BIAS ((CARD32)-IEEE_EXPONENT_BIAS + VAX_EXPONENT_BIAS)

#define MAX_VAX_NEGATIVE 0xffffffff
#define MIN_VAX_NEGATIVE 0x00008000
#define MAX_VAX_POSITIVE 0xffff7fff
#define MIN_VAX_POSITIVE 0x00000000
#define VAX_SIGN_MASK 0xffff7fff

#define MIN_IEEE_NEGATIVE 0x80000000
#define MAX_IEEE_NEGATIVE 0xff800000
#define MIN_IEEE_POSITIVE 0x00000000
#define MAX_IEEE_POSITIVE 0x7f800000
#define IEEE_SIGN_MASK 0x7fffffff

/* stupid procedure. tests different machine's handling of the extreme cases
 void PrintMaxMinTest()
{
    ErrorF("MAX_IEEE_POSITIVE %f\n",MAX_IEEE_POSITIVE);
    ErrorF("MIN_IEEE_POSITIVE %f\n",MIN_IEEE_POSITIVE);
    ErrorF("MAX_IEEE_NEGATIVE %f\n",MAX_IEEE_NEGATIVE);
    ErrorF("MIN_IEEE_NEGATIVE %f\n",MIN_IEEE_NEGATIVE);
    ErrorF("MAX_VAX_POSITIVE  %f\n",MAX_VAX_POSITIVE);
    ErrorF("MIN_VAX_POSITIVE  %f\n",MIN_VAX_POSITIVE);
    ErrorF("MAX_VAX_NEGATIVE  %f\n",MAX_VAX_NEGATIVE);
    ErrorF("MIN_VAX_NEGATIVE  %f\n",MIN_VAX_NEGATIVE);
};
*/

/*****************************************************************
 * TAG( ConvertVaxToIEEE )
 * 
 * 
 * Inputs:
 * 	A floating point number in VAX format.
 * Outputs:
 * 	The floating point number in IEEE format.
 * Assumptions:
 * 	The number must not be 'out of the bounds' of the floating point
 * 	format which it is being converted to.  I have not yet figured a way
 * 	to ensure that the server will not crash after converting some nasty
 * 	floating point number.  My guess, however, is that the problems would
 * 	arise in the other routine more than in the vax to ieee routine.
 *
 * 	The routine handles the MAX and MIN cases.  I found that the
 * 	MIN_XXXX_NEGATIVE numbers cause floating point exceptions on
 * 	the VAX and the RT. Thus -0.0 is never returned and +0.0 is returned
 * 	instead.
 * 	
 * Algorithm:
 * 	brute force BITMASKS and shifts.
 */

void
ConvertVaxToIEEE(VaxnumR)
    PEXFLOAT *VaxnumR;
{
    register CARD32 Vaxnum = *(CARD32 *)VaxnumR;
    CARD32 *VaxnumP = (CARD32 *)VaxnumR;
    CARD32 result;

    if ((VAX_SIGN_MASK & Vaxnum)==MAX_VAX_POSITIVE)
    {
	*VaxnumP = MAX_IEEE_POSITIVE |
	    (((0x00008000)&Vaxnum) ? 0x80000000 : 0L);
	return;
    }
    
    if ((VAX_SIGN_MASK & Vaxnum)==MIN_VAX_POSITIVE)
    {
	*VaxnumP = MIN_IEEE_POSITIVE;
	return;
    }

    /*
     * these bitfields should OR into mutually exclusive fields in
     * result field.
     */
    
    result = ((((BITMASK(8)<<7) & Vaxnum)>>7)+VAX_TO_IEEE_BIAS)<<23;
    result |= (((BITMASK(7) & Vaxnum)<<16) |
	       (((BITMASK(16)<<16) & Vaxnum)>>16));
    result |= ( (0x00008000 & Vaxnum) ? 0x80000000 : 0L);
    *VaxnumP = result;
    return;
}

/*****************************************************************
 * TAG( ConvertIEEEToVax )
 * 
 * Function converts IEEE format floating point numbers to Vax floating
 * point numbers.
 * 
 * Inputs:
 * 	A floating point number in IEEE format.
 * Outputs:
 * 	A floating point number in VAX format.
 * Assumptions:
 * 	The number must not be 'out of the bounds' of the floating point
 * 	format which it is being converted to.  I have not yet figured a way
 * 	to ensure that the server will not crash after converting some nasty
 * 	floating point number.  My guess, however, is that the problems would
 * 	arise in this routine more than the vax to ieee routine.
 *
 * 	The routine handles the MAX and MIN cases.  I found that the
 * 	MIN_XXXX_NEGATIVE numbers cause floating point exceptions on
 * 	the VAX and the RT. Thus -0.0 is never returned and +0.0 is returned
 * 	instead.
 *     
 * Algorithm:
 * 	brute force BITMASKS and shifts.
 */
    
void 
ConvertIEEEToVax(IEEEnumR)
    PEXFLOAT *IEEEnumR;
{
    register CARD32 IEEEnum = *(CARD32 *)IEEEnumR;
    CARD32 *IEEEnumP = (CARD32 *)IEEEnumR;
    CARD32 result=0;

    if ((IEEE_SIGN_MASK & IEEEnum)==MAX_IEEE_POSITIVE)
    {
	*IEEEnumP = MAX_VAX_POSITIVE |
	    (0x80000000&IEEEnum)>>16;
	return;
    };
    
    if ((IEEE_SIGN_MASK & IEEEnum)==MIN_IEEE_POSITIVE)
    {
	*IEEEnumP = MIN_VAX_POSITIVE;
	return;
    };

    /*
     * these bitfields should OR into mutually exclusive fields in
     * result field.
     */

    result = ((((BITMASK(8)<<23) & IEEEnum)>>23)+IEEE_TO_VAX_BIAS)<<7;
    result |= ((BITMASK(7)<<16)&IEEEnum)>>16;
    result |= (BITMASK(16)&IEEEnum)<<16;
    result |= (0x80000000&IEEEnum)>>16;
    *IEEEnumP = result;
    return;  
}
