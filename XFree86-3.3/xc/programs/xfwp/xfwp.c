/* $XConsortium: xfwp.c /main/38 1996/12/27 16:18:32 kaleb $ */

/*
Copyright (c) 1996  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and sell copies of the Software, and to
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

/*
  Author: Reed A. Augliere
  under contract to X Consortium, Inc.
 */

/* $XFree86: xc/programs/xfwp/xfwp.c,v 1.7 1997/01/18 07:02:52 dawes Exp $ */

#if defined(sun) && defined(i386) && defined(SVR4)
#define __EXTENSIONS__
#endif

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xos.h>
/* Xos.h takes care of ...
   #include <string.h>
   #include <sys/types.h>
   #include <unistd.h>
   #include <sys/time.h>
 */
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef Lynx
#include <sys/socket.h>
#else
#include <socket.h>
#endif
#include <sys/wait.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#ifndef Lynx
#include <sys/param.h>
#endif
#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/Xfuncs.h>
#include <X11/Xmd.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/ICE/ICElib.h>
#include <X11/ICE/ICEconn.h>
#include <X11/ICE/ICEmsg.h>
#include <X11/ICE/ICEproto.h>
#include <X11/Xpoll.h>
#include <X11/PM/PM.h>
#include <X11/PM/PMproto.h>
#include <assert.h>
#include "xfwp.h"

#ifndef SOMAXCONN
#define SOMAXCONN 128
#endif

#ifdef __EMX__
typedef unsigned short ushort;
#else
extern int errno;
#endif

struct clientDataStruct global_data;  /* necessary for ICE callbacks */
struct pm_conn_buf *  	pm_conn_array[MAX_PM_CONNS];
struct server_list * 	server_array[MAX_SERVERS];
struct listen_port_info	port_array[MAX_CLIENT_LISTEN_PORTS];
/*
// what FWP returns to clients who are denied access; put here
// to make changing them convenient 
*/
char * server_reason[2] = {"Authentication rejected", "permission denied"}; 

#define READPOLICYFROMFILE

#ifdef READPOLICYFROMFILE
char **SitePolicies    = NULL;	/* list of site security policy strings */
int    SitePolicyCount = 0;	/* count of elements in SitePolicies */
Bool   SitePolicyPermit = 0;	/* True  := permit iff server supports at
					    least one listed policy,
				   False := deny if server has any of the
					    listed policies.
				 */
Bool   HaveSitePolicy = 0;
#else
char *SitePolicies[]  = {"A", "B", "" };
int   SitePolicyCount = 3;
Bool  SitePolicyPermit = True;
#endif

Bool printConfigVerify = FALSE;

void doPrintEval(struct config*, int);

static void Usage()
{
    fprintf(stderr, "Usage:  xfwp [-pdt <#secs>] [-clt <#secs>] [-cdt <#secs>] [-pmport <port#>] [-config <path>] [-verify]\n");
    exit(0);
}

static void MallocFailed()
{
    fprintf(stderr, "Memory allocation failed, exiting\n");
    exit(1);
}

static char* Realloc(char *p, int s)
{
    if (!p)
	p = malloc(s);
    else
	p = realloc(p, s);

    if (!p)
	MallocFailed();

    return p;
}

static char* Malloc(int s)
{
    char *p = malloc(s);

    if (!p)
	MallocFailed();

    return p;	
}

main (int argc, char * argv[])
{
  struct clientDataStruct 	client_data;	
  struct sockaddr_in 		dest_server; 
  struct pm_policy 		policy;
  int 				pm_listen_array[MAX_TRANSPORTS];
  int 				pm_conn_counter;
  struct sockaddr_in		pm_sockaddr_in; 
  int   			rem_listen_counter; 
  struct sockaddr_in		rem_sockaddr_in;
  fd_set 			readable, writable, rinit, winit;
  int				nfds = 0;
  int 				nready = 0;
  int				config_num_conns;
  struct client_conn_buf * 	client_conn_array[MAX_CLIENT_CONNS];
  struct ice_data		ice_data;
  struct ICE_setup_info 	pm_conn_setup;
  IceListenObj *		listen_objects;
  ushort			server_port;
  struct config *		config_info;
  int				config_status = 0;
  /*
  // setup the global client data struct; we need to do this in order
  // to access program data in the ICE FWPprocessMessages() callback 
  // without making everything global!  See FWPprotocolSetupProc() for
  // the rest of what we are doing
  */
  config_info = (struct config *) Malloc(sizeof(struct config));

  global_data.config_info = config_info;
  global_data.nfds =  &nfds;
  global_data.rinit = &rinit;
  global_data.winit = &winit;
  /*
  // now do the rest of the setup
  */
  doProcessInputArgs(config_info, argc, argv);
  if ((config_status = doHandleConfigFile(config_info)) == FAILURE)
    exit(1);
  doInitDataStructs(config_info, 
		    client_conn_array, 
		    &ice_data,
		    &pm_conn_setup,
		    pm_listen_array);	
  /*
  // set up the ICE io error handler
  */
  doInstallIOErrorHandler();
  /*
  // zero the select() read/write masks
  */
  FD_ZERO(&readable);
  FD_ZERO(&writable);
  FD_ZERO(&rinit);
  FD_ZERO(&winit);
  /*
  // create FWP listener socket(s) for PM connection; set select()
  // read/write mask bits
  */
  doSetupPMListen(config_info->pm_listen_port, pm_listen_array, &listen_objects, 
		  &nfds, &rinit);
  /*
  // initialize counters of pm and remote listen connections
  */
  pm_conn_counter = 0;
  rem_listen_counter = 0;

  while(1)
  {
    readable = rinit;
    writable = winit;
    doSelect(config_info, &nfds, &nready, &readable, &writable);
    doCheckTimeouts(config_info, &nfds, &nready, &rinit, &winit,
		    &readable, &writable, 
		    client_conn_array); 
    doProcessSelect(&nfds, &nready, &readable, &writable, &rinit, &winit,
		    pm_listen_array, &pm_conn_counter, 
		    &rem_listen_counter, config_info, 
		    client_conn_array,
		    &ice_data, &listen_objects);
  } 
}


void  doProcessInputArgs(struct config * config_info, 
	                 int argc, 
                         char * argv[])
{
  int 	arg_counter;
  int	break_flag = 0;
  config_info->num_client_conns = MAX_CLIENT_CONNS;
  config_info->num_pm_conns = MAX_PM_CONNS;
  config_info->num_servers = MAX_SERVERS;
  config_info->num_transports = MAX_TRANSPORTS;
  /*
  // initialize timeout values to zero here
  */
  config_info->pm_data_timeout = 0;
  config_info->client_listen_timeout = 0;
  config_info->client_data_timeout = 0;

  config_info->rule_count = config_info->lines_allocated = 0;
  config_info->config_file_data = NULL;

  config_info->config_file_path = NULL;
  
  /*
  // initialize timeout for three port types; if a timeout for a 
  // particular port type (pmdata, clientlisten, clientdata) is 
  // not specified explicitly, then it assumes the hard-coded
  // default value; initialize other command line options here
  // as well
  */
  for (arg_counter = 1; arg_counter < argc; arg_counter++)
  {
    if (argv[arg_counter][0] == '-')
    {
      if (!strcmp("-pdt", argv[arg_counter]))
      {
        if (arg_counter + 1 == argc)
	{
          break_flag = 1; 
	  break;
        }
	config_info->pm_data_timeout = atoi(argv[arg_counter + 1]);
      }
      else if (!strcmp("-clt", argv[arg_counter]))
      {
        if (arg_counter + 1 == argc)
        {
	  break_flag = 1;
          break; 
        }
	config_info->client_listen_timeout = atoi(argv[arg_counter + 1]);
      }
      else if (!strcmp("-cdt", argv[arg_counter]))
      {
        if (arg_counter + 1 == argc)
        {
          break_flag = 1;
          break; 
        }
	config_info->client_data_timeout = atoi(argv[arg_counter + 1]);
      }
      else if (!strcmp("-pmport", argv[arg_counter]))
      {
        if (arg_counter + 1 == argc)
        {
          break_flag = 1; 
          break;
        }
	if (atoi(argv[arg_counter + 1]) > 65536)
        {
          break_flag = 1;
	  break;     
        }
	strcpy(config_info->pm_listen_port, argv[arg_counter + 1]);
      }
      else if (!strcmp("-config", argv[arg_counter]))
      {
        if (arg_counter + 1 == argc)
        {
          break_flag = 1;
          break; 
        }
	config_info->config_file_path = Malloc(strlen(argv[arg_counter+1])+1);
	strcpy(config_info->config_file_path, argv[arg_counter + 1]);
      }
      else if (!strcmp("-verify", argv[arg_counter]))
      {
	  printConfigVerify = TRUE;
      }
      else
      {
        fprintf(stderr, "Unrecognized command line argument.\n");
	Usage();
      }
    }
  } 
  if (break_flag)
      Usage();

  /*
  // check timeout values; if still zero then apply defaults
  */
  if (config_info->pm_data_timeout <= 0)
    config_info->pm_data_timeout = PM_DATA_TIMEOUT_DEFAULT;
  if (config_info->client_listen_timeout <= 0)
    config_info->client_listen_timeout = CLIENT_LISTEN_TIMEOUT_DEFAULT;
  if (config_info->client_data_timeout <= 0)
    config_info->client_data_timeout = CLIENT_DATA_TIMEOUT_DEFAULT;
  if (strlen(config_info->pm_listen_port) == 0)
    strcpy(config_info->pm_listen_port, PM_LISTEN_PORT);
}


void doInitDataStructs(struct config * config_info, 
                      struct client_conn_buf * client_conn_array[],
		      struct ice_data * ice_data,
		      struct ICE_setup_info * pm_conn_setup,
		      int pm_listen_array[])
	
{
  int i;
  char * temp_name = "exert.x.org";
  /*
  // Initialize select() timeout; start with a high value, which will
  // be overridden by the minimum timeout value of all fd's taken
  // together;this heuristic allows us to block inside select()
  // as much as possible (avoiding CPU spin cycles), as well as
  // to periodically check timeouts on open ports and thereby recover
  // them
  */
  config_info->select_timeout.tv_usec = 0;
  config_info->select_timeout.tv_sec = 180000; 
  /*	
  // load defines to config struct
  */
  for (i = 0; i < config_info->num_client_conns; i++)
    client_conn_array[i] = NULL;
  for (i = 0; i < config_info->num_pm_conns; i++)
    pm_conn_array[i] = NULL;
  for (i = 0; i < config_info->num_servers; i++)
    server_array[i] = NULL;
  for (i = 0; i < config_info->num_transports; i++)
    pm_listen_array[i] = -1;
  /*  ice_data->server_host_name = NULL;
  //
  // init server name to test PM handling only  
  */
  ice_data->server_host_name = (char *) malloc (strlen(temp_name) + 1);
  strcpy(ice_data->server_host_name, temp_name);
  for (i = 0; i < MAX_CLIENT_LISTEN_PORTS; i++)
    port_array[i].state = AVAILABLE;
  /*
  // init ICE connection setup data
  */
  pm_conn_setup->opcode = 0;
  pm_conn_setup->versionCount = 1;
  pm_conn_setup->PMVersions->major_version = 1;
  pm_conn_setup->PMVersions->minor_version = 0;
  pm_conn_setup->PMVersions->process_msg_proc = 
			(IcePaProcessMsgProc) FWPprocessMessages;
  /*
  // Register for protocol setup
  */
  if ((pm_conn_setup->opcode = IceRegisterForProtocolReply(
				"PROXY_MANAGEMENT",
				"XC",
				"1.0",
				pm_conn_setup->versionCount,
				pm_conn_setup->PMVersions,
				0, /* authcount */
				NULL, /* authname */
				NULL, /* authprocs */
				FWPHostBasedAuthProc, /* force non-auth */
				FWPprotocolSetupProc,
				NULL, /* protocol activate proc */
				NULL /* IceIOErrorProc */ )) < 0)
  {
    /*
    // Log this message?
    */
#ifdef DEBUG
    fprintf(stderr, "Could not register PROXY_MANAGEMENT ICE protocol.");
#endif
    return;
  }
  global_data.major_opcode = pm_conn_setup->opcode;
}

int   doSetupPMListen(char *  pm_port,
		      int pm_listen_array[],
		      IceListenObj ** listen_objects,
		      int * nfds,
		      fd_set * rinit)
{
  int 	num_fds_returned;
  char  errormsg[256];
  int   fd_counter;
  IceListenObj * temp_obj;
  /*
  // establish PM listeners
  */  
  if (!IceListenForWellKnownConnections(pm_port, 
					&num_fds_returned,
					listen_objects,
					256,
					errormsg))
  {
    /*
    // write this error to fwp logfile???
    */
#ifdef DEBUG
    fprintf(stderr, "%s\n", errormsg); 
#endif
    return;
  }
  /*
  // obtain the PM listen fd's for the connection objects 
  */
  for (fd_counter = 0; fd_counter < num_fds_returned; fd_counter++)
  {
    /*
    // get fd(s) for PM listen (could be more than one if different
    // transport mechanisms) 
    */ 
    temp_obj = *listen_objects;
    IceSetHostBasedAuthProc(temp_obj[fd_counter], FWPHostBasedAuthProc);
    pm_listen_array[fd_counter] = 
     		IceGetListenConnectionNumber(temp_obj[fd_counter]);
    /*
    // set all read mask bits on which we are going to select(); 
    // [NOTE:  We don't care about write bits here because we don't
    // use select() to manage writing to the PM] 
    */
    FD_SET(pm_listen_array[fd_counter], rinit);
    /*
    // compute nfds for select()
    */
    *nfds = max(*nfds, pm_listen_array[fd_counter] + 1);
  }
  return;
}

int   doSetupRemClientListen(char ** listen_port_string,
			     struct clientDataStruct * program_data,
			     char * server_address)
{
  int			this_server;
  int 			one = 1;
  struct sockaddr_in 	rem_sockaddr_in;
  int 			port_counter;
  int 			listen_port;
  char			port_buff[10];
  char *		server_name_base;
  int			msg_len;
  char *		tmp_server_hostport;
  int			status;
  char			hostname[MAX_HOSTNAME_LEN];
  struct timeval	time_val;
  struct timezone	time_zone;

  /* ugh.  This really shouldn't be kept as a sparse list but no time...*/
  for (this_server = 0;
       this_server < MAX_SERVERS && server_array[this_server] != NULL;
       this_server++);

  if (this_server == MAX_SERVERS)
  {
      fputs("Out of server slots\n", stderr);
      return FAILURE;
  }

  /*
  // find the next available client listen port
  */ 
  for (port_counter = 0; port_counter < MAX_CLIENT_LISTEN_PORTS; port_counter++)
  {
    if (port_array[port_counter].state == AVAILABLE)
    {
      port_array[port_counter].state = IN_USE;   
      break;
    } else if (port_counter == MAX_CLIENT_LISTEN_PORTS - 1) 
    {
      /*
      // No more listen ports available for allocation
      */ 
#ifdef DEBUG
      fprintf(stderr,"All listen ports in use.\n");
#endif
      return FAILURE;
    }

  }
  /*
  // offset listen port into the X protocol range; 
  // must be > 6000 < 6xxx (configurable?)
  */
  listen_port = port_counter + 6001;
  /*
  // allocate the server_array struct and init the fd elements;
  // can't use the PM connection fd as an index into this array, since
  // there could be multiple servers per PM connection
  */
  if ((server_array[this_server] =
      (struct server_list *) malloc(sizeof(struct server_list))) == NULL)
  {
#ifdef DEBUG
    fprintf(stderr,"SetupRemClient: couldn't malloc server_array!\n");
#endif
    return FAILURE;
  }
  if ((server_array[this_server]->client_listen_fd = 
					socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
#ifdef DEBUG
      fprintf(stderr,"doRemClientSetup: socket() call failed!");
#endif
      port_array[port_counter].state = AVAILABLE;
      free(server_array[this_server]);
      server_array[this_server] = NULL;
      return FAILURE;
  }
  /*
  // this is where we initialize the current time and timeout on this 
  // client_listen object
  */
  gettimeofday(&time_val, &time_zone);
  server_array[this_server]->creation_time = time_val.tv_sec; 
  server_array[this_server]->time_to_close = 
				global_data.config_info->client_listen_timeout;
  /*
  // set up the rest of the remote client listener
  */
  bzero((char * ) &rem_sockaddr_in, sizeof(rem_sockaddr_in));
  rem_sockaddr_in.sin_family = AF_INET;
  if (setsockopt(server_array[this_server]->client_listen_fd,
		 SOL_SOCKET, SO_REUSEADDR,
                 &one, sizeof(one)) < 0) 
  {
#ifdef DEBUG
      fprintf(stderr, "doRemClientSetup:  setsockopt() call failed!\n");
#endif
  returnFailure:
      port_array[port_counter].state = AVAILABLE;
      close(server_array[this_server]->client_listen_fd);
      free(server_array[this_server]);
      server_array[this_server] = NULL;
      return FAILURE;
  }

  while (True) {
      rem_sockaddr_in.sin_port = htons(listen_port);
      if (bind(server_array[this_server]->client_listen_fd, 
	       (struct sockaddr *)&rem_sockaddr_in, 
	       sizeof(rem_sockaddr_in)) == 0)
	  break;
      if (errno != EADDRINUSE)
      {
  #ifdef DEBUG
	  fprintf(stderr,"doSetupRemClientListen:  bind() call failed!\n");
  #endif
	  goto returnFailure;
      }
      port_array[port_counter++].state = AVAILABLE;
      for (; port_counter < MAX_CLIENT_LISTEN_PORTS; port_counter++)
      {
	if (port_array[port_counter].state == AVAILABLE)
	{
	  port_array[port_counter].state = IN_USE;   
	  break;
	} else if (port_counter == MAX_CLIENT_LISTEN_PORTS - 1) 
	{
	  /*
	  // No more listen ports available for allocation
	  */ 
	  fprintf(stderr,"All listen ports in use.\n");
	  goto returnFailure;
	}
      }
      listen_port = port_counter + 6001;
  }
#ifdef DEBUG
  fprintf (stderr, "\nClient connect port:  %d\n", listen_port);
#endif

  if (listen(server_array[this_server]->client_listen_fd, SOMAXCONN) < 0) 
  {
#ifdef DEBUG
      fprintf(stderr, "doSetupRemClientListen:  listen() call failed!\n");
#endif
      goto returnFailure;
  }
  /*
  // add the client_listen_fd to the port_array so we can check timeouts
  */
  port_array[port_counter].client_listen_fd = 
			server_array[this_server]->client_listen_fd;
  /*
  //  set the select() read mask for this descriptor
  */
  FD_SET(server_array[this_server]->client_listen_fd, program_data->rinit);
  /*
  //  update the nfds
  */
  *(program_data->nfds) = max(*(program_data->nfds), 
	  server_array[this_server]->client_listen_fd + 1);
  /*
  // get fully qualified name of host on which FWP is running
  */
  if ((status = gethostname(hostname, MAX_HOSTNAME_LEN)) < 0)
  {
#ifdef DEBUG
     fprintf(stderr, "doSetupRemClientListen:  error on gethostname()!\n");
#endif
     FD_CLR(server_array[this_server]->client_listen_fd, program_data->rinit);
     goto returnFailure;
  }
  /*
  // allocate and convert the listen_port string for return to PM;
  // string equals address of host on which FWP is running 
  // plus ":<listen_port - 6000> (up to xxx)"
  */
  if (((*listen_port_string) = 
	(char *) malloc (strlen(hostname + 1) + 4)) == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "SetupRemClient: couldn't malloc listen_port_string!\n");
#endif
    FD_CLR(server_array[this_server]->client_listen_fd, program_data->rinit);
    goto returnFailure;
  }
  strcpy(*listen_port_string, hostname);
  strcat(*listen_port_string, ":");
  sprintf(port_buff, "%d", port_counter + 1); 
  strcat(*listen_port_string, port_buff); 
  /*
  // add the server name associated with the current PM request
  // to the list
  */
  if ((server_array[this_server]->x_server_hostport =
	(char *) malloc (strlen(server_address) + 1)) == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "SetupRemClient: couldn't malloc server_array string\n");
#endif
    free(*listen_port_string);
    *listen_port_string = NULL;
    FD_CLR(server_array[this_server]->client_listen_fd, program_data->rinit);
    goto returnFailure;
  }
  strcpy(server_array[this_server]->x_server_hostport, server_address);
  /*
  // add the client listen port associated with the current PM connection
  // to the list
  */
  if ((server_array[this_server]->listen_port_string =
	(char *) malloc (strlen(*listen_port_string) + 1)) == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "SetupRemClient: couldn't malloc server_array string\n");
#endif
    free(server_array[this_server]->x_server_hostport);
    free(*listen_port_string);
    *listen_port_string = NULL;
    FD_CLR(server_array[this_server]->client_listen_fd, program_data->rinit);
    goto returnFailure;
  }
  strcpy(server_array[this_server]->listen_port_string, *listen_port_string);
  return SUCCESS;
}


void  doSelect(struct config * config_info,
	       int * nfds, 
 	       int * nready, 
               fd_set * readable, 
               fd_set * writable)
{
  struct timeval	tmp_timeval;

  if ((*nready = select(*nfds, 
			readable, 
			writable, 
			NULL, 
			&config_info->select_timeout)) == -1)
  {
    if (errno == EINTR)
      return;
#ifdef DEBUG
    fprintf(stderr, "doSelect:  select() returned error!\n");
#endif
    perror("select");
    exit(1);
  }
}

void  doProcessSelect(int * nfds,
		      int * nready, 
		      fd_set * readable, 
		      fd_set * writable,
		      fd_set * rinit, 
		      fd_set * winit,
		      int pm_listen_array[],
		      int * pm_conn_counter,
		      int * rem_listen_counter,
		      struct config * config_info,
	              struct client_conn_buf * client_conn_array[],
		      struct ice_data * ice_data,
		      IceListenObj ** listen_objects)
{
  int fd_counter;
  /*
  // Loop through descriptors
  */
  for (fd_counter = 0; fd_counter < *nfds && *nready; fd_counter++)
  {
    /*
    // Look at fd's for writables
    */
    if (FD_ISSET(fd_counter, writable))
    {
      /*
      // Decrement the list of read/write ready connections
      */
      *nready -= 1; 
      doProcessWritables(fd_counter, nfds, nready, readable, writable, 
		       rinit, winit, pm_listen_array,
		       pm_conn_counter, 
		       rem_listen_counter, config_info, 
		       client_conn_array,
		       ice_data);
    }
    /*
    // Now, do the same thing for the readables
    */
    if (FD_ISSET(fd_counter, readable))
    {
      /*
      // Decrement the list of read/write ready connections
      */
      *nready -= 1; 
      doProcessReadables(fd_counter, nfds, nready, readable, writable, 
		       rinit, winit, pm_listen_array,
		       pm_conn_counter, 
		       rem_listen_counter, config_info, 
		       client_conn_array,
		       ice_data, listen_objects);
    }
  }
}


void  doProcessWritables(int fd_counter,
 		         int * nfds,
			 int * nready, 
		         fd_set * readable, 
		         fd_set * writable,
		         fd_set * rinit, 
		         fd_set * winit,
		         int pm_listen_array[],
		         int * pm_conn_counter,
		         int * rem_listen_counter,
		         struct config * config_info,
	                 struct client_conn_buf * client_conn_array[],
		         struct ice_data * ice_data)
{
  int bytes_written; 
  int remainder;
  /*
  // start off by writing from the selected fd to its connection
  // partner 
  */
  if (client_conn_array[fd_counter]->wbytes)
  {
    /*
    // See how much you manage to write
    */
    bytes_written = write(fd_counter, client_conn_array[fd_counter]->writebuf,
	       client_conn_array[fd_counter]->wbytes);
    /*
    // handle some common error conditions
    */
    if (bytes_written == -1)
    {
      /*
      // no process attached to the other end of this socket
      */	
      if (errno == EPIPE)
      {
#ifdef DEBUG
	fprintf (stderr, "fd %d:", fd_counter);
#endif
        perror("socket write");
      }
      /*
      // clean up
      */
      FD_CLR(fd_counter, rinit);
      FD_CLR(fd_counter, winit);
      close(fd_counter);
      if (client_conn_array[fd_counter]->conn_to != -1)
      {
        FD_CLR(client_conn_array[fd_counter]->conn_to, rinit);
        FD_CLR(client_conn_array[fd_counter]->conn_to, winit);
        close(client_conn_array[fd_counter]->conn_to);
      }  
      client_conn_array[client_conn_array[fd_counter]->conn_to]->conn_to = -1;
      client_conn_array[fd_counter]->conn_to = -1;
      free(client_conn_array[fd_counter]);
      client_conn_array[fd_counter] = NULL;
      return;
    } else
    {
      /*
      // no errors on write, but did you write everything in the buffer?
      */
      remainder = client_conn_array[fd_counter]->wbytes - bytes_written;
      if (remainder)
      {
	/*
	// move any remainder to front of writebuffer and adjust
        // writebuf byte counter
	*/
	bcopy(client_conn_array[fd_counter]->writebuf + bytes_written,
	      client_conn_array[fd_counter]->writebuf,
	      remainder);
 	client_conn_array[fd_counter]->wbytes = remainder;
      } else
	/*
	// writebuffer *must* be empty, so zero byte counter
	*/ 
	client_conn_array[fd_counter]->wbytes = 0;
      /*
      //take this opportunity to get more read data, if any present
      */
      if ((client_conn_array[fd_counter]->conn_to != -1) &&
	 (client_conn_array[client_conn_array[fd_counter]->conn_to]->rbytes))
	   doCopyFromTo(client_conn_array[fd_counter]->conn_to, fd_counter,
		     client_conn_array, rinit, winit);
      /*
      // If the above operation didn't put anything on the writebuffer,
      // and the readables handler marked the fd ready to close, then
      // clean up and close the connection; otherwise, simply clear the
      // fd_set mask for this fd
      */
      if (client_conn_array[fd_counter]->wbytes == 0)
      {
        if (client_conn_array[fd_counter]->wclose == 1)
        {
	  FD_CLR(fd_counter, rinit);
	  client_conn_array[fd_counter]->conn_to = -1;
	  client_conn_array[fd_counter]->wclose = 0;
	  close(fd_counter);
	  free(client_conn_array[fd_counter]);
	  client_conn_array[fd_counter] = NULL;
	}
	FD_CLR(fd_counter, winit);
      }
      /*
      // since we just wrote data to the conn_to fd, mark it as ready 
      // to check for reading when we go through select() the next time
      */
      if (client_conn_array[fd_counter]->conn_to != -1)
        FD_SET(client_conn_array[fd_counter]->conn_to, rinit);
    } /* end else no errors on write  */
  } else
  {
    /*
    // There was nothing to write on this fd (can't see how we'd get
    // here if select() returned this fd as writable, but it's in 
    // XForward so who am I to say?!) 
    */
    if ((client_conn_array[fd_counter]->conn_to != -1) &&
        (client_conn_array[client_conn_array[fd_counter]->conn_to]->rbytes))
    {
      doCopyFromTo(client_conn_array[fd_counter]->conn_to, fd_counter,
		     client_conn_array, rinit, winit);
      /*
      // if you got anything to write, then proceed to next iter of select()
      */
      if (client_conn_array[fd_counter]->wbytes)
        return;
    }
    /*
    // You didn't get anything from that copy; check to see if it was
    // because the readables handler marked the fd closed; if so, 
    // close this association; otherwise, simply clear the fd_set 
    // writable mask for this fd 
    */ 
    if (client_conn_array[fd_counter]->wclose)
      {
        FD_CLR(fd_counter, rinit);
	client_conn_array[fd_counter]->conn_to = -1;
	client_conn_array[fd_counter]->wclose = 0;
	close(fd_counter);
	free(client_conn_array[fd_counter]);
	client_conn_array[fd_counter] = NULL;
      }
    FD_CLR(fd_counter, winit);
  }
  /*
  // you're done with the writables  
  */
}

void  doProcessReadables(int fd_counter,
			 int * nfds,
			 int * nready, 
		         fd_set * readable, 
		         fd_set * writable,
		         fd_set * rinit, 
		         fd_set * winit,
		         int pm_listen_array[],
		         int * pm_conn_counter,
		         int * rem_listen_counter,
		         struct config * config_info,
	                 struct client_conn_buf * client_conn_array[],
		         struct ice_data * ice_data,
			 IceListenObj ** listen_objects)
{
  int 				listen_counter;
  int 				found_pm_listener = 0;
  int				bytes_read;
  int				endian;
  int				four = 4;
  int				server_reason_remainder;
  char *			conn_auth_name = "XC-QUERY-SECURITY-1"; 
  char * 			server_reason_padded;
  int				server_reason_len;
  int 				conn_auth_namelen, conn_auth_datalen;
  char				throw_away[RWBUFFER_SIZE];
  struct sockaddr_in		server_sockaddr_in;
  enum CONFIG_CHECK		server_status;
  xConnClientPrefix 		client;    
  xConnSetupPrefix 		prefix;    
  /*
  // Check to see if this readable is the PM requesting 
  // a connection (this is a single well-known fd); NOTE:
  // we're special-casing the PM connections because they're
  // different from our usual client<->server ones; most 
  // notably, we don't forward any data to them from another
  // connection
  */
  for (listen_counter = 0; listen_counter < MAX_TRANSPORTS; listen_counter++)
  {
    if (pm_listen_array[listen_counter] == fd_counter) 
    {
      found_pm_listener = 1;
      break;
    }
  }
  if ((found_pm_listener) && (pm_conn_array[fd_counter] == NULL))
  {
    /*
    //  the fd selected indicates a PM listener socket, but no
    //  PM connection buffer has been allocated for the fd, so we 
    //  must prepare to accept a new connection 
    */
    IceConn			new_ice_conn;
    IceAcceptStatus  		accept_status;
    IceProcessMessagesStatus  	process_status;
    int				temp_sock_fd;
    IceListenObj *		temp_obj;
    int				check_sock_fd;
    struct timeval		time_val;
    struct timezone		time_zone;
    struct sockaddr_in		temp_sockaddr_in;
    int				retval;
    int				config_check;
    int				addrlen = sizeof(temp_sockaddr_in);
    /*
    // start by accepting the connection if you can, use pm_listen_array 
    // index to index into ICE listen_object list (this is because the
    // listen_objects list must correspond to the pm_listen_array)
    */
    temp_obj = *listen_objects; 
    new_ice_conn = IceAcceptConnection(temp_obj[listen_counter],
			&accept_status);
    /*
    // extract the fd from this new connection; remember, the fd of
    // the listen socket is *not* the fd of the actual connection!
    */
    temp_sock_fd = IceConnectionNumber(new_ice_conn); 
    /*
    // before we get any further, do a config check on the new ICE
    // connection; start by using getpeername() to get endpoint info
    */
    retval = getpeername(temp_sock_fd, 
			 (struct sockaddr*)&temp_sockaddr_in, 
			 &addrlen);

    assert(temp_sockaddr_in.sin_family == AF_INET);
    
    /*
    // then do the configuration check; NOTE:  we're not doing anything
    // with the server_sockaddr_in argument 
    */
    if ((config_check = doConfigCheck(&temp_sockaddr_in, 
				      &server_sockaddr_in,
				      config_info,
				      PMGR)) == FAILURE)
    {
      /*
      // close the PM connection 
      // 
      */
#ifdef DEBUG
      fprintf(stderr, 
	      "doProcessReadables:  Proxy manager failed config check!\n");
#endif
      IceCloseConnection(new_ice_conn);
      return;
    }
    /*	
    // you've started the connection process; allocate a buffer
    // for this connection, then continue processing other fd's without 
    // blocking while waiting to read the coming PM data; [NOTE:
    // we use the fd of the connection socket as index into the
    // pm_conn_array; this saves us much troublesome linked-list
    // management!]
    */
    if ((pm_conn_array[temp_sock_fd] =
                (struct pm_conn_buf *) malloc(sizeof(struct pm_conn_buf))) == NULL)
    {
#ifdef DEBUG
      fprintf(stderr, "doProcessReadables:  couldn't malloc pm connection object!\n");
#endif
      return;
    }
    /*
    // save the ICEconn struct for future status checks; also
    // the fd (although you could extract it from the ICEconn
    // each time you need it, but that's a pain)
    */
    pm_conn_array[temp_sock_fd]->fd = temp_sock_fd;
    pm_conn_array[temp_sock_fd]->ice_conn = new_ice_conn; 
    /*
    // put the PM connection fd into global data (you're going
    // to need it in the ICE callbacks)
    */
    global_data.pm_connection_fd = temp_sock_fd; 
    /*
    // Increment the pm connection counter, which represents
    // the total number of PM connections we have accepted
    // NOTE:  We are simply keeping track of this for logging
    // purposes
    */
    *pm_conn_counter += 1;
    /*
    // Set the readables select() to listen for a readable on this
    // fd; remember, we're not interested in pm writables, since
    // all the negotiation is handled inside this routine; adjust
    // the nfds (must do that everytime we get a new socket to
    // select() on), and then contnue processing current selections
    */
    FD_SET(temp_sock_fd, rinit);
    *nfds = max(*nfds, temp_sock_fd + 1);
    /*
    // this is where we initialize the current time and timeout on this 
    // pm_connection object
    */
    gettimeofday(&time_val, &time_zone);
    pm_conn_array[temp_sock_fd]->creation_time = time_val.tv_sec; 
    pm_conn_array[temp_sock_fd]->time_to_close = config_info->pm_data_timeout; 
    /*
    // we've finished handling the new PM connection, so return
    */
    return;
  } else
  {
    /*
    // we either found an already-connected PM listener fd or
    // not a PM listener at all
    */
    int 			temp_counter; 
    IceConnectStatus    	connection_status;
    IceProcessMessagesStatus    process_status;
    int				check_sock_fd;
    /*
    // find out if this is an already-accepted PM connection;
    // if so, and there's something to read on its fd, then call
    // IceProcessMessages() to invoke the FWPprocessMessages
    // callback 
    */
    for (temp_counter = 0; temp_counter < MAX_PM_CONNS; temp_counter++)
    {
      if ((pm_conn_array[temp_counter] != NULL) &&
	  (pm_conn_array[temp_counter]->fd == fd_counter))
      {
	/*
 	// the selected fd is one of the previously-accepted PM
	// connections; so check its status
	*/	
        connection_status = 
	    IceConnectionStatus(pm_conn_array[temp_counter]->ice_conn);
        if (connection_status == IceConnectPending)
	{
  	  /*
	  // for some reason this connection still isn't ready for 
	  // reading, so return and try next readable
	  */
	  process_status = 
	  IceProcessMessages(pm_conn_array[temp_counter]->ice_conn, 
			   NULL, NULL);
	  return;
  	} else if (connection_status == IceConnectAccepted)
  	{
    	  /*
    	  // you're ready to read the PM data, allocate and send back 
    	  // your client listen port, etc., etc.; do this inside 
	  // FWPprocessMessages() by calling IceProcessMessages()
          // [NOTE:  The NULL args set it up for non-blocking]
          */
    	  process_status = 
	      IceProcessMessages(pm_conn_array[temp_counter]->ice_conn, 
				   NULL, NULL);
          if (process_status == IceProcessMessagesSuccess) 
    	  {
      	    /*
	    // you read the server data, allocated a listen port 
	    // for the remote client and wrote it back to the PM,
	    // so you don't need to do anything more until PM
	    // closes the connection (NOTE:  Make sure we don't
	    // do this more than once!!)
            */
            return;
          } else if (process_status == IceProcessMessagesIOError) 
	  {
      	    /*
            //  there was a problem with the connection, close it explicitly,
	    //  reset the select() readables mask and nfds, free the buffer
            //  memory on this array element, reset the pointer to NULL
 	    //  and return
            */
  	    IceCloseConnection(pm_conn_array[temp_counter]->ice_conn);
	    FD_CLR(pm_conn_array[temp_counter]->fd, rinit);
	    *nfds = max(*nfds, pm_conn_array[temp_counter]->fd + 1);
	    *pm_conn_counter -= 1;
	    free(pm_conn_array[temp_counter]);
	    pm_conn_array[temp_counter] = NULL;
	    return;
          } else 
	  {
            if (process_status == IceProcessMessagesConnectionClosed)
            {
	      /*
	      // the connection somehow closed itself, so don't call 
              // IceCloseConnection, but *do* free the pm_conn_array
	      // memory, reset its pointer to NULL, clear the 
	      // select() readables mask, recomputer nfds and return
	      */
	      FD_CLR(pm_conn_array[temp_counter]->fd, rinit);
	      *nfds = max(*nfds, pm_conn_array[temp_counter]->fd + 1);
	      *pm_conn_counter -= 1;
	      free(pm_conn_array[temp_counter]);
	      pm_conn_array[temp_counter] = NULL;
	      return;
            }
          }
        } else if (connection_status == IceConnectRejected)
        {
          /*
          // don't know yet what to do in this case, but for now simply
          // output diagnostic and return to select() processing
          */
#ifdef DEBUG
	  fprintf(stderr, "doProcessReadables:  PM connection rejected!\n");
#endif
	  return;
        } else if (connection_status == IceConnectIOError)
        {
          /*
          // don't know yet what to do in this case, but for now simply
          // output diagnostic and return to select() processing
          */
#ifdef DEBUG
	  fprintf(stderr, "doProcessReadables:  PM connection error!\n");
#endif
	  return;
        } else
	{
          /*
          // this condition undefined
          */
          return;
	}
      } /* end if this was an already-accepted PM connection */
    } /* end for to check all possible PM connections */
  } /* end else see if this was an established PM connection request */
  /*
  // Okay, we just finished checking the PM readables;
  // if our current readable wasn't one of these, go
  // on to the rem_listen_client readables; NOTE there
  // is no single rem_listen_fd (as in XForward), but
  // a *list* of them (each one corresponding to a different
  // X-server connection); we therefore have to step through
  // all of these fd's to determine if the readable is among
  // them;  if it is, the first thing we do is accept()
  // this connection and check it against configuration data
  // to see whether its origination host is allowed;
  // next, we connect to the server found in the lookup,
  // synthesize a proxy connection setup request to be sent
  // to that server to determine whether it`s a secure server;
  // we can't block on the server reply so we go ahead and allocate
  // a data structure for this client connection, mark its
  // state PENDING, and exit the main "for" loop looking for
  // something else to read or write
  */
  for (listen_counter = 0;
             listen_counter < MAX_SERVERS; listen_counter++)
  {
    /*
    // Cycle through the server_array, comparing fd's on
    // incoming client connection request to list of fd's
    // associated with particular server connections (originally
    // sent by PM)
    */
    if ((server_array[listen_counter] != NULL) &&
        (server_array[listen_counter]->client_listen_fd == fd_counter))
    {
      /*
      // Okay, you found the fd returned by select()
      // in this list; remember, this is the *listen* fd
      // on FWP for remote clients trying to connect to a
      // specific server; so accept the connection; check
      // configuration, if connection allowed, allocate
      // the read/write buffer for this connection;
      */
      struct sockaddr_in  temp_sockaddr_in;
      int                 temp_sock_fd;
      int                 temp_sock_len;
      int                 host_count;
      int                 config_check;
      struct timeval	  time_val;
      struct timezone	  time_zone;

      temp_sock_len = sizeof(temp_sockaddr_in);
      if ((temp_sock_fd = accept(fd_counter,
                                 (struct sockaddr *) &temp_sockaddr_in, 
	 			 &temp_sock_len)) < 0)
      {
        /*
        // EINTR is no cause for alarm; simply skip this fd and 
	// keep processing
        */
        if (errno == EINTR)
          return; 
        else
        {
#ifdef DEBUG
          fprintf(stderr, 
		  "doProcessReadables: can't accept rem client connection\n");
#endif
          return;
        }
      }
      /*
      //  try to open a connection to the server 
      */
      if ((server_status = 
	doServerConnectSetup(server_array[listen_counter]->x_server_hostport, 
			     &server_array[listen_counter]->server_fd, 
			     &server_sockaddr_in)) == FAILURE)
      {
#ifdef DEBUG
	fprintf(stderr, 
		"doProcessReadables:  could not setup server connection!\n");
#endif
	return;
      }
      if ((server_status = 
		doServerConnect(&server_array[listen_counter]->server_fd, 
		&server_sockaddr_in)) == FAILURE)
      {
#ifdef DEBUG
	fprintf(stderr, 
		"doProcessReadables:  could not connect to server!\n");
#endif
	return;
      }
      /*
      // do config check on client source and destination (must do
      // it here because otherwise we don't have a server socket
      // to query and we may not be able to resolve server name 
      // alone from xfindproxy() 
      */
      if ((config_check = doConfigCheck(&temp_sockaddr_in, 
				 	&server_sockaddr_in,
					config_info,
					CLIENT)) == FAILURE)
      {
        /*
        // close client and server sockets and return
        // 
        */
#ifdef DEBUG
        fprintf(stderr, 
		"doProcessReadables:  Remote client failed config check!\n");
#endif
        close(temp_sock_fd);
        close(server_array[listen_counter]->server_fd);
        return;
      }
      /*
      // If configured authorization succeeds, go ahead and
      // allocate a client_conn_buf struct for client connection
      */
      if ((client_conn_array[temp_sock_fd] =
          (struct client_conn_buf *) malloc(sizeof (struct client_conn_buf)))
          == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, 
	 "doProcessReadables:  could not allocate client connection buffer!\n");
#endif
	return;
      }
      /*
      // allocate a buffer for the X server connection 
      // and create the association between client and server 
      */
      if ((client_conn_array[server_array[listen_counter]->server_fd] =
          (struct client_conn_buf *) malloc(sizeof (struct client_conn_buf)))
          == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, 
	 "doProcessReadables:  could not allocate server connection buffer!\n");
#endif
	return;
      }
      client_conn_array[server_array[listen_counter]->server_fd]->conn_to =
      			temp_sock_fd;
      client_conn_array[temp_sock_fd]->conn_to = 
			server_array[listen_counter]->server_fd;
      /*
      // save this sock fd for future reference (in timeout computation)
      */ 
      client_conn_array[temp_sock_fd]->fd = temp_sock_fd;
      /*
      // mark this buffer as readable and writable and waiting for 
      // authentication to complete; mark the server conn buffer
      // with a special state to make sure that its reply to 
      // the authentication request can be read and interpreted
      // before it is simply forwarded to the client 
      */
      client_conn_array[temp_sock_fd]->state = CLIENT_WAITING;
      client_conn_array[server_array[listen_counter]->server_fd]->state 
						= SERVER_REPLY;
      /*
      // update the select() fd mask and the nfds
      */
      FD_SET(temp_sock_fd, rinit);
      *nfds = max(*nfds, temp_sock_fd + 1);
      FD_SET(server_array[listen_counter]->server_fd, rinit);
      *nfds = max(*nfds, server_array[listen_counter]->server_fd + 1);
      /*
      // this is where we initialize the current time and timeout on this 
      // client_data object
      */
      gettimeofday(&time_val, &time_zone);
      client_conn_array[temp_sock_fd]->creation_time = time_val.tv_sec;
      client_conn_array[temp_sock_fd]->time_to_close = 
					config_info->client_data_timeout;
      /*
      // be sure the mark the server side of the association, too
      */
      client_conn_array[server_array[listen_counter]->server_fd]->creation_time =
						       time_val.tv_sec; 
      client_conn_array[server_array[listen_counter]->server_fd]->time_to_close =
					config_info->client_data_timeout;
      /*
      // go get next readable 
      */
      return; 
    } /* end if rem_listen */
    /*
    // You didn't find the fd on the selected socket in your
    // server_list; this is impossible! (unless you failed
    // to save it correctly during PM processing, or you set
    // up your fd_set readable mask incorrectly)
    */
  } /* end "for" loop checking client listen fd's */
  /*
  // If you didn't select() on the client listen fd 
  // the only other possible readable is an existing
  // client or server connection; in this case there are still
  // three options:  we might be reading the server fd
  // (representing either the server's reply to the pending
  // authentication *or* other data coming from server->client)
  // or we're reading a remote client fd which has already been
  // authenticated to the server, in which case we simply want
  // to forward the data
  */
  if (client_conn_array[fd_counter] != NULL)
  { 
    if (client_conn_array[fd_counter]->state == CLIENT_WAITING)
    {
      /*
      // The remote client is sending more data on an already-
      // established connection, but we still haven't checked
      // authentication on this client from the associated
      // X-server; so do the following:
      // 1. create the authentication header
      // 2. mark the server fd writable
      // 3. copy the header info into the write buffer
      // 3. set the wbytes field to the header size
      // 4. mark the state SERVER_WAITING
      // 5. return
      */
      conn_auth_namelen = strlen(conn_auth_name);

      if (SitePolicyCount == 0)
	  conn_auth_datalen = 0;
      else
      {
	  int sitePolicy;
	  conn_auth_datalen = 3;
	  for (sitePolicy = 0; sitePolicy < SitePolicyCount; sitePolicy++)
	  {
	      conn_auth_datalen += 1 + strlen(SitePolicies[sitePolicy]);
	  }
      }

      endian = 1;
      if (*(char *) &endian)
          client.byteOrder = '\154'; /* 'l' */
      else
          client.byteOrder = '\102'; /* 'B' */
      client.majorVersion = X_PROTOCOL;
      client.minorVersion = X_PROTOCOL_REVISION;
      client.nbytesAuthProto = conn_auth_namelen;
      client.nbytesAuthString = conn_auth_datalen;
      /*
      // now the tricky part; put the authentication message
      // into the appropriate client_conn_buf object and increment
      // the write bytes counter, then mark this fd as selectable
      // to force a write() operation
      */
      {
	  /*
	  // compute required padding for name and data strings
	  */
	  int name_remainder = (4 - (conn_auth_namelen % 4)) % 4;
	  int data_remainder = (4 - (conn_auth_datalen % 4)) % 4;

	  char *bufP=client_conn_array[client_conn_array[fd_counter]->conn_to]
		     ->writebuf;

	  memcpy(bufP, (char *) &client, sizeof(client));
	  bufP += sizeof(client);

	  memcpy(bufP, (char *) conn_auth_name, conn_auth_namelen);
	  bufP += conn_auth_namelen;

	  bzero(bufP, name_remainder);
	  bufP += name_remainder;

	  if (conn_auth_datalen)
	  {
	      int sitePolicy;

	      *bufP++ = 0x02;	/* Site Policies only at this point */
	      *bufP++ = (SitePolicyPermit == False);
	      *bufP++ = SitePolicyCount;
	      for (sitePolicy = 0; sitePolicy < SitePolicyCount; sitePolicy++)
	      {
		  char nameLen = strlen(SitePolicies[sitePolicy]);
		  *bufP++ = nameLen;
		  memcpy(bufP, SitePolicies[sitePolicy], nameLen);
		  bufP += nameLen;
	      }
	      bzero(bufP, data_remainder);
	  }

	  client_conn_array[client_conn_array[fd_counter]->conn_to]
	    ->wbytes = sizeof(client) + conn_auth_namelen + name_remainder
				      + conn_auth_datalen + data_remainder;
      }
      /*
      // force write of authentication request to server for 
      // this remote client 
      */
      FD_SET(client_conn_array[fd_counter]->conn_to, winit);
      /*
      // mark the connection SERVER_WAITING (so that we don't
      // read any more client data until the authentication
      // sequence is complete)
      */ 
      client_conn_array[fd_counter]->state = SERVER_WAITING;
      return;
    } else if (client_conn_array[fd_counter]->state == CONNECTION_READY)
    {
      /*
      // This is the most common case, where we've finished
      // our authentication handshaking and are forwarding data
      // either from client to server or vice versa
      */
      if (client_conn_array[fd_counter]->rbytes < RWBUFFER_SIZE)
      {
        /*
        // read what you have room for
        */
        bytes_read = read(fd_counter,
                          client_conn_array[fd_counter]->readbuf +
                          client_conn_array[fd_counter]->rbytes, RWBUFFER_SIZE -
                          client_conn_array[fd_counter]->rbytes);
        /*
        // check for I/O error on this fd; this is our only way
        // of knowing if remote closed the connection
        */
        if (bytes_read == -1)
        {
          /*
          // remote apparently closed the connection;
          // clear bits in the select() mask, reclaim conn_buffs and
          // listen port 
          */
          FD_CLR(fd_counter, rinit);
          FD_CLR(fd_counter, winit);
          FD_CLR(client_conn_array[fd_counter]->conn_to, rinit);
          FD_CLR(client_conn_array[fd_counter]->conn_to, winit);
          close(client_conn_array[fd_counter]->conn_to);
          close(fd_counter);
          free(client_conn_array[fd_counter]);
          client_conn_array[fd_counter] = NULL;
          /*
          // exit readables for loop
          */
          return;
        } else if (bytes_read == 0)
        {
          /*
          // make sure we don't try to read on this fd again 
          */
          FD_CLR(fd_counter, rinit);
          FD_CLR(fd_counter, winit);
          close(fd_counter);
          if (client_conn_array[fd_counter]->conn_to != -1)
	  {
            /* 
            // mark this conn_fd fd ready to close 
  	    */
   	    client_conn_array[client_conn_array[fd_counter]->conn_to]->wclose 
									= 1;
   	    client_conn_array[client_conn_array[fd_counter]->conn_to]->conn_to 
									= -1;
	    /*
            // but still force a last write on the conn_to connection
            */ 
	    FD_SET(client_conn_array[fd_counter]->conn_to, winit);
	  }	
          /*
          // and mark this connection for no further activity 
          */
   	  client_conn_array[fd_counter]->rbytes = 0;
   	  client_conn_array[fd_counter]->wbytes = 0;
   	  client_conn_array[fd_counter]->conn_to = -1;
        } else
	{
          /*
          // Move bytes between buffers on associated fd's
          // (read data on one becomes write data on other)
          */
          client_conn_array[fd_counter]->rbytes += bytes_read;
          if (client_conn_array[fd_counter]->conn_to != 0)
          doCopyFromTo(fd_counter,
                       client_conn_array[fd_counter]->conn_to,
                       client_conn_array, rinit, winit);
          /*
          // check if you have more read data than available space
          */
          if (client_conn_array[fd_counter]->rbytes >= RWBUFFER_SIZE)
          {
            /*
            // if so, abort the attempted copy until you can
            // write some data out of the buffer first, and
            // don't allow any more reading until that's done
            */
            FD_CLR(fd_counter, rinit);
      	  } 
	}
      } /* end if room to write */
    } else  if (client_conn_array[fd_counter]->state == SERVER_WAITING)
    {  
      /*
      // this is a do-nothing state while we're waiting for the
      // server reply (see below)
      */
      return;
    } else
    {
      if (client_conn_array[fd_counter]->state == SERVER_REPLY)
      {
	/*
	// read the server reply to the authentication request
        */      
        bytes_read = read(fd_counter,
                          client_conn_array[fd_counter]->readbuf +
                          client_conn_array[fd_counter]->rbytes, RWBUFFER_SIZE -
                          client_conn_array[fd_counter]->rbytes);
	/*
	// switch on reply as appropriate
	*/
	switch ((BYTE) client_conn_array[fd_counter]->readbuf[0])
	{
	  case SERVER_REPLY_FAILURE:
#ifdef DEBUG
	    {  char *replyP = client_conn_array[fd_counter]->readbuf;
	       int reasonLength = *++replyP;
	       replyP += 6;	/* skip major & minor version, msg len */
	       *(replyP+reasonLength+1) = '\0';
	       fprintf(stderr, "Server replied FAILURE: %s\n", replyP);
	    }
#endif

	    /* FALL-THROUGH */
	  case SERVER_REPLY_SUCCESS:
	    /* 
	    // two possibilities here:  either the policy field 
	    // passed to the server is unauthorized, or the server
	    // does not support the security extension; in both cases
   	    // we read the client fd then synthesize a response
	    // which we forward to the client before closing the 
	    // connection
            */
            bytes_read = read(client_conn_array[fd_counter]->conn_to,
			      throw_away, 0);
	    /*
	    // construct the client response
            */
	    prefix.success = 0;
	    prefix.lengthReason = server_reason_len = 
		strlen(server_reason
		       [(int) client_conn_array[fd_counter]->readbuf[0]]);
      	    prefix.majorVersion = X_PROTOCOL;
            prefix.minorVersion = X_PROTOCOL_REVISION;
            server_reason_remainder = server_reason_len;
	    /*
	    // calculate quadword padding required
	    */
            while (server_reason_remainder > 0)
	      server_reason_remainder = server_reason_remainder - four;
            server_reason_remainder = abs(server_reason_remainder);
	    /*
	    // allocate the padded buffer
	    */
            if ((server_reason_padded = 
	       (char *) malloc (server_reason_len + 
				server_reason_remainder)) == NULL)
            {
#ifdef DEBUG
	      fprintf(stderr, 
		"doProcessReadables:  could not malloc server reason!\n");
#endif
	      return;
            } 
	    /*
	    // calculate the "additional data" field
	    */
            prefix.length = (server_reason_len + server_reason_remainder) /
			    four; 
	    /*
	    // compare client and xfwp byte ordering and swap prefix fields
	    // as necessary
            */
   	    endian = 1;
            if (((throw_away[0] == '\154') && !(*(char *) &endian)) ||
                ((throw_away[0] == '\102') && (*(char *) &endian)))
	    {
	      /*
	      // client and xfwp are different byte order
	      // so swap all fwp 2-byte fields to little endian
	      */
      	      swab((char *)(long) prefix.majorVersion, 
		   (char *)(long) prefix.majorVersion,
		   sizeof(prefix.majorVersion));
      	      swab((char *)(long) prefix.minorVersion, 
		   (char *)(long) prefix.minorVersion,
		   sizeof(prefix.minorVersion));
	      swab((char *)(long) prefix.length, 
		   (char *)(long) prefix.length, sizeof(prefix.length));
	    }
	    /*
	    // load the padded reason
            */
            bzero((char *) server_reason_padded, 
		   server_reason_len + server_reason_remainder);
            memcpy((char *) server_reason_padded, 
		   (char *) server_reason
			    [(int) client_conn_array[fd_counter]->readbuf[0]],
		   server_reason_len);
	    /*
	    // load the complete synthesized server reply (which will
	    // be sent to the client next time the writables are
	    // processed (again, to avoid blocking) 
            */
            memcpy((char *)
	      client_conn_array[fd_counter]
	      ->readbuf, (char *) &prefix, sizeof(prefix));
            memcpy((char *)
	      client_conn_array[fd_counter]
	      ->readbuf + sizeof(prefix),
              (char *) server_reason_padded, 
	      server_reason_len + server_reason_remainder);
            client_conn_array[fd_counter]
	      ->rbytes = sizeof(prefix) + server_reason_len + 
			 server_reason_remainder;
	    /*
 	    // make sure to zero the wbytes on the client conn object
            // before forwarding the server reply
	    */
	    client_conn_array[client_conn_array[fd_counter]->conn_to]->wbytes = 0;
	    doCopyFromTo(fd_counter, client_conn_array[fd_counter]->conn_to,
		     client_conn_array, rinit, winit);
	    client_conn_array[fd_counter]->wclose = 1;
	    client_conn_array[client_conn_array[fd_counter]->conn_to]->conn_to = -1;
	    /*
	    // clear the select() mask for the client socket and for
	    // the server
	    */
	    FD_CLR(client_conn_array[fd_counter]->conn_to, rinit);
	    FD_CLR(fd_counter, rinit);
	    /*
	    // output a trace message
            */
	    if (((int) client_conn_array[fd_counter]->readbuf[0]) == 
							SERVER_REPLY_SUCCESS)
	    {
#ifdef DEBUG
	      fprintf(stderr, "Server replied SUCCESS!\n");
#endif
	      ;
	    }
	    /*
	    // clean up memory
	    */
	    free(server_reason_padded);
	    break;
	  case SERVER_REPLY_AUTHENTICATE:
	    /*
	    // the server supports the security extension; begin
	    // forwarding client and server messages normally
	    */
	    client_conn_array[fd_counter]->state = CONNECTION_READY;
	    client_conn_array[client_conn_array[fd_counter]->conn_to]->state =
							CONNECTION_READY;
#ifdef DEBUG
	    fprintf(stderr, "Server replied AUTHENTICATE!\n");
#endif
	    break;
	  default:
#ifdef DEBUG
	    fprintf(stderr, "Server replied UNKNOWN!\n");
#endif
	    break;
	}
      } 
    }	
  }/* end if client_conn_array != NULL */
}

int doConfigCheck(struct sockaddr_in * source_sockaddr_in,
	          struct sockaddr_in * dest_sockaddr_in,
		  struct config * config_info,
		  int context)
{
  int			line_counter;
  /*
  // look through the config file parse tree for a source IP address
  // that matches this request
  */ 
  for (line_counter = 0; line_counter < config_info->rule_count; line_counter++)
  {
    if (config_info->config_file_data[line_counter] != NULL)
    {
      if ((source_sockaddr_in->sin_addr.s_addr & 
	  (~(config_info->config_file_data[line_counter]->source_net))) ==
	  config_info->config_file_data[line_counter]->source_host)
      {
	/*
	// okay, the source host and netmask fields pass, see if the
        // config file specifies "permit" or "deny" for this host
        */
        if (!strcmp(config_info->config_file_data[line_counter]->permit_deny,
	     "permit"))
  	{
	  /*
          // check for presence of destination info
          */
	  if ((config_info->config_file_data[line_counter]->dest_hostname) &&
	      (context != PMGR))
          {
	    /*
            // compute destination info restrictions
            */
            if ((dest_sockaddr_in->sin_addr.s_addr & 
	        (~(config_info->config_file_data[line_counter]->dest_net))) ==
	        config_info->config_file_data[line_counter]->dest_host)
	    {
	      /*
	      // you got a match on the destination, so look at 
              // the operator and service fields to see if the "permit"
              // might be specific to one particular connection-type only
              */
    	      if (config_info->config_file_data[line_counter]->operator != NULL)
	      {
		/*
                // there *is* a service id; see if it matches our current
                // config check request
                */
		if (config_info->config_file_data[line_counter]->service_id 
									== context)
		{
	  	  if (printConfigVerify)
		    doPrintEval(config_info, line_counter);  
		  return 1;
		} else
		  /*
	          // we didn't get a match on context; this "permit" doesn't 
		  // apply to the current request; so keep trying 
		  */
		  continue;
	      } else	
		/*
		// there's no service qualifier; permit the connection 
		*/
                if (printConfigVerify)
		  doPrintEval(config_info, line_counter);
		return 1;
            } else
              /*
	      // the destination field doesn't match; keep trying
              */
	      continue;
	  } else if ((config_info->
			config_file_data[line_counter]->dest_hostname) &&
	      		(context == PMGR))
            { 
	      /*
              // skip the destination address check and test for
              // the operator and service_id
              */
    	      if (config_info->config_file_data[line_counter]->operator != NULL)
	      {
		/*
                // there *is* a service id; see if it matches our current
                // config check context 
                */
		if (config_info->config_file_data[line_counter]->service_id 
								== context)
		{
		  if (printConfigVerify)
		    doPrintEval(config_info, line_counter);
		  return 1;
		} else
		  /*
	          // we didn't get a match on context; this "permit" doesn't
		  // apply to the current client request; so keep trying 
		  */
		  continue;
	      } else	
              {		
		/*
		// there's no service qualifier; permit the connection 
		*/
                if (printConfigVerify)
		  doPrintEval(config_info, line_counter);
		return 1;
	      }
	    } else
	    {
	      /*
              // there's no destination specified; permit the connection
              */
              if (printConfigVerify)
		doPrintEval(config_info, line_counter);
	      return 1;
	    }
        }
        else
  	{
	  /*
          // we still have to check the destination and service fields
	  // to know exactly what we are denying
	  */
	  if ((config_info->config_file_data[line_counter]->dest_hostname) &&
	      (context != PMGR))
          {
	    /*
            // compute destination info restrictions
            */
            if ((dest_sockaddr_in->sin_addr.s_addr & 
	        (~(config_info->config_file_data[line_counter]->dest_net))) ==
	        config_info->config_file_data[line_counter]->dest_host)
	    {
	      /*
	      // you got a match on the destination, so look at
              // the operator and service fields to see if the "deny"
              // might be specific to one particular connection-type only
              */
    	      if (config_info->config_file_data[line_counter]->operator != NULL)
	      {
		/*
                // there *is* a service id; see if it matches our current
                // config check request
                */
		if (config_info->config_file_data[line_counter]->service_id 
									== context)
		{
		  /*
                  // the match signifies an explicit denial of permission
	          */
	  	  if (printConfigVerify)
		    doPrintEval(config_info, line_counter);  
		  return 0;
	        } else
		  /*
		  // we didn't get a match on the service id; the "deny"
	          // operation doesn't apply to this connection, so keep
                  // trying 
		  */
		  continue;
	      } else	
	      {
		/*
		// there's no service qualifier; deny the connection 
		*/
	  	if (printConfigVerify)
		  doPrintEval(config_info, line_counter);  
                return 0;
	      }	
            } else
              /*
              // the destination field doesn't match; keep trying 
              */
	      continue;
	  } else if ((config_info->
			config_file_data[line_counter]->dest_hostname) &&
	      		(context == PMGR))
            { 
	      /*
              // skip the destination address check and test for
              // the operator and service_id
              */
    	      if (config_info->config_file_data[line_counter]->operator != NULL)
	      {
		/*
                // there *is* a service id; see if it matches our current
                // config check context 
                */
		if (config_info->config_file_data[line_counter]->service_id 
								== context)
		{
		  /*
                  // this is a request to explicitly deny service, so do it
		  */
	  	  if (printConfigVerify)
		    doPrintEval(config_info, line_counter);  
		  return 0;
		} else
		  /*
	          // we didn't get a match on context; this "deny" doesn't
		  // apply to the current client request; so keep trying 
		  */
		  continue;
	      } else	
	      {
		/*
		// there's no service qualifier; deny the connection 
		*/
	  	if (printConfigVerify)
		  doPrintEval(config_info, line_counter);  
                return 0;
	      }
	    } else
	    {
	      /*
	      // there's no destination specified; deny the connection
	      */
	      if (printConfigVerify)
	        doPrintEval(config_info, line_counter);  
              return 0;	
	    }
	  } /* end else deny */
      } /* end if match on source */
    } /* end if valid config line */
  } /* end all config lines for loop */
  /*
  // whatever you did not explicitly permit you must deny -- *unless* --
  // no config file was specified, in which case permit all
  */
  if (config_info->config_file_path == NULL)
  {
    if (printConfigVerify)
	fputs("matched default permit 0.0.0.0 255.255.255.255\n", stderr);
    return 1;
  }

  if (printConfigVerify)
      fputs("matched default deny 0.0.0.0 255.255.255.255\n", stderr);

  return 0;
}

int doCopyFromTo(int fd_from, 
		 int fd_to, 
		 struct client_conn_buf * client_conn_array[],
		 fd_set * rinit,
		 fd_set * winit)
{
  int ncopy;
	
  if (client_conn_array[fd_from]->wbytes < RWBUFFER_SIZE)
  {
    /*
    // choose to write either how much you have (from->rbytes),
    // or how much you can hold (to->wbytes), whichever is
    // smaller 
    */
    ncopy = min(client_conn_array[fd_from]->rbytes,
	        RWBUFFER_SIZE - client_conn_array[fd_to]->wbytes);
    /*
    // index into existing number bytes into the write buffer
    // to get the start point for copying 
    */
    bcopy(client_conn_array[fd_from]->readbuf,
	  client_conn_array[fd_to]->writebuf + 
	  client_conn_array[fd_to]->wbytes, ncopy);
    /*
    // Then up the to->wbytes counter
    */
    client_conn_array[fd_to]->wbytes += ncopy;
    /*
    // something has to be done here with the select mask!!
    */
    FD_SET(fd_to, winit); 
    if (ncopy == client_conn_array[fd_from]->rbytes)
      client_conn_array[fd_from]->rbytes = 0;
    else
    {
      bcopy(client_conn_array[fd_from]->readbuf + ncopy,
	    client_conn_array[fd_from]->readbuf,
	    client_conn_array[fd_from]->rbytes - ncopy);
      client_conn_array[fd_from]->rbytes -= ncopy;
    }
    /*
    // and here
    */
    FD_SET(fd_to, rinit);
  }
  /*
  // If there's no room in the fd_to write buffer, do nothing 
  // this iteration (keep iterating on select() until something
  // gets written from this fd)
  */
  return;
}


void FWPprocessMessages(IceConn iceConn, 
                        IcePointer * client_data,
                        int opcode,
                        unsigned long length,
                        Bool swap)
{
  int			PMopcode;
  switch (opcode)
  { 
    /*
    // this is really the only opcode we care about -- the one
    // which indicates an XFindProxy request for a connection
    // to a specified server
    */
    case PM_GetProxyAddr:
    {
      pmGetProxyAddrMsg       	*pMsg;
      char                      *pData, *pStart;
      char                      *serviceName = NULL, *serverAddress = NULL;
      char                      *hostAddress = NULL, *startOptions = NULL;
      char                      *authName = NULL, *authData = NULL;
      char                      *proxyAddress, *errorString = NULL;
      int                       len, authLen;
      int                       pushRequest = 0;
      enum CONFIG_CHECK	        status;
      struct clientDataStruct * program_data; 
      char *			listen_port_string;
      int			pm_send_msg_len;
      pmGetProxyAddrReplyMsg *	pReply;
      char *			pReplyData;
      struct hostent * 		hostptr;
      struct sockaddr_in	server_sockaddr_in;
      struct sockaddr_in	dummy_sockaddr_in;
      char *			server_name_base;
      int			config_check;
      char *			config_failure = "unrecognized server or permission denied";
      char *			tmp_str;
      /*
      // this is where we need and get access to that client data we
      // went through such contortions to set up earlier!
      */
      program_data = (struct clientDataStruct *) client_data;
      /*
      // initial check on expected message size
      */ 
      CHECK_AT_LEAST_SIZE (iceConn, PMopcode, opcode,
          length, SIZEOF (pmGetProxyAddrMsg), IceFatalToProtocol);

      IceReadCompleteMessage (iceConn, SIZEOF (pmGetProxyAddrMsg),
          pmGetProxyAddrMsg, pMsg, pStart);

      if (!IceValidIO (iceConn))
      {
        IceDisposeCompleteMessage (iceConn, pStart);
        return;
      }

      authLen = swap ? lswaps (pMsg->authLen) : pMsg->authLen;

      pData = pStart;

      SKIP_STRING (pData, swap);      /* proxy-service */
      SKIP_STRING (pData, swap);      /* server-address */
      SKIP_STRING (pData, swap);      /* host-address */
      SKIP_STRING (pData, swap);      /* start-options */
      if (authLen > 0)
      {
        SKIP_STRING (pData, swap);              /* auth-name */
        pData += (authLen +  PAD64 (authLen));  /* auth-data */
      }
      /*
      // now a detailed check on message size
      */
      CHECK_COMPLETE_SIZE (iceConn, PMopcode, opcode,
         length, pData - pStart + SIZEOF (pmGetProxyAddrMsg),
         pStart, IceFatalToProtocol);

      pData = pStart;
      /*
      // extract message data, based on known characteristics
      // of this message type
      */
      EXTRACT_STRING (pData, swap, serviceName);
      EXTRACT_STRING (pData, swap, serverAddress);
      EXTRACT_STRING (pData, swap, hostAddress);
      EXTRACT_STRING (pData, swap, startOptions);
      if (authLen > 0)
      {
        EXTRACT_STRING (pData, swap, authName);
        authData = (char *) malloc (authLen);
        memcpy (authData, pData, authLen);
      }
#ifdef DEBUG
      fprintf (stderr, "Got GetProxyAddr, serviceName = %s, serverAddr = %s\n",
              serviceName, serverAddress);
      fprintf (stderr, "  hostAddr = %s, options = %s, authLen = %d\n",
              hostAddress, startOptions, authLen);
#endif
      if (authLen > 0)
          printf ("  authName = %s\n", authName);
      /*
      // need to copy the host port string because strtok() changes it   
      */
      if ((tmp_str = 
    		(char *) malloc (strlen(serverAddress) + 1)) == NULL)
      {
#ifdef DEBUG
        fprintf(stderr, "FWPprocessMessages:  couldn't malloc serverAddress copy!\n");
#endif
        goto sendFailure;
      }
      strcpy(tmp_str, serverAddress);
      /*
      // before proceeding we want to verify that we are allowed to
      // accept connections from the host who called xfindproxy(); 
      // the thing is, we don't get that host name from Proxy Manager
      // even if the "-host <hostname>" command-line option was present
      // in xfindproxy (and even if it was we shouldn't rely on it --
      // much better to have ProxyMngr query the xfindproxy connect
      // socket for its origin); the upshot of all this that we do
      // a configuration check *only* on the destination (which we
      // assume in this case to be the serverAddress passed in by
      // xfindproxy(); so get the destination IP address!
      */
      server_name_base = strtok(tmp_str, ":");
      if ((hostptr = gethostbyname(server_name_base)) == NULL)
      {
#ifdef DEBUG
        fprintf(stderr, "FWPprocessMessages:  could not get server host name!\n");
#endif
	goto sendFailure;
      }
      memset(&server_sockaddr_in, 0, sizeof(server_sockaddr_in));
      memset(&dummy_sockaddr_in, 0, sizeof(dummy_sockaddr_in)); 
      memcpy((char *) &server_sockaddr_in.sin_addr, 
	 hostptr->h_addr,
	 hostptr->h_length);

      /*
      // need to initialize dummy to something, but doesn't matter
      // what (should eventually be the true host address); 
      // NOTE:  source configuration will always match (see XFWP man
      // page) unless sysadmin explicitly chooses to deny 
      */
      memcpy((char *) &dummy_sockaddr_in.sin_addr, 
	 hostptr->h_addr,
	 hostptr->h_length);
      if (!(config_check = doConfigCheck(&dummy_sockaddr_in, 
  				        &server_sockaddr_in,
				        global_data.config_info,
					FINDPROXY)))
      { 
      sendFailure:
        /*
        // report failure back to the ProxyMgr
        // 
        */
        pm_send_msg_len = STRING_BYTES(config_failure)
	      		+ STRING_BYTES(NULL);
        IceGetHeaderExtra(iceConn, 
	      	        program_data->major_opcode, 
	      		PM_GetProxyAddrReply,
	       		SIZEOF(pmGetProxyAddrReplyMsg),
	       		WORD64COUNT (pm_send_msg_len),
	       		pmGetProxyAddrReplyMsg,
	       		pReply,
	       		pReplyData);
        pReply->status = PM_Failure;
        STORE_STRING(pReplyData, NULL);
        STORE_STRING(pReplyData, config_failure);
        IceFlush(iceConn);
#ifdef DEBUG
        fprintf(stderr, 
	      "FWPprocessMessages:  xfindproxy() failed config check!\n");
#endif
	free(tmp_str);
        return; 
      }
      /* 
      // okay, you got what you need from the PM to proceed,
      // so extract the fd of the selected connection and use
      // it to set up the remote client listen port and add 
      // the name of the X server to your list of server connections
      */
      /*
      // Check first to see if you've gotten a PM connection
      // request for this server already; if so, don't allocate
      // another listen port for it, but return the already 
      // allocated one
      */
      if ((status = doCheckServerList(serverAddress, 
				      program_data, 
				      &listen_port_string)) == FAILURE) 
      {
        /*
        // this server name isn't in your list; so set up a new
        // remote client listen port for it; extract the fd from
        // the connection and pass it in as index to array lookup
        */
        if ((status = doSetupRemClientListen(&listen_port_string,
					     program_data,
					     serverAddress)) == FAILURE)
 	{
#ifdef DEBUG
          fprintf(stderr, "error on allocation of listen port.\n");
#endif
          free(tmp_str);
	  return;
	}
      }
      /*
      // the PM-sent server address *was* in your list, so send back
      // the rem client listen port you had already associated with 
      // that server (it will presumably be forwarded to the remote
      // client through some other channel)
      // use IceGetHeaderExtra() and the 
      */
      pm_send_msg_len = STRING_BYTES(listen_port_string)
			+ STRING_BYTES(NULL);
      IceGetHeaderExtra(iceConn, 
		        program_data->major_opcode, 
			PM_GetProxyAddrReply,
			SIZEOF(pmGetProxyAddrReplyMsg),
			WORD64COUNT (pm_send_msg_len),
			pmGetProxyAddrReplyMsg,
			pReply,
			pReplyData);
      pReply->status = PM_Success;
      STORE_STRING(pReplyData, listen_port_string);
      STORE_STRING(pReplyData, NULL); 
      IceFlush(iceConn);
      /*
      // before leaving this routine, change the select() timeout
      // here to be equal to the configured client listen timeout
      // (otherwise you'll never *get* to your listen timeout
      // if it's shorter than the startup select() default
      */
      program_data->config_info->select_timeout.tv_sec =
		program_data->config_info->client_listen_timeout;
      break;
    }

    case ICE_Error:
    {
	iceErrorMsg *pMsg;
	char *pData, *pStart;

	CHECK_AT_LEAST_SIZE (iceConn, PMopcode, ICE_Error, length,
			     sizeof(iceErrorMsg), IceFatalToProtocol);

	IceReadCompleteMessage (iceConn, SIZEOF (iceErrorMsg),
				iceErrorMsg, pMsg, pStart);

	if (!IceValidIO (iceConn))
	{
	    IceDisposeCompleteMessage (iceConn, pStart);
	    return;
	}

	pData = pStart;

	if (swap)
	{
	    pMsg->errorClass = lswaps (pMsg->errorClass);
	    pMsg->offendingSequenceNum = lswapl (pMsg->offendingSequenceNum);
	}
#ifdef DEBUG
	fprintf(stderr, "Proxy Manager reported ICE Error: class=0x%x\n  offending minor opcode=%d, severity=%d, sequence=%d\n",
		pMsg->errorClass, pMsg->offendingMinorOpcode, pMsg->severity,
		pMsg->offendingSequenceNum);
#endif

	IceDisposeCompleteMessage (iceConn, pStart);

	break;
    }

    default:
      break;
  } /* end switch */
}

Bool FWPHostBasedAuthProc (char * hostname)
{
  /*
  // don't worry about config for now
  */
  return True;
  /*
  // this routine gets called *after* IceAcceptConnection 
  // is called but *before* that routine returns its status;
  // it is therefore the logical place to check configuration 
  // data on which PM connections (from which hosts) will be
  // accepted; so do it and return either 0 to terminate 
  // connection (automatically informing PM) or 1 to proceed
  */
  /*
  // the PM host is not allowed; terminate connection and inform
  // requestor why (handled automatically by ICElib)
  */
}

Status FWPprotocolSetupProc(IceConn iceConn,
                           int major_version,
                           int minor_version,
                           char * vendor,
                           char * release,
                           IcePointer * clientDataRet,
                           char ** failureReasonRet)
{
  /*
  // This routine gets invoked when the remote ICE originator
  // (in this case PM) calls IceRegisterSetup() on *its* end
  // of the FWP connection; this is where the pointer to
  // client data should be initialized, so that this data
  // can be accessed when FMprocessMessages is called by
  // IceProcessMessages()
  */
  struct clientDataStruct * 	client_data;
  if ((client_data = (struct clientDataStruct *) 
		     malloc (sizeof (struct clientDataStruct))) == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "couldn't malloc client data object!");
#endif
    return (0);
  }
  /*
  // setup the client data struct; we need this object in order to
  // avoid making these variables global so they can be accessed in the
  // ICE FWPprocessMessages() callback; now you see that our global_data
  // struct was the only way of getting program data into the 
  // protocolReply setup routine!
  */
  client_data->config_info = global_data.config_info;
  client_data->nfds = global_data.nfds;
  client_data->rinit = global_data.rinit;
  client_data->winit = global_data.winit;
  client_data->major_opcode = global_data.major_opcode;
  *clientDataRet = client_data;

  return (1);
}

int doCheckServerList(char * server_address, 
		      struct clientDataStruct * program_data,
		      char ** listen_port_string)
{
  /*
  // this routine checks the server_address (provided by XFindProxy
  // and forwarded through the PM to the FWP) against the list of
  // servers to which connections have already been established;
  // it does no format type checking or conversions! (i.e., network-id 
  // vs. hostname representations); if the string received is not an 
  // exact match to one in the list, FWP will open a new connection
  // to the specified server, even though one may already exist under
  // a different name-format; all this is in a separate routine in
  // case we want to check the various formats in the future
  */
  int list_counter;

  for (list_counter = 0; list_counter < MAX_SERVERS; list_counter++)
  {
    if (server_array[list_counter] != NULL)
    { 
      if (!strcmp(server_array[list_counter]->x_server_hostport, 
	          server_address))
      {
	/*
	// allocate and return the listen_port_string 
	*/
	if ((*listen_port_string = (char *) malloc
	(strlen(server_array[list_counter]->listen_port_string) + 1)) 
	== NULL)
	{
#ifdef DEBUG
    	  fprintf(stderr, 
		"DoCheckServerList: listen_port_string malloc failed!\n");
#endif
	  return;
	}
        strcpy(*listen_port_string, 
	       server_array[list_counter]->listen_port_string);
        return SUCCESS; 
      }
    }
  }
  return FAILURE;
}


int doServerConnectSetup(char * x_server_hostport,
                         int * server_connect_fd,
                         struct sockaddr_in * server_sockaddr_in)
{
  struct hostent * 	hostptr;
  char *		server_name_base;
  char			server_port_base[10];
  int			server_port;
  int			i = 0;
  char *		tmp_str;
  char *		tmp_hostport_str;
  /*
  // need to copy the host port string because strtok() changes it   
  */
  if ((tmp_hostport_str = 
		(char *) malloc (strlen(x_server_hostport) + 1)) == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "doServerConnectSetup:  couldn't malloc hostport copy!\n");
#endif
    return FAILURE;
  }
  strcpy(tmp_hostport_str, x_server_hostport);
  tmp_str = x_server_hostport;
  while (tmp_str[i] != ':')
     tmp_str++;
  tmp_str++;
  strcpy(server_port_base, tmp_str);
  server_name_base = strtok(tmp_hostport_str,":");
  server_port = atoi(server_port_base) + 6000; 
  hostptr = gethostbyname(server_name_base);
  free(tmp_hostport_str);
  if (hostptr == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "doServerConnectSetup:  could not get server host name!\n");
#endif
    return;
  }
  if ((*server_connect_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
#ifdef DEBUG
    fprintf(stderr, 
	    "doServerConnectSetup socket() call failed: %s\n", 
	    strerror(errno)); 
#endif
    return FAILURE;
  }
  memset(server_sockaddr_in, 0, sizeof(*server_sockaddr_in));
  server_sockaddr_in->sin_family = hostptr->h_addrtype;
  memcpy((char *) &server_sockaddr_in->sin_addr, 
	 hostptr->h_addr,
	 hostptr->h_length);
  server_sockaddr_in->sin_port = htons(server_port);
  return SUCCESS;
}

int doServerConnect(int * server_connect_fd,
                    struct sockaddr_in * server_sockaddr_in)
{
  if(connect(*server_connect_fd, (struct sockaddr * )server_sockaddr_in,
	     sizeof(*server_sockaddr_in)) < 0)
  {
#ifdef DEBUG
    fprintf(stderr, 
	    "doServerConnect:  connect() call failed: %s\n", 
	    strerror(errno)); 
#endif
    return FAILURE;
  }
  return SUCCESS;
}

void  doCheckTimeouts(struct config * config_info,
                      int * nfds,
                      int * nfds_ready,
                      fd_set * rinit,
                      fd_set * winit,
                      fd_set * readable,
                      fd_set * writable,
		      struct client_conn_buf * client_conn_array[])
{
  int			client_data_counter;
  int			client_listen_counter;
  int			port_counter;
  int			pm_conn_counter;
  struct timeval 	current_time;	
  struct timezone 	current_zone;
  char			flush_buff[RWBUFFER_SIZE];
  /*
  // get current time
  */
  gettimeofday(&current_time, &current_zone); 
  /*
  // start with the clients; we have to do them all, because a 
  // timeout may occur even if the object's fd is not currently
  // readable or writable
  */
  for (client_data_counter = 0; client_data_counter < MAX_CLIENT_CONNS;
				client_data_counter++)
  {
    if (client_conn_array[client_data_counter] != NULL)
    {
      /*
      // do the shutdown time computation
      */
      if ((current_time.tv_sec 
	  - client_conn_array[client_data_counter]->creation_time) 
	  > client_conn_array[client_data_counter]->time_to_close)
      {
	/*
        // time to shut this client conn down; we're not going to be graceful
        // about it, either; we're just going to clear the select() masks for 
        // the relevant file descriptors, close these fd's and deallocate
        // the connection objects (for both client and server), and finally 
        // adjust the select() return params as necessary
        */
	FD_CLR(client_conn_array[client_data_counter]->fd, rinit);
	FD_CLR(client_conn_array[client_data_counter]->fd, winit);
        FD_CLR(client_conn_array[client_data_counter]->conn_to, rinit);
        FD_CLR(client_conn_array[client_data_counter]->conn_to, winit); 
 	close(client_conn_array[client_data_counter]->fd);
 	close(client_conn_array[client_data_counter]->conn_to);
        free(client_conn_array[client_conn_array[client_data_counter]->conn_to]);
        free(client_conn_array[client_data_counter]);
        client_conn_array[client_conn_array[client_data_counter]->conn_to] = NULL;
        client_conn_array[client_data_counter] = NULL; 
	/*
	// the nfds_ready value is tricky, because we're not sure if we got
        // a readable or writable on the associated connection for this 
        // iteration through select(); we'll decrement it one instead of two,
        // but it really doesn't matter either way given the logic of the
        // process readables and writables code
 	*/
	*nfds_ready--;	
	/*
	// if you just shut this connection object down, you don't want
        // to reset its creation date to now, so go to the next one
        */
 	continue;	
      }
      /*
      // recompute select() timeout to maximize blocking time without
      // preventing timeout checking
      */
      config_info->select_timeout.tv_sec = 
			min(config_info->select_timeout.tv_sec,
			client_conn_array[client_data_counter]->time_to_close -
			(current_time.tv_sec  - 
			client_conn_array[client_data_counter]->creation_time));
      /*
      // this wasn't a shutdown case, so check to see if there's activity
      // on the fd; if so, then reset the creation time field to now 
      */
      if (FD_ISSET(client_conn_array[client_data_counter]->fd, readable) ||
	  FD_ISSET(client_conn_array[client_data_counter]->fd, writable)) 
	client_conn_array[client_data_counter]->creation_time = current_time.tv_sec;
      /*
      // do the same thing with the conn_to connections, but only
      // if they haven't already been marked for closing
      */
      if ((client_conn_array[client_data_counter]->conn_to) > 0)
      { 
      
        if ((FD_ISSET(client_conn_array[client_data_counter]->conn_to, 
								readable)) || 
	    (FD_ISSET(client_conn_array[client_data_counter]->conn_to, 
								writable)))
	  client_conn_array[client_data_counter]->creation_time = 
							current_time.tv_sec;
      }
    }
  } 
  /*
  // now do the client listen fds; as with the client data objects, 
  // we have to do them all, because a timeout may occur even if the
  // object's fd is not currently readable or writable
  */
  for (client_listen_counter = 0; client_listen_counter < MAX_CLIENT_LISTEN_PORTS;
				client_listen_counter++)
  {
    if (server_array[client_listen_counter] != NULL)
    {
      /*
      // do the shutdown time computation
      */
      if ((current_time.tv_sec 
	  - server_array[client_listen_counter]->creation_time) 
	  > server_array[client_listen_counter]->time_to_close)
      {
        /*
        // reclaim this listen port for future allocation
        */
	for (port_counter = 0; port_counter < MAX_CLIENT_LISTEN_PORTS; 
							port_counter++)
        {
          if (port_array[port_counter].client_listen_fd ==
			server_array[client_listen_counter]->client_listen_fd)
	  {
	    port_array[port_counter].state = AVAILABLE;
	    break;
 	  }
        }
	/*
        // time to shut this listener down just like we did above;
        // we close the server connection here as well but we don't
        // need to worry about the select() mask because we're not
        // using the server fd in this object for reading or writing --
        // only to initialize the client data object server connections
        */
	FD_CLR(server_array[client_listen_counter]->client_listen_fd, rinit);
	FD_CLR(server_array[client_listen_counter]->client_listen_fd, winit);
 	close(server_array[client_listen_counter]->client_listen_fd);
        free(server_array[client_listen_counter]);
        server_array[client_listen_counter] = NULL; 
	*nfds_ready--;	
	/*
	// if you just shut this connection object down, you don't want
        // to reset its creation date to now, so go to the next one
        */
 	continue;	
      }
      /*
      // recompute select() timeout to maximize blocking time without
      // preventing timeout checking
      */
      config_info->select_timeout.tv_sec = 
			min(config_info->select_timeout.tv_sec,
			server_array[client_listen_counter]->time_to_close -
			(current_time.tv_sec  - 
			server_array[client_listen_counter]->creation_time));
      /*
      // this wasn't a shutdown case, so check to see if there's activity
      // on the fd; if so, then reset the creation time field to now 
      */
      if (FD_ISSET(server_array[client_listen_counter]->client_listen_fd, 
								readable) ||
	  FD_ISSET(server_array[client_listen_counter]->client_listen_fd, writable))
	server_array[client_listen_counter]->creation_time = 
						current_time.tv_sec;
    }
  } 
  /*
  // last of all the pm connection fds
  */
  for (pm_conn_counter = 0; pm_conn_counter < MAX_PM_CONNS;
				pm_conn_counter++)
  {
    if (pm_conn_array[pm_conn_counter] != NULL)
    {
      /*
      // do the shutdown time computation
      */
      if ((current_time.tv_sec 
	  - pm_conn_array[pm_conn_counter]->creation_time) 
	  > pm_conn_array[pm_conn_counter]->time_to_close)
      {
	/*
        // shut this connection down just like the others 
        */
	FD_CLR(pm_conn_array[pm_conn_counter]->fd, rinit);
	FD_CLR(pm_conn_array[pm_conn_counter]->fd, winit);
 	close(pm_conn_array[pm_conn_counter]->fd);
        free(pm_conn_array[pm_conn_counter]);
        pm_conn_array[pm_conn_counter] = NULL;
	*nfds_ready--;	
	/*
	// if you just shut this connection object down, you don't want
        // to reset its creation date to now, so go to the next one
        */
 	continue;	
      }
      /*
      // recompute select() timeout to maximize blocking time without
      // preventing timeout checking
      */
      config_info->select_timeout.tv_sec = 
			min(config_info->select_timeout.tv_sec,
			pm_conn_array[pm_conn_counter]->time_to_close -
			(current_time.tv_sec  - 
			pm_conn_array[pm_conn_counter]->creation_time));
      /*
      // this wasn't a shutdown case, so check to see if there's activity
      // on the fd; if so, then reset the creation time field to now 
      */
      if (FD_ISSET(pm_conn_array[pm_conn_counter]->fd, readable) || 
	  FD_ISSET(pm_conn_array[pm_conn_counter]->fd, writable)) 
	pm_conn_array[pm_conn_counter]->creation_time = current_time.tv_sec;
    }
  }
}

int doHandleConfigFile (struct config * config_info)
{
  FILE *			stream;
  char				line[128];
  char *			status;
  int				num_chars = 120;
  int				line_number = 0;

  if (!config_info->config_file_path)
      return 1;

  if ((stream = fopen(config_info->config_file_path, "r")) == NULL)
  {
    perror("Could not open config file");
    return 0;
  }
  while (1) 
  { 
    if ((status = fgets(line, num_chars, stream)) == NULL) 
    {
#ifdef DEBUG
      fprintf(stderr, "Read returned NULL!\n");
#endif
      break;
    }
    else
#ifdef DEBUG
      fprintf(stdout, line);
#endif
    line_number++;
    if (!doProcessLine(line, config_info, line_number))
    {
      fprintf(stderr,"Config file format error. Parse failed.\n");
#ifndef DEBUG
      fprintf(stderr, "line: %s\n", line);
#endif
      fclose(stream);
      return 0;
    }
  } 
  if (!feof(stream))
  {
      fputs("Error parsing config file; not at eof\n",stderr);
      fclose(stream);
      return 0;
  }

  if (printConfigVerify)
      fprintf(stderr, "%d rules read; ready\n", config_info->rule_count);

  fclose(stream);
  return 1;
}

int doInitNewRule(struct config *config_info)
{
  int rule_number = config_info->rule_count;
  struct config_line *config_lineP;

  if (rule_number == config_info->lines_allocated)
  {
      config_info->config_file_data = (struct config_line**)
	  Realloc((char*)config_info->config_file_data,
		  (config_info->lines_allocated += ADD_LINES) *
		      sizeof(struct config_line *));
  }
	
  config_lineP = (struct config_line *) Malloc (sizeof(struct config_line));

  config_lineP->permit_deny = NULL;
  config_lineP->source_hostname = NULL;
  config_lineP->source_host = 0;
  config_lineP->source_netmask = NULL;
  config_lineP->source_net = 0;
  config_lineP->dest_hostname = NULL;
  config_lineP->dest_host = 0;
  config_lineP->dest_netmask = NULL;
  config_lineP->dest_net = 0;
  config_lineP->operator = NULL;
  config_lineP->service = NULL; 

  config_info->config_file_data[rule_number] = config_lineP;

  return rule_number;
}

int doProcessLine(char *line,
		  struct config *config_info,
		  int config_line)
{
  char * 	result;
  int		bad_parse = 0;

  if (line[0] == '#' || line[0] == '\n')
      return 1;

  if ((result = strtok(line, SEPARATOR1)) != NULL)
  {
    if (!(strcmp("permit", result)) || (!strcmp("deny", result)))
    {
	bad_parse = doConfigPermitDeny(config_info, result);
    }
    else
    if (!strcmp("require", result) || !strcmp("disallow", result))
	bad_parse = doConfigRequireDisallow(config_line, result);

    else
	bad_parse = 1;
  }

  if (bad_parse)
     return 0;	 
  else
     return 1;
}

int doConfigPermitDeny(struct config *config_info, char *result)
{
  struct config_line ** config_file_data;
  int		line_number;
  int		bad_token = 0;

  /* caution; config_info->config_file_data can move in doInitNewRule */
  line_number = doInitNewRule(config_info);

  config_file_data = config_info->config_file_data;

  if ((config_file_data[line_number]->permit_deny = 
       (char *) malloc (strlen(result) + 1)) == NULL)
  {
#ifdef DEBUG
    fprintf(stderr, "doConfigPermitDeny:  token malloc failed!\n");
#endif
    return 0;
  }
  strcpy(config_file_data[line_number]->permit_deny, result);
#ifdef DEBUG
  fprintf(stderr, 
	  "first token = %s\n", 
	  config_file_data[line_number]->permit_deny);
#endif

  /*
  // do the source hostname field
  */
  if ((result = strtok(NULL, SEPARATOR1)) != NULL)
  {
    char   	token[64];
    strcpy(token, result);
    if (doVerifyHostMaskToken(token))
    {
      if ((config_file_data[line_number]->source_hostname = 
				      (char *) malloc (strlen(result) + 1)) == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, "doConfigPermitDeny:  token malloc failed!\n");
#endif
	return 0;
      }
      strcpy(config_file_data[line_number]->source_hostname, result);
#ifdef DEBUG
      fprintf(stderr, 
	   "second token = %s\n", 
	    config_file_data[line_number]->source_hostname);
#endif
      /*
      // generate network address format
      */
      config_file_data[line_number]->source_host = 
	      inet_addr(config_file_data[line_number]->source_hostname);
    } else
      bad_token = 1;
  }

  /*
  // now the source netmask field
  */
  if ((result = strtok(NULL, SEPARATOR1)) != NULL)
  {
    char   	token[64];
    strcpy(token, result);
    if (doVerifyHostMaskToken(token))
    {
      if ((config_file_data[line_number]->source_netmask = 
				      (char *) malloc (strlen(result) + 1)) == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, "doConfigPermitDeny:  token malloc failed!\n");
#endif
	return 0;
      }
      strcpy(config_file_data[line_number]->source_netmask, result);
#ifdef DEBUG
      fprintf(stderr, 
	   "third token = %s\n", 
	    config_file_data[line_number]->source_netmask);
#endif
      config_file_data[line_number]->source_net = 
	      inet_addr(config_file_data[line_number]->source_netmask);
    } else
      bad_token = 1;
  }
  /*
  // now the destination hostname field
  */
  if ((result = strtok(NULL, SEPARATOR1)) != NULL)
  {
    char   	token[64];
    strcpy(token, result);
    if (doVerifyHostMaskToken(token))
    {
      if ((config_file_data[line_number]->dest_hostname = 
				      (char *) malloc (strlen(result) + 1)) == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, "doConfigPermitDeny:  token malloc failed!\n");
#endif
	return 0;
      }
      strcpy(config_file_data[line_number]->dest_hostname, result);
#ifdef DEBUG
      fprintf(stderr, 
	   "fourth token = %s\n", 
	    config_file_data[line_number]->dest_hostname);
#endif
      config_file_data[line_number]->dest_host = 
	      inet_addr(config_file_data[line_number]->dest_hostname);
    } else
      bad_token = 1;
  }
  /*
  // now the destination netmask field
  */
  if ((result = strtok(NULL, SEPARATOR1)) != NULL)
  {
    char   	token[64];
    strcpy(token, result);
    if (doVerifyHostMaskToken(token))
    {
      if ((config_file_data[line_number]->dest_netmask = 
				      (char *) malloc (strlen(result) + 1)) == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, "doConfigPermitDeny:  token malloc failed!\n");
#endif
	return 0;
      }
      strcpy(config_file_data[line_number]->dest_netmask, result);
#ifdef DEBUG
      fprintf(stderr, 
	   "fifth token = %s\n", 
	    config_file_data[line_number]->dest_netmask);
#endif
      config_file_data[line_number]->dest_net = 
	      inet_addr(config_file_data[line_number]->dest_netmask);
    } else
      bad_token = 1;
  }
  /*
  // now the operator field
  */
  if ((result = strtok(NULL, SEPARATOR1)) != NULL)
  {
    if (!strcmp("eq", result))
    {
      if ((config_file_data[line_number]->operator = 
				      (char *) malloc (strlen(result) + 1)) == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, "doConfigPermitDeny:  token malloc failed!\n");
#endif
	return 0;
      }
      strcpy(config_file_data[line_number]->operator, result);
#ifdef DEBUG
      fprintf(stderr, 
	      "sixth token = %s\n", 
	      config_file_data[line_number]->operator);
#endif
    } else
      bad_token = 1;
  }
  /*
  // and finally the service field
  */
  if ((result = strtok(NULL, SEPARATOR1)) != NULL)
  {
    if (!(strncmp("pm", result, 2)) || (!strncmp("fp", result, 2)) ||
				   (!strncmp("cd", result, 2)))	
    {
      if ((config_file_data[line_number]->service = 
				      (char *) malloc (strlen(result) + 1)) == NULL)
      {
#ifdef DEBUG
	fprintf(stderr, "doConfigPermitDeny:  token malloc failed!\n");
#endif
	return 0;
      }
      strcpy(config_file_data[line_number]->service, result);
#ifdef DEBUG
      fprintf(stderr, 
	      "seventh token = %s\n", 
	      config_file_data[line_number]->service);
#endif
      /*
      // load the appropriate service id
      */
      if (!strncmp(config_file_data[line_number]->service, "pm", 2))
	config_file_data[line_number]->service_id = PMGR;
      else if (!strncmp(config_file_data[line_number]->service, "fp", 2))
	config_file_data[line_number]->service_id = FINDPROXY;
      else 
	if (!strncmp(config_file_data[line_number]->service, "cd", 2))
	  config_file_data[line_number]->service_id = CLIENT;
    } else
      bad_token = 1;
  }

  /*
  // rules for bad parse
  */
  if (bad_token ||
      (config_file_data[line_number]->permit_deny == NULL) ||
      ((config_file_data[line_number]->permit_deny != NULL) && 
       (config_file_data[line_number]->source_hostname == NULL)) ||
      ((config_file_data[line_number]->source_hostname != NULL) && 
       (config_file_data[line_number]->source_netmask == NULL)) || 
      ((config_file_data[line_number]->dest_hostname != NULL) && 
       (config_file_data[line_number]->dest_netmask == NULL)) || 
      ((config_file_data[line_number]->operator != NULL) && 
       (config_file_data[line_number]->service == NULL)))
      return 1;

  config_info->rule_count++;
  return 0;
}

int doVerifyHostMaskToken(char token[])
{
  char * result;
  int	 delimiter_count = 0;
  /*
  // verify there are 3 "." delimiters in the token
  */
  while (token)
  {
    if ((result = strchr(token, SEPARATOR2)) != NULL)
    {
      token = result;
      delimiter_count++;
      token ++;
    } else
      token = result;
  }
  if ((delimiter_count < 3) || (delimiter_count > 3))
    return 0; 
  else
    return 1;
}

void BadSyntax(char *msg, int line)
{
#ifdef DEBUG
    fprintf(stderr, "Config error: %s at line %d\n", msg, line);
#endif
}

void BadMalloc(int line)
{
    fprintf(stderr, "Error: memory exhaused at line %d\n", line);
}

Bool doConfigRequireDisallow(int line, char* result)
{
  Bool	permit = (strcmp("require", result) == 0);

  if (((result = strtok(NULL, SEPARATOR1)) == NULL) ||
      (strcmp(result, "sitepolicy") != 0))
  {
      BadSyntax("require/disallow must specify \"sitepolicy\"", line);
      return 1;
  }

  if (HaveSitePolicy && (SitePolicyPermit != permit))
  {
      BadSyntax("can't mix require and disallow policies", line);
      return 1;
  }

  HaveSitePolicy = True;
  SitePolicyPermit = permit;

  if ((result = strtok(NULL, " \n")) == NULL)
  {
      BadSyntax("missing policy string after \"sitepolicy\"", line);
      return 1;
  }

  if (SitePolicies)
    SitePolicies = (char**)realloc((char**)SitePolicies,
				   (SitePolicyCount+1) * sizeof(char*));
  else
    SitePolicies = (char**)malloc(sizeof(char*));

  if (!SitePolicies)
  {
      BadMalloc(line);
      return 1;
  }

  SitePolicies[SitePolicyCount] = malloc(strlen(result));

  if (!SitePolicies[SitePolicyCount])
  {
      BadMalloc(line);
      return 1;
  }

  strcpy(SitePolicies[SitePolicyCount++], result);

#ifdef DEBUG
  fprintf(stderr, "%s %s", permit ? "requiring" : "disallowing", result);
#endif

  return False;;
}

void doPrintEval(struct config * config_info, int line_counter)
{
  struct config_line *ruleP = config_info->config_file_data[line_counter];
  fprintf(stderr,"matched: %s %s %s %s %s %s %s\n",
	  ruleP->permit_deny,
	  ruleP->source_hostname, ruleP->source_netmask,
	  ruleP->dest_hostname, ruleP->dest_netmask,
	  ruleP->operator,
	  ruleP->service);
}

/*
 * The real way to handle IO errors is to check the return status
 * of IceProcessMessages.  xsm properly does this.
 *
 * Unfortunately, a design flaw exists in the ICE library in which
 * a default IO error handler is invoked if no IO error handler is
 * installed.  This default handler exits.  We must avoid this.
 *
 * To get around this problem, we install an IO error handler that
 * does a little magic.  Since a previous IO handler might have been
 * installed, when we install our IO error handler, we do a little
 * trick to get both the previous IO error handler and the default
 * IO error handler.  When our IO error handler is called, if the
 * previous handler is not the default handler, we call it.  This
 * way, everyone's IO error handler gets called except the stupid
 * default one which does an exit!
 */

static IceIOErrorHandler prev_handler;

void
MyIoErrorHandler (ice_conn)

IceConn ice_conn;

{
    if (prev_handler)
        (*prev_handler) (ice_conn);
}

void doInstallIOErrorHandler ()

{
    IceIOErrorHandler default_handler;

    prev_handler = IceSetIOErrorHandler (NULL);
    default_handler = IceSetIOErrorHandler (MyIoErrorHandler);
    if (prev_handler == default_handler)
        prev_handler = NULL;
#ifdef X_NOT_POSIX
    signal(SIGPIPE, SIG_IGN);
#else
    {
        struct sigaction act;

        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        act.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &act, NULL);
    }
#endif
}

