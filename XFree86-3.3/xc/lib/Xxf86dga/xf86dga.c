/* $XFree86: xc/lib/Xxf86dga/XF86DGA.c,v 3.11 1997/01/18 06:52:20 dawes Exp $ */
/*

Copyright (c) 1995  Jon Tombs
Copyright (c) 1995,1996  The XFree86 Project, Inc

*/

/* THIS IS NOT AN X CONSORTIUM STANDARD */

#ifdef __EMX__ /* needed here to override certain constants in X headers */
#define INCL_DOS
#define INCL_DOSIOCTL
#include <os2.h>
#endif

#define NEED_EVENTS
#define NEED_REPLIES
#include "Xlibint.h"
#include "xf86dgastr.h"
#include "Xext.h"
#include "extutil.h"

static XExtensionInfo _xf86dga_info_data;
static XExtensionInfo *xf86dga_info = &_xf86dga_info_data;
static char *xf86dga_extension_name = XF86DGANAME;

#define XF86DGACheckExtension(dpy,i,val) \
  XextCheckExtension (dpy, i, xf86dga_extension_name, val)

/*****************************************************************************
 *                                                                           *
 *			   private utility routines                          *
 *                                                                           *
 *****************************************************************************/

static int close_display();
static /* const */ XExtensionHooks xf86dga_extension_hooks = {
    NULL,				/* create_gc */
    NULL,				/* copy_gc */
    NULL,				/* flush_gc */
    NULL,				/* free_gc */
    NULL,				/* create_font */
    NULL,				/* free_font */
    close_display,			/* close_display */
    NULL,				/* wire_to_event */
    NULL,				/* event_to_wire */
    NULL,				/* error */
    NULL,				/* error_string */
};

static XEXT_GENERATE_FIND_DISPLAY (find_display, xf86dga_info, 
				   xf86dga_extension_name, 
				   &xf86dga_extension_hooks, 
				   0, NULL)

static XEXT_GENERATE_CLOSE_DISPLAY (close_display, xf86dga_info)


/*****************************************************************************
 *                                                                           *
 *		    public XFree86-DGA Extension routines                *
 *                                                                           *
 *****************************************************************************/

Bool XF86DGAQueryExtension (dpy, event_basep, error_basep)
    Display *dpy;
    int *event_basep, *error_basep;
{
    XExtDisplayInfo *info = find_display (dpy);

    if (XextHasExtension(info)) {
	*event_basep = info->codes->first_event;
	*error_basep = info->codes->first_error;
	return True;
    } else {
	return False;
    }
}

Bool XF86DGAQueryVersion(dpy, majorVersion, minorVersion)
    Display* dpy;
    int* majorVersion; 
    int* minorVersion;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGAQueryVersionReply rep;
    xXF86DGAQueryVersionReq *req;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGAQueryVersion, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGAQueryVersion;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }
    *majorVersion = rep.majorVersion;
    *minorVersion = rep.minorVersion;
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86DGAGetVideoLL(dpy, screen, offset, width, bank_size, ram_size)
    Display* dpy;
    int screen;
    int *offset;
    int *width, *bank_size, *ram_size;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGAGetVideoLLReply rep;
    xXF86DGAGetVideoLLReq *req;
    int i;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGAGetVideoLL, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGAGetVideoLL;
    req->screen = screen;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }

    *offset = /*(char *)*/rep.offset;
    *width = rep.width;
    *bank_size = rep.bank_size;
    *ram_size = rep.ram_size;
	
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

    
Bool XF86DGADirectVideoLL(dpy, screen, enable)
    Display* dpy;
    int screen;
    int enable;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGADirectVideoReq *req;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGADirectVideo, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGADirectVideo;
    req->screen = screen;
    req->enable = enable;
    UnlockDisplay(dpy);
    SyncHandle();
    XSync(dpy,False);
    return True;
}

Bool XF86DGAGetViewPortSize(dpy, screen, width, height)
    Display* dpy;
    int screen;
    int *width, *height;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGAGetViewPortSizeReply rep;
    xXF86DGAGetViewPortSizeReq *req;
    int i;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGAGetViewPortSize, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGAGetViewPortSize;
    req->screen = screen;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }

    *width = rep.width;
    *height = rep.height;
	
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}
    
    
Bool XF86DGASetViewPort(dpy, screen, x, y)
    Display* dpy;
    int screen;
    int x, y;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGASetViewPortReq *req;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGASetViewPort, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGASetViewPort;
    req->screen = screen;
    req->x = x;
    req->y = y;
    UnlockDisplay(dpy);
    SyncHandle();
    XSync(dpy,False);
    return True;
}

    
Bool XF86DGAGetVidPage(dpy, screen, vpage)
    Display* dpy;
    int screen;
    int *vpage;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGAGetVidPageReply rep;
    xXF86DGAGetVidPageReq *req;
    int i;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGAGetVidPage, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGAGetVidPage;
    req->screen = screen;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }

    *vpage = rep.vpage;
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

    
Bool XF86DGASetVidPage(dpy, screen, vpage)
    Display* dpy;
    int screen;
    int vpage;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGASetVidPageReq *req;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGASetVidPage, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGASetVidPage;
    req->screen = screen;
    req->vpage = vpage;
    UnlockDisplay(dpy);
    SyncHandle();
    XSync(dpy,False);
    return True;
}

Bool XF86DGAInstallColormap(dpy, screen, cmap)
Display* dpy;
int screen;
Colormap cmap;
{
   XExtDisplayInfo *info = find_display (dpy);
   xXF86DGAInstallColormapReq *req;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGAInstallColormap, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGAInstallColormap;
    req->screen = screen;
    req->id = cmap;
    UnlockDisplay(dpy);
    SyncHandle();
    XSync(dpy,False);
    return True;
}

Bool XF86DGAQueryDirectVideo(dpy, screen, flags)
    Display *dpy;
    int screen;
    int *flags;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGAQueryDirectVideoReply rep;
    xXF86DGAQueryDirectVideoReq *req;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGAQueryDirectVideo, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGAQueryDirectVideo;
    req->screen = screen;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }
    *flags = rep.flags;
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86DGAViewPortChanged(dpy, screen, n)
    Display *dpy;
    int screen;
    int n;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86DGAViewPortChangedReply rep;
    xXF86DGAViewPortChangedReq *req;

    XF86DGACheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86DGAViewPortChanged, req);
    req->reqType = info->codes->major_opcode;
    req->dgaReqType = X_XF86DGAViewPortChanged;
    req->screen = screen;
    req->n = n;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return rep.result;
}



/* Helper functions */

#if 0
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xmu/StdSel.h>
#endif
#include <X11/Xmd.h>
#include <X11/extensions/xf86dga.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#if defined(ISC) 
# define HAS_SVR3_MMAP
# include <sys/types.h>
# include <errno.h>

# include <sys/at_ansi.h>
# include <sys/kd.h>

# include <sys/sysmacros.h>
# include <sys/immu.h>
# include <sys/region.h>

# include <sys/mmap.h>
#else
# if !defined(Lynx)
#  if !defined(__EMX__)
#   include <sys/mman.h>
#  endif
# else
#  include <sys/types.h>
#  include <errno.h>
#  include <smem.h>
# endif
#endif
#include <sys/wait.h>
#include <signal.h>
extern int errno;

#if defined(SVR4) && !defined(sun) && !defined(SCO325)
#define DEV_MEM "/dev/pmem"
#else
#define DEV_MEM "/dev/mem"
#endif

#if defined(ISC) && defined(HAS_SVR3_MMAP)
struct kd_memloc XFree86mloc;
#endif

static char * _XFree86addr = NULL;
static int    _XFree86size = 0;

/*
 * Still need to find a clean way of detecting the death of a DGA app
 * and returning things to normal - Jon
 * This is here to help debugging without rebooting... Also C-A-BS
 * should restore text mode.
 */

int XF86DGAForkApp(int screen)
{
     pid_t pid;
     int status;

     /* fork the app, parent hangs around to clean up */
     if ((pid = fork()) > 0) {
        Display *disp;

	waitpid(pid, &status, 0);
	disp = XOpenDisplay(NULL);
	XF86DGADirectVideo(disp, screen, 0);
	XSync(disp,False);
        if (WIFEXITED(status))
	    _exit(0);
	else
	    _exit(-1);
     }
     return pid;
}

XF86DGADirectVideo(dis, screen, enable)
Display *dis;
int screen;
int enable;
{
   if (enable&XF86DGADirectGraphics) {
	 fprintf(stderr, "video memory unprotecting\n");
#if !defined(ISC) && !defined(HAS_SVR3_MMAP) && !defined(Lynx) && !defined(__EMX__)
      if (_XFree86addr && _XFree86size)
         if (mprotect(_XFree86addr,_XFree86size, PROT_READ|PROT_WRITE)) {
         fprintf(stderr, "XF86DGADirectVideo: mprotect (%s)\n",
                           strerror(errno));
		exit (-3);
	 }
#endif
   } else {
      if (_XFree86addr && _XFree86size)
	 fprintf(stderr, "video memory protecting\n");
#if !defined(ISC) && !defined(HAS_SVR3_MMAP) && !defined(__EMX__)
#ifndef Lynx
         if (mprotect(_XFree86addr,_XFree86size, PROT_READ)) {
         fprintf(stderr, "XF86DGADirectVideo: mprotect (%s)\n",
                           strerror(errno));
		exit (-4);
	 }
#else
	smem_create(NULL, _XFree86addr, _XFree86size, SM_DETACH);
	smem_remove("XF86DGA");
#endif
#endif
   }
   XF86DGADirectVideoLL(dis, screen, enable);
}


static void XF86cleanup(int sig)
{
        Display *disp;
	static beenhere = 0;

	if (beenhere)
		_exit(3);
	beenhere = 1;
	disp = XOpenDisplay(NULL);
	XF86DGADirectVideo(disp, 0, 0);
	XSync(disp,False);
        _exit(3);
}

XF86DGAGetVideo(dis, screen, addr, width, bank, ram)
Display *dis;
int screen;
char **addr;
int *width, *bank, *ram;
{
   int offset, fd;
#ifdef __EMX__
   APIRET rc;
   ULONG action;
   HFILE hfd;
#endif

   XF86DGAGetVideoLL(dis, screen , &offset, width, bank, ram);

#ifndef Lynx
#if defined(ISC) && defined(HAS_SVR3_MMAP)
   if ((fd = open("/dev/mmap", O_RDWR)) < 0)
   {
        fprintf(stderr, "XF86DGAGetVideo: failed to open /dev/mmap (%s)\n",
                           strerror(errno));
#else
#ifdef __EMX__
   /* Dragon warning here! /dev/pmap$ is never closed, except on progam exit.
    * Consecutive calling of this routine will make PMAP$ driver run out
    * of memory handles. Some umap/close mechanism should be provided
    */

   rc = DosOpen("/dev/pmap$", &hfd, &action, 0, FILE_NORMAL, FILE_OPEN,
		OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE, (PEAOP2)NULL);
   if (rc != 0) {
	fprintf(stderr, 
		"XF86DGAGetVideo: failed to open /dev/pmap$ (rc=%d)\n",
		rc);
#else
   if ((fd = open(DEV_MEM, O_RDWR)) < 0)
   {
        fprintf(stderr, "XF86DGAGetVideo: failed to open %s (%s)\n",
                           DEV_MEM, strerror(errno));
#endif
#endif
        exit (-1);
   }
#endif

#if defined(ISC) && defined(HAS_SVR3_MMAP)
   XFree86mloc.vaddr=(char *) 0;
   XFree86mloc.physaddr=(char *)offset;
   XFree86mloc.length=*bank;
   XFree86mloc.ioflg=1;

#define DEBUG_MMAP(state)     fprintf(stderr,"\
        XF86DGAGetVideo: (%s) \n\
            vaddr=%x/%x physaddr=%x/%x length=%d/%d\n",\
        (state) ,\
        XFree86mloc.vaddr,*addr,\
        XFree86mloc.physaddr,offset,\
        XFree86mloc.length,*bank)

   DEBUG_MMAP("before MMAP");
   if ((*addr = (char *) ioctl( fd, MAP, &XFree86mloc)) != (char *)-1) {
     offset=(int)XFree86mloc.physaddr;
     *bank=(int)XFree86mloc.length;
     DEBUG_MMAP("after MMAP");
   } else {
     DEBUG_MMAP("after MMAP");
     fprintf(stderr, "XF86DGAGetVideo: failed to mmap /dev/mmap (%s)\n",
                           strerror(errno));
#else /* !ISC */
#ifdef Lynx
   *addr = (void *)smem_create("XF86DGA", (char *)offset, *bank, SM_READ|SM_WRITE);
   if (*addr == NULL) {
        fprintf(stderr, "XF86DGAGetVideo: smem_create() failed (%s)\n",
                           strerror(errno));
#else /* !Lynx */
#ifdef __EMX__
   {
	struct map_ioctl {
		union {
			ULONG phys;
			void* user;
		} a;
		ULONG size;
	} pmap,dmap;
	ULONG plen,dlen;
#define XFREE86_PMAP	0x76
#define PMAP_MAP	0x44

	pmap.a.phys = offset;
	pmap.size = *bank;
	rc = DosDevIOCtl(hfd, XFREE86_PMAP, PMAP_MAP,
			 (PULONG)&pmap, sizeof(pmap), &plen,
			 (PULONG)&dmap, sizeof(dmap), &dlen);
	if (rc==0) {
		*addr = dmap.a.user;
	}
   }
   if (rc != 0) {
        fprintf(stderr, 
		"XF86DGAGetVideo: failed to mmap /dev/pmap$ (rc=%d)\n",
                rc);
#else /* !__EMX__ */
#ifndef MAP_FILE
#define MAP_FILE 0
#endif
   /* This requires linux-0.99.pl10 or above */
   *addr = (void *)mmap(NULL, *bank, PROT_READ,
                        MAP_FILE | MAP_SHARED, fd, (off_t)offset);
#ifdef DEBUG
   fprintf(stderr, "XF86DGAGetVideo: physaddr: 0x%08x, size: %d\n",
	   (long)offset, *bank);
#endif
   if (*addr == (char *) -1) {
        fprintf(stderr, "XF86DGAGetVideo: failed to mmap %s (%s)\n",
                           DEV_MEM, strerror(errno));
#endif /* !__EMX__*/
#endif /* !Lynx */
#endif /* !ISC && !HAS_SVR3_MMAP */
        exit (-2);
   }
   _XFree86size = *bank;
   _XFree86addr = *addr;

   
   atexit((void(*)(void))XF86cleanup);
   /* one shot XF86cleanup attempts */
   signal(SIGSEGV, XF86cleanup);
#ifdef SIGBUS
   signal(SIGBUS, XF86cleanup);
#endif
   signal(SIGHUP, XF86cleanup);
   signal(SIGFPE, XF86cleanup);  

   return 1;
}
