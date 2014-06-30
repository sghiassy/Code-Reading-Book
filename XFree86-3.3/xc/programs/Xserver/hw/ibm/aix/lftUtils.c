/*
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#ifdef _IBM_LFT
#include <fcntl.h>
#include <sys/lft_ioctl.h>
#include <sys/cfgodm.h>
#include <sys/cfgdb.h>
#include "ibmTrace.h"
#include "lftUtils.h"


static lft_disp_query_t lfq_disp;

char *get_rcm_path()
{
     char   *rcmpath;
     rcmpath = (char *) malloc(strlen("/dev/rcm0") + 1);
     strcpy(rcmpath, "/dev/rcm0");
     return(rcmpath);
}

static int rcm_fd = -1;

int get_rcm_fd() 
{
    char *rcm_path;

	if(rcm_fd <0) {
		rcm_path = get_rcm_path();
		if(rcm_path) {
			rcm_fd = open(rcm_path, O_RDWR);
		}
		else return -1;
	}
	else return rcm_fd;
}




int
init_lft_disp_info()
{
    int             fd;
    lft_query_t     lfq;
    int		    i;
    int		    rc;
    extern int 	    aixForceFlag;

    TRACE(("init_lft_disp_info\n"));

    if(aixForceFlag) {
    	fd = open("/dev/lft0", O_RDONLY);
    }
    else {
    	fd = open("/dev/tty", O_RDONLY);
    }
    if(fd <0) {
	ibmInfoMsg("init_lft_disp_info : Couldnot open /dev/tty\n");
	return FALSE;
    }
    rc = ioctl(fd, LFT_QUERY_LFT, &lfq);
    if(rc < 0) {
	close(fd);
	ibmInfoMsg("init_lft_disp_info : ioctl LFT_QUERY_LFT failed\n");
	return FALSE;
    }
    lfq_disp.lft_disp = (lft_disp_info_t*)malloc(
		sizeof(lft_disp_info_t)* lfq.number_of_displays);
    lfq_disp.num_of_disps = lfq.number_of_displays;
    rc = ioctl(fd, LFT_QUERY_DISP, &lfq_disp);
    if(rc < 0) {
	ibmInfoMsg("init_lft_disp_info : ioctl LFT_QUERY_DISP failed\n");
    	close(fd);
	return FALSE;
    }
    close(fd);
    return TRUE;
}

char *get_disp_name_by_number(anumber)
int anumber; /*  0,1,2..*/
{
     extern lft_disp_query_t lfq_disp;

	if((anumber >= lfq_disp.num_of_disps) || (anumber <0))
		return  NULL;
	if(lfq_disp.lft_disp[anumber].disp_enable)
		return lfq_disp.lft_disp[anumber].disp_name ;
	else 
		return NULL;
}

/* Currently we support only SKYWAY,
 * Add other devices here
 */
unsigned long get_lft_disp_id(anumber)
int anumber;
{
	char *disp_name;
	disp_name = get_disp_name_by_number(anumber);
	if(!disp_name)
		return 0xffffffff;
	else if(!strncmp(disp_name, "gda", 3)) {
		return SKYWAY_ID ;
	}
	else 
		return 0xffffffff;
	
}
int get_num_lft_displays()
{
    return lfq_disp.num_of_disps;
}

#endif 
