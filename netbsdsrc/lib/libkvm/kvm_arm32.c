/*	$NetBSD: kvm_arm32.c,v 1.6 1997/10/18 02:37:00 mark Exp $	*/

/*-
 * Copyright (C) 1996 Wolfgang Solfrank.
 * Copyright (C) 1996 TooLs GmbH.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	from: kvm_powerpc.c,v 1.3 1997/09/19 04:00:23 thorpej Exp
 */

/*
 * arm32 machine dependent routines for kvm.
 */

#include <sys/param.h>

#include <vm/vm.h>

#include <stdlib.h>
#include <db.h>
#include <limits.h>
#include <kvm.h>

#include "kvm_private.h"

void
_kvm_freevtop(kd)
	kvm_t *kd;
{
	if (kd->vmst != 0)
		free(kd->vmst);
}

int
_kvm_initvtop(kd)
	kvm_t *kd;
{
	return 0;
}

int
_kvm_kvatop(kd, va, pa)
	kvm_t *kd;
	u_long va;
	u_long *pa;
{
	_kvm_err(kd, 0, "vatop not yet implemented!");
	return 0;
}

off_t
_kvm_pa2off(kd, pa)
	kvm_t *kd;
	u_long pa;
{
	_kvm_err(kd, 0, "pa2off not yet implemented!");
	return 0;
}

/*
 * Machine-dependent initialization for ALL open kvm descriptors,
 * not just those for a kernel crash dump.  Some architectures
 * have to deal with these NOT being constants!  (i.e. m68k)
 */
int
_kvm_mdopen(kd)
	kvm_t	*kd;
{

	kd->usrstack = USRSTACK;
	kd->min_uva = VM_MIN_ADDRESS;
	kd->max_uva = VM_MAXUSER_ADDRESS;

	return (0);
}
