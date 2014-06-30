/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/OS_Mach.c,v 3.4.2.1 1997/05/06 13:24:19 dawes Exp $ */
/*
 * (c) Copyright 1993,1994 by Robert V. Baron 
 *			<Robert.Baron@ernst.mach.cs.cmu.edu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * ROBERT V. BARON  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of Robert V. Baron shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Robert V. Baron.
 *
 */
/* $XConsortium: OS_Mach.c /main/5 1996/02/21 17:11:12 kaleb $ */

#include "Probe.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

/* These can be Mach 2.5 or 3.0 headers */
#include <mach.h>
#include <mach_error.h>
#include <mach/message.h>
#ifdef MACH_PORT_NULL
#define MACH3		/* they must have been 3.0 headers beckerd@cs.unc.edu */
#endif
#undef task_self

static int IOPL_fd = -1;

/*
 * OpenVideo --
 *
 * Enable access to the installed video hardware.  For Mach, we disable
 * IO protection, since this is currently the only way to access any
 * IO registers.
 */
int screen_addr;

int OpenVideo()
{
	int ret;
#define C /*Bios_Base*/ 0xc0000
#define S 0x40000

	if ((IOPL_fd = open("/dev/iopl", O_RDWR, 0)) < 0) {
		fprintf(stderr, "Failed to open /dev/iopl\n");
		return -1;
	}
#define KERN_SUCESS 0
	if (KERN_SUCESS != vm_allocate(task_self(), &screen_addr, S, TRUE)) {
		fprintf(stderr, "Failed vmallocate %x\n", S);
		close(IOPL_fd);
		return -1;
	}
	if (mmap(screen_addr+C, S, 3, 1, IOPL_fd, C) < 0) {
		fprintf(stderr, "Failed to mmap %x bytes at %x\n", S, C);
		vm_deallocate(task_self(), screen_addr, S);
		close(IOPL_fd);
		return -1;
	}
	return(IOPL_fd);
}

/*
 * CloseVideo --
 *
 * Disable access to the video hardware.  For Mach, we re-enable
 * IO protection.
 */
void CloseVideo()
{
	if (KERN_SUCESS != vm_deallocate(task_self(), screen_addr, S)) {
		fprintf(stderr, "Failed vmdeallocate %x\n", S);
	}
	close(IOPL_fd);
}

/*
 * MapVGA --
 *
 * Map the VGA memory window (0xA0000-0xAFFFF) as read/write memory for
 * the process for use in probing memory.
 */
Byte *MapVGA()
{
        return((Byte *)0);
}

Byte *MapMem(address,size)
	unsigned long address;
	unsigned long size;
{
	return((Byte *)0);
}

/*
 * UnMapVGA --
 *
 * Unmap the VGA memory window.
 */
void UnMapVGA(base)
Byte *base;
{
	return;
}

void UnMapMem(base,size)
Byte *base;
unsigned long size;
{
	return;
}

/*
 * ReadBIOS --
 *
 * Read 'Len' bytes from the video BIOS at address 'Bios_Base'+'Offset' into 
 * buffer 'Buffer'.
 */
int ReadBIOS(Offset, Buffer, Len)
unsigned Offset;
Byte *Buffer;
int Len;
{
	Word tmp;
	Byte *Base = (Byte *)(screen_addr + Bios_Base + Offset);

#ifdef	DEBUG
	{int i;
		fprintf(stderr, "ReadBIOS(Offset %x, Buffer %x, Len %x) .. ",
			Offset, Buffer, Len);
		for (i=0;i<Len;i++)
			fprintf(stderr," [%c](%x)|", *(Base+i), *(Base+i));
		fprintf(stderr,"\n");
	}
#endif
	bcopy(Base, Buffer, Len);
	return (Len);
}

/*
 * EnableIOPort --
 *
 * Enable access to 'NumPorts' IO ports listed in array 'Ports'.  For Mach, 
 * we've disabled IO protections so this is a no-op.
 */
/*ARGSUSED*/
int EnableIOPorts(NumPorts, Ports)
CONST int NumPorts;
CONST Word *Ports;
{
	return(0);
}

/*
 * DisableIOPort --
 *
 * Disable access to 'NumPorts' IO ports listed in array  'Ports'.  For Mach, 
 * we've disabled IO protections so this is a no-op.
 */
/*ARGSUSED*/
int DisableIOPorts(NumPorts, Port)
CONST int NumPorts;
CONST Word *Port;
{
	return(0);
}

/*
 * ShortSleep --
 *
 * Sleep for the number of milliseconds specified in 'Delay'.
 */
void ShortSleep(Delay)
int Delay;
{
#ifdef MACH3
	struct trial {
		mach_msg_header_t	h;
		mach_msg_type_t	t;
		int		d;
	} msg_rcv;
	mach_port_t		my_port;
#else
	/* This does Mach 2.5 IPC */	
	struct trial {
		msg_header_t	h;
		msg_type_t	t;
		int		d;
	} msg_rcv;
	port_t		my_port;
#endif
	kern_return_t	error;
	int		ret;

	if ((error = port_allocate(task_self(), &my_port)) != KERN_SUCCESS) {
		printf("ShortSleep: port_allocate failed with %d: %s\n", 
			error, mach_error_string(error));
		return;
	}
	
#ifdef MACH3
	msg_rcv.h.msgh_size = sizeof(msg_rcv);
	if ((ret = msg_receive(&msg_rcv.h, MACH_RCV_TIMEOUT,	Delay)) != MACH_RCV_TIMED_OUT) {
#else
	if ((ret = msg_receive(&msg_rcv.h, RCV_TIMEOUT,	Delay)) != RCV_TIMED_OUT) {
	msg_rcv.h.msg_local_port = my_port;
#endif
		mach_error("ShortSleep: msg_receive returned ", ret);
		return;
	}

	if ((error = port_deallocate(task_self(), my_port)) != KERN_SUCCESS) {
		printf("ShortSleep: port_deallocate failed with %d: %s\n", 
			error, mach_error_string(error));
		return;
	}
}
