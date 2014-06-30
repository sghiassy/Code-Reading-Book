/* $XConsortium: options.c /main/13 1996/11/15 21:29:05 rws $ */
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

#include <stdio.h>
#include "X.h"
#include "misc.h"
#include "XLbx.h"
#include "lbxopts.h"
#include "proxyopts.h"
#include "lbximage.h"
#include "util.h"
#include "colormap.h"
#ifndef NO_ZLIB
#include "lbxzlib.h"
#endif /* NO_ZLIB */

static int LbxDeltaReply();
static int LbxProxyDeltaReq();
static int LbxServerDeltaReq();
static int LbxStreamCompReq();
static int LbxBitmapCompReq();
static int LbxPixmapCompReq();
static int LbxSquishReq();
static int LbxUseTagsReq();
static int LbxCmapAllReq();

static int LbxProxyDeltaReply();
static int LbxServerDeltaReply();
static int LbxStreamCompReply();
static int LbxBitmapCompReply();
static int LbxPixmapCompReply();
static int LbxSquishReply();
static int LbxUseTagsReply();
static int LbxCmapAllReply();

#define OPT_INDEX_STREAM_COMP		2

/*
 * List of LBX options we recognize and may negotiate
 */
static struct _LbxOption {
    CARD8	optcode;
    Bool	negotiate;
    int		(*req)();
    int		(*reply)();
} LbxOptions[] = {
    { LBX_OPT_DELTA_PROXY,  TRUE,  LbxProxyDeltaReq,	LbxProxyDeltaReply },
    { LBX_OPT_DELTA_SERVER, TRUE,  LbxServerDeltaReq,	LbxServerDeltaReply },
    { LBX_OPT_STREAM_COMP,  TRUE,  LbxStreamCompReq,	LbxStreamCompReply },
    { LBX_OPT_BITMAP_COMP,  TRUE,  LbxBitmapCompReq,	LbxBitmapCompReply },
    { LBX_OPT_PIXMAP_COMP,  TRUE,  LbxPixmapCompReq,	LbxPixmapCompReply },
    { LBX_OPT_MSG_COMP,     TRUE,  LbxSquishReq,	LbxSquishReply },
    { LBX_OPT_USE_TAGS,     TRUE,  LbxUseTagsReq,	LbxUseTagsReply },
    { LBX_OPT_CMAP_ALL,	    TRUE,  LbxCmapAllReq,	LbxCmapAllReply },
};

#define LBX_N_OPTS	(sizeof(LbxOptions) / sizeof(struct _LbxOption))

static int optcount;
static int optlist[LBX_N_OPTS];

LbxNegOptsRec lbxNegOpt;

void
LbxOptInit()
{
    bzero((char *)&lbxNegOpt, sizeof(LbxNegOptsRec));
    lbxNegOpt.proxyDeltaN = LBX_OPT_DELTA_NCACHE_DFLT;
    lbxNegOpt.proxyDeltaMaxLen = LBX_OPT_DELTA_MSGLEN_DFLT;
    lbxNegOpt.serverDeltaN = LBX_OPT_DELTA_NCACHE_DFLT;
    lbxNegOpt.serverDeltaMaxLen = LBX_OPT_DELTA_MSGLEN_DFLT;
    lbxNegOpt.numBitmapCompMethods = 0;
    lbxNegOpt.bitmapCompMethods = NULL;
    lbxNegOpt.numPixmapCompMethods = 0;
    lbxNegOpt.pixmapCompMethods = NULL;
    lbxNegOpt.squish = TRUE;
    lbxNegOpt.useTags = TRUE;
}

int
LbxOptBuildReq(buf)
    register char *buf;
{
    int		  i;
    char *bufstart = buf;
    char *pnopts = buf++;

    optcount = 0;

    for (i = 0; i < LBX_N_OPTS; i++) {
	int reqlen;

	if (LbxOptions[i].negotiate) {
	    reqlen = (*LbxOptions[i].req)(buf + LBX_OPT_SMALLHDR_LEN);
	    *buf++ = LbxOptions[i].optcode;
	    *buf++ = LBX_OPT_SMALLHDR_LEN + reqlen;
	    buf += reqlen;
	    optlist[optcount++] = i;
	}
    }

    *pnopts = optcount;
    return (buf - bufstart);
}

int
LbxOptParseReply(nopts, preply, replylen)
    int		  nopts;
    unsigned char *preply;
    int		  replylen;
{
    int		  i;

    for (i = 0; i < nopts; i++) {
	int len;
	int hdrlen;
	int result;
	int optindex = *preply;

	if (optindex >= optcount)
	    return -1;
	LBX_OPT_DECODE_LEN(preply + 1, len, hdrlen);
	if (len < ++hdrlen || len > replylen) {
#ifdef OPTDEBUG
	    fprintf(stderr, "bad length, len = %d, hdrlen = %d, optlen = %d\n",
			    len, hdrlen, replylen);
#endif
	    return -1;
	}

	result = (*LbxOptions[optlist[optindex]].reply)(preply + hdrlen,
							len - hdrlen);
	if (result < 0)
	    return -1;

	preply += len;
	replylen -= len;
    }
    return 0;
}

struct deltaOpt {
    CARD8	minN;			/* min cache size */
    CARD8	maxN;			/* max cache size */
    CARD8	prefN;			/* preferred cache size */
    CARD8	minMaxMsgLen;		/* min max-message-len / 4 */
    CARD8	maxMaxMsgLen;		/* max max-message-len / 4 */
    CARD8	prefMaxMsgLen;		/* preferred max-message-len / 4 */
};

static struct deltaOpt proxyDeltaOpt = {
    0,
    0xff,
    LBX_OPT_DELTA_NCACHE_DFLT,
    LBX_OPT_DELTA_MSGLEN_MIN >> 2,
    0xff,
    LBX_OPT_DELTA_MSGLEN_DFLT >> 2
};

static struct deltaOpt serverDeltaOpt = {
    0,
    0xff,
    LBX_OPT_DELTA_NCACHE_DFLT,
    LBX_OPT_DELTA_MSGLEN_MIN >> 2,
    0xff,
    LBX_OPT_DELTA_MSGLEN_DFLT >> 2
};

void
LbxNoDelta()
{
    proxyDeltaOpt.prefN = 0;
    serverDeltaOpt.prefN = 0;
}

static int
LbxProxyDeltaReq(buf)
    unsigned char *buf;
{
    memcpy(buf, (char *)&proxyDeltaOpt, sizeof(struct deltaOpt));
    return LBX_OPT_DELTA_REQLEN;
}

static int
LbxServerDeltaReq(buf)
    unsigned char *buf;
{
    memcpy(buf, (char *)&serverDeltaOpt, sizeof(struct deltaOpt));
    return LBX_OPT_DELTA_REQLEN;
}

static int
LbxProxyDeltaReply(preply, replylen)
    unsigned char *preply;
    int		  replylen;
{
    return LbxDeltaReply(preply, 
			 replylen,
			 &lbxNegOpt.proxyDeltaN,
			 &lbxNegOpt.proxyDeltaMaxLen);
}

static int
LbxServerDeltaReply(preply, replylen)
    unsigned char *preply;
    int		  replylen;
{
    return LbxDeltaReply(preply, 
			 replylen,
			 &lbxNegOpt.serverDeltaN,
			 &lbxNegOpt.serverDeltaMaxLen);
}

static int
LbxDeltaReply(preply, replylen, pn, pmaxlen)
    unsigned char *preply;
    int		  replylen;
    short	  *pn;
    short	  *pmaxlen;
{
    if (replylen < 2)
	return -1;
    *pn = *preply++;
    *pmaxlen = *preply;
    if ((*pmaxlen <<= 2) == 0)
	*pn = 0;
    else if (*pmaxlen < 32) {
#ifdef DEBUG
	printf("bad delta max msg length %d\n", *pmaxlen);
#endif
	return -1;
     }
    return 0;
}

static int OptZlibReq();
static int OptZlibReply();

static struct _LbxStreamComp {
    int		typelen;
    char	*type;
    int		(*req)();
    int		(*reply)();
} LbxStreamComp[] = {
#ifndef NO_ZLIB
    { ZLIB_STRCOMP_OPT_LEN,  ZLIB_STRCOMP_OPT,  OptZlibReq,  OptZlibReply }
#endif /* NO_ZLIB */
};

#define LBX_N_STRCOMP	\
    (sizeof(LbxStreamComp) / sizeof(struct _LbxStreamComp))

void
LbxNoComp()
{
    LbxOptions[OPT_INDEX_STREAM_COMP].negotiate = FALSE;
}

static int
LbxStreamCompReq(buf)
    unsigned char *buf;
{
    int		  i;
    int		  reqlen;

    *buf++ = LBX_N_STRCOMP;
    reqlen = 1;

    for (i = 0; i < LBX_N_STRCOMP; i++) {
	int optdatalen;
	int typelen = LbxStreamComp[i].typelen;

	*buf++ = typelen;
	memcpy(buf, LbxStreamComp[i].type, typelen);
	buf += typelen;
	reqlen += 1 + typelen;

	optdatalen = (*LbxStreamComp[i].req)(buf + 1);
	*buf = optdatalen + 1;
	buf += optdatalen + 1;
	reqlen += optdatalen + 1;
    }

    return reqlen;
}

static int
LbxStreamCompReply(preply, replylen)
    unsigned char *preply;
    int		  replylen;
{
    int		  optindex;

    if (replylen < 1 || (optindex = *preply) >= LBX_N_STRCOMP)
	return -1;
    return (*LbxStreamComp[optindex].reply)(preply + 1, replylen - 1);
	
}

extern int  zlevel;

static int
OptZlibReq(buf)
    unsigned char *buf;
{
    *buf++ = 1; /* len */
    *buf = zlevel;
    return (2);
}

extern LbxStreamCompHandle ZlibInit();
extern int ZlibStuffInput(), ZlibInputAvail(), ZlibFlush(),
	 ZlibRead(), ZlibWriteV();
extern void ZlibCompressOn(), ZlibCompressOff(), ZlibFree();

/*ARGSUSED*/
static int
OptZlibReply(preply, replylen)
    unsigned char *preply;
    int		  replylen;
{
    lbxNegOpt.streamOpts.streamCompInit = ZlibInit;
    lbxNegOpt.streamOpts.streamCompArg = (pointer) zlevel;
    lbxNegOpt.streamOpts.streamCompStuffInput = ZlibStuffInput;
    lbxNegOpt.streamOpts.streamCompInputAvail = ZlibInputAvail;
    lbxNegOpt.streamOpts.streamCompFlush = ZlibFlush;
    lbxNegOpt.streamOpts.streamCompRead = ZlibRead;
    lbxNegOpt.streamOpts.streamCompWriteV = ZlibWriteV;
    lbxNegOpt.streamOpts.streamCompOn = ZlibCompressOn;
    lbxNegOpt.streamOpts.streamCompOff = ZlibCompressOff;
    lbxNegOpt.streamOpts.streamCompFreeHandle = ZlibFree;

    return 0;
}

Bool lbxDoSquishing = TRUE;

static int
LbxSquishReq(buf)
    unsigned char *buf;
{
    *buf = lbxDoSquishing;
    return 1;
}

static int
LbxSquishReply(preply, replylen)
    unsigned char *preply;
    int		  replylen;
{
    if (replylen < 1)
	return -1;
    lbxNegOpt.squish = *preply;
    return 0;
}


Bool lbxUseTags = TRUE;

static int
LbxUseTagsReq(buf)
    unsigned char *buf;
{
    *buf = lbxUseTags;
    return 1;
}


static int
LbxUseTagsReply(preply, replylen)
    unsigned char *preply;
    int		  replylen;
{
    if (replylen < 1)
	return -1;
    lbxNegOpt.useTags = *preply;
    return 0;
}


/*
 * Option negotiation for image compression
 */

LbxBitmapCompMethod
LbxBitmapCompMethods [] = {
  {
    "XC-FaxG42D",		/* compression method name */
    0,				/* inited */
    -1,				/* method opcode - to be filled on reply */
    NULL,			/* init function */
    LbxImageEncodeFaxG42D,	/* encode function */
    LbxImageDecodeFaxG42D	/* decode function */
  }
};

#define NUM_BITMAP_METHODS \
	(sizeof (LbxBitmapCompMethods) / sizeof (LbxBitmapCompMethod))



#if 1
/*
 * Currently, we don't support any pixmap compression algorithms
 * because regular stream compression does much better than PackBits.
 * If we want to plug in a better pixmap image compression algorithm,
 * it would go here.
 */

#define NUM_PIXMAP_METHODS 0
LbxPixmapCompMethod LbxPixmapCompMethods [1]; /* dummy */

#else

LbxPixmapCompMethod
LbxPixmapCompMethods [] = {
  {
    "XC-PackBits",		/* compression method name */
    1 << ZPixmap,		/* formats supported */
    1, {8},			/* depths supported */
    0,				/* inited */
    -1,				/* method opcode - to be filled on reply */
    NULL,			/* init function */
    LbxImageEncodePackBits,	/* encode function */
    LbxImageDecodePackBits	/* decode function */
  }
};

#define NUM_PIXMAP_METHODS \
	(sizeof (LbxPixmapCompMethods) / sizeof (LbxPixmapCompMethod))
#endif



static int
LbxBitmapCompReq (buf)

unsigned char *buf;

{
    unsigned char *bufStart = buf;
    int i;

    *buf++ = NUM_BITMAP_METHODS;

    for (i = 0; i < NUM_BITMAP_METHODS; i++)
    {
	int len = strlen (LbxBitmapCompMethods[i].methodName);
	*buf++ = len;
	memcpy (buf, LbxBitmapCompMethods[i].methodName, len);
	buf += len;
    }

    return (buf - bufStart);
}

/*ARGSUSED*/
static int
LbxBitmapCompReply (preply, replylen)

unsigned char *preply;
int	      replylen;

{
    int count = *preply++;
    int i;

    lbxNegOpt.numBitmapCompMethods = count;

    if (count > 0)
    {
	lbxNegOpt.bitmapCompMethods = (char *) xalloc (count);
	if (lbxNegOpt.bitmapCompMethods == NULL)
	{
	    lbxNegOpt.numBitmapCompMethods = 0;
	    return -1;
	}
    }

    for (i = 0; i < count; i++)
    {
	int index = *preply++;
	lbxNegOpt.bitmapCompMethods[i] = index;
	LbxBitmapCompMethods[index].methodOpCode = *preply++;
    }

    return 0;
}


static int
LbxPixmapCompReq (buf)

unsigned char *buf;

{
    unsigned char *bufStart = buf;
    int i, j;

    *buf++ = NUM_PIXMAP_METHODS;

    for (i = 0; i < NUM_PIXMAP_METHODS; i++)
    {
	int len = strlen (LbxPixmapCompMethods[i].methodName);
	*buf++ = len;
	memcpy (buf, LbxPixmapCompMethods[i].methodName, len);
	buf += len;
	*buf++ = LbxPixmapCompMethods[i].formatMask;
	*buf++ = LbxPixmapCompMethods[i].depthCount;
	for (j = 0; j < LbxPixmapCompMethods[i].depthCount; j++)
	    *buf++ = LbxPixmapCompMethods[i].depths[j];
    }

    return (buf - bufStart);
}

/*ARGSUSED*/
static int
LbxPixmapCompReply (preply, replylen)

unsigned char *preply;
int	      replylen;

{
    int count = *preply++;
    int i, j;

    lbxNegOpt.numPixmapCompMethods = count;

    if (count > 0)
    {
	lbxNegOpt.pixmapCompMethods = (char *) xalloc (count);
	if (lbxNegOpt.pixmapCompMethods == NULL)
	{
	    lbxNegOpt.numPixmapCompMethods = 0;
	    return -1;
	}
    }

    for (i = 0; i < count; i++)
    {
	int index = *preply++;
	lbxNegOpt.pixmapCompMethods[i] = index;
	LbxPixmapCompMethods[index].methodOpCode = *preply++;
	LbxPixmapCompMethods[index].formatMask = *preply++;
	LbxPixmapCompMethods[index].depthCount = *preply++;
	for (j = 0; j < LbxPixmapCompMethods[index].depthCount; j++)
	    LbxPixmapCompMethods[index].depths[j] = *preply++;
    }

    return 0;
}


LbxBitmapCompMethod *
LbxLookupBitmapCompMethod (methodOpCode)

int methodOpCode;

{
    int i;

    for (i = 0; i < lbxNegOpt.numBitmapCompMethods; i++)
    {
	LbxBitmapCompMethod *method;

	method = &LbxBitmapCompMethods[lbxNegOpt.bitmapCompMethods[i]];

	if (method->methodOpCode == methodOpCode)
	    return (method);
    }

    return (NULL);
}


LbxPixmapCompMethod *
LbxLookupPixmapCompMethod (methodOpCode)

int methodOpCode;

{
    int i;

    for (i = 0; i < lbxNegOpt.numPixmapCompMethods; i++)
    {
	LbxPixmapCompMethod *method;

	method = &LbxPixmapCompMethods[lbxNegOpt.pixmapCompMethods[i]];

	if (method->methodOpCode == methodOpCode)
	    return (method);
    }

    return (NULL);
}


LbxBitmapCompMethod *
LbxFindPreferredBitmapCompMethod ()

{
    if (lbxNegOpt.numBitmapCompMethods == 0)
	return NULL;
    else
	return (&LbxBitmapCompMethods[lbxNegOpt.bitmapCompMethods[0]]);
}


LbxPixmapCompMethod *
LbxFindPreferredPixmapCompMethod (format, depth)

int format;
int depth;

{
    if (lbxNegOpt.numPixmapCompMethods == 0)
	return NULL;
    else
    {
	LbxPixmapCompMethod *method;
	int i, j;

	for (i = 0; i < lbxNegOpt.numPixmapCompMethods; i++)
	{
	    method = &LbxPixmapCompMethods[lbxNegOpt.pixmapCompMethods[i]];

	    if ((method->formatMask & (1 << format)))
	    {
		for (j = 0; j < method->depthCount; j++)
		    if (depth == method->depths[j])
			return method;
	    }
	}

	return NULL;
    }
}

typedef struct _LbxCmapAllMethod {
    char *methodName;
    void (*resolver)(
#if NeedFunctionPrototypes
	LbxVisualPtr /* pVisual */,
	CARD16* /* red */,
	CARD16* /* green */,
	CARD16* /* blue */
#endif
    );
    Pixel (*findfree)(
#if NeedFunctionPrototypes
	ColormapPtr /* pmap */,
	CARD32 	/* red */,
	CARD32	/* green */,
	CARD32	/* blue */
#endif
    );
    Entry * (* findbest)(
#if NeedFunctionPrototypes
	ColormapPtr /* pmap */,
	CARD32 	/* red */,
	CARD32	/* green */,
	CARD32	/* blue */,
	int	/* channels */
#endif
	);

} LbxCmapAllMethod;

LbxCmapAllMethod
LbxCmapAllMethods [] = {
  {
    "XC-CMAP",			/* colormap allocation method name */
    ResolveColor,		/* rgb resolver */
    FindFreePixel,		/* allocator */
    FindBestPixel,		/* matcher */
  }
};

#define NUM_CMAP_METHODS \
	(sizeof (LbxCmapAllMethods) / sizeof (LbxCmapAllMethod))

static int
LbxCmapAllReq (buf)

unsigned char *buf;

{
    unsigned char *bufStart = buf;
    int i;

    *buf++ = NUM_CMAP_METHODS;

    for (i = 0; i < NUM_CMAP_METHODS; i++)
    {
	int len = strlen (LbxCmapAllMethods[i].methodName);
	*buf++ = len;
	memcpy (buf, LbxCmapAllMethods[i].methodName, len);
	buf += len;
    }

    return (buf - bufStart);
}

static int
LbxCmapAllReply(preply, replylen)
    unsigned char *preply;
    int		  replylen;
{
    if (replylen < 1)
	return -1;
    if (*preply < NUM_CMAP_METHODS) {
	LbxResolveColor = LbxCmapAllMethods[*preply].resolver;
	LbxFindFreePixel = LbxCmapAllMethods[*preply].findfree;
	LbxFindBestPixel = LbxCmapAllMethods[*preply].findbest;
    }
    return 0;
}
