/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/s3v/s3v_rop.h,v 1.1.2.1 1997/05/14 07:52:59 dawes Exp $ */
/*
 *
 * Copyright 1995-1997 The XFree86 Project, Inc.
 *
 */
/* This file contains the data structures which map the X ROPs to the
 * ViRGE ROPs. It also contains other mappings which are used when supporting
 * planemasks and transparency.
 *
 * Created by Sebastien Marineau, 29/03/97.
 * This file should be included only from s3v_accel.c to avoid 
 * duplicate symbols. 
 * 
 */

#include "regs3v.h"

static int s3vAlu[16] =
{
   ROP_0,               /* GXclear */
   ROP_DSa,             /* GXand */
   ROP_SDna,            /* GXandReverse */
   ROP_S,               /* GXcopy */
   ROP_DSna,            /* GXandInverted */
   ROP_D,               /* GXnoop */
   ROP_DSx,             /* GXxor */
   ROP_DSo,             /* GXor */
   ROP_DSon,            /* GXnor */
   ROP_DSxn,            /* GXequiv */
   ROP_Dn,              /* GXinvert*/
   ROP_SDno,            /* GXorReverse */
   ROP_Sn,              /* GXcopyInverted */
   ROP_DSno,            /* GXorInverted */
   ROP_DSan,            /* GXnand */
   ROP_1                /* GXset */
};

/* S -> P, for solid and pattern fills. */
static int s3vAlu_sp[16]=
{
   ROP_0,
   ROP_DPa,
   ROP_PDna,
   ROP_P,
   ROP_DPna,
   ROP_D,
   ROP_DPx,
   ROP_DPo,
   ROP_DPon,
   ROP_DPxn,
   ROP_Dn,
   ROP_PDno,
   ROP_Pn,
   ROP_DPno,
   ROP_DPan,
   ROP_1
};

/* ROP  ->  (ROP & P) | (D & ~P) */
/* These are used to support a planemask for S->D ops */
static int s3vAlu_pat[16] =
{
   ROP_0PaDPnao,
   ROP_DSaPaDPnao,
   ROP_SDnaPaDPnao,
   ROP_SPaDPnao,
   ROP_DSnaPaDPnao,
   ROP_DPaDPnao,
   ROP_DSxPaDPnao,
   ROP_DSoPaDPnao,
   ROP_DSonPaDPnao,
   ROP_DSxnPaDPnao,
   ROP_DnPaDPnao,
   ROP_SDnoPaDPnao,
   ROP_SnPaDPnao,
   ROP_DSnoPaDPnao,
   ROP_DSanPaDPnao,
   ROP_1PaDPnao
};

/* ROP -> (ROP & S) | (~ROP & D) */
/* This is used for our transparent mono pattern fills to support trans/plane*/
static int s3vAlu_MonoTrans[16] =
{
   ROP_D,
   ROP_DSPnoa,
   ROP_DPSao,
   ROP_DPSDxax,
   ROP_DPSoa,
   ROP_DSa,
   ROP_SSPxDSxax,
   ROP_SDPoa,
   ROP_DSPnao,
   ROP_SSDxPDxax,
   ROP_DSo,
   ROP_SDPnao,
   ROP_SPDSxax,
   ROP_SDPnoa,
   ROP_SDPao,
   ROP_S
};
