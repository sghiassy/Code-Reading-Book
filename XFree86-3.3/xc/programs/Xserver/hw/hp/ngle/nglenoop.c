/* $XConsortium: nglenoop.c,v 1.1 93/08/08 12:57:40 rws Exp $ */

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

/* This file contains stub procedures that do nothing or very little.  */

#ifndef MISC_H
#include "misc.h"
#endif /* MISC_H */
#include "nglenoop.h"



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleNoop()
 *
 * Description:
 *
 *	This routine does nothing.  It is useful for stubbing out a procedure
 *	pointer for a procedure that doesn't return anything.
 *
 ******************************************************************************/

void ngleNoop()
{
} /* ngleNoop() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleNoopTrue()
 *
 * Description:
 *
 *	This routine just returns TRUE.  It is useful for stubbing out a
 *	procedure pointer for a procedure that returns a boolean value
 *	where that value should be returned as TRUE.
 *
 ******************************************************************************/

Bool ngleNoopTrue()
{
    return(TRUE);
} /* ngleNoopTrue() */
