/* $XConsortium: atomcache.h /main/8 1996/11/18 20:17:12 rws $ */
/*
 * Copyright 1994 Network Computing Devices, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name Network Computing Devices, Inc. not be
 * used in advertising or publicity pertaining to distribution of this 
 * software without specific, written prior permission.
 * 
 * THIS SOFTWARE IS PROVIDED `AS-IS'.  NETWORK COMPUTING DEVICES, INC.,
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT
 * LIMITATION ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NONINFRINGEMENT.  IN NO EVENT SHALL NETWORK
 * COMPUTING DEVICES, INC., BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS OF USE, DATA,
 * OR PROFITS, EVEN IF ADVISED OF THE POSSIBILITY THEREOF, AND REGARDLESS OF
 * WHETHER IN AN ACTION IN CONTRACT, TORT OR NEGLIGENCE, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
#ifndef _ATOMCACHE_H_
#define _ATOMCACHE_H_

#define AtomPreInternFlag	0x1
#define AtomWMCacheFlag		0x2
#define AtomNoCacheFlag		0x4

typedef struct {
    unsigned char flags;
    int len;
    char *name;
} AtomControlRec, *AtomControlPtr;

extern AtomControlPtr atom_control;
extern int atom_control_count;
extern int min_keep_prop_size;
#define DEF_KEEP_PROP_SIZE 8

extern Atom LbxMakeAtom(
#if NeedFunctionPrototypes
    char * /*string*/,
    unsigned /*len*/,
    Atom /*atom*/,
    int /*makeit*/
#endif
);

extern char *NameForAtom(
#if NeedFunctionPrototypes
    Atom /*atom*/
#endif
);

extern unsigned FlagsForAtom(
#if NeedFunctionPrototypes
    Atom /*atom*/
#endif
);

extern void FreeAtoms(
#if NeedFunctionPrototypes
    void
#endif
);

#endif				/* _ATOMCACHE_H_ */
