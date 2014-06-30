/* $XConsortium: xfwp.h /main/18 1996/12/12 16:57:17 swick $ */

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

/*
// the following are timeout defaults (in seconds) on the three
// types of ports (pm-data, client-listen and client-data)
*/
#define 	PM_DATA_TIMEOUT_DEFAULT			3600	/* 1 hour */
#define 	CLIENT_LISTEN_TIMEOUT_DEFAULT		86400	/* 24 hours */
#define 	CLIENT_DATA_TIMEOUT_DEFAULT		604800  /* 1 week */
/*
// connection object defines
*/
#define	 	MAX_SERVERS			100
#define         MAX_CLIENT_CONNS        	4096
#define		MAX_PM_CONNS			100
#define		MAX_PM_HOSTS			10
#define		MAX_CLIENT_LISTEN_PORTS		100
#define		MAX_HOSTNAME_LEN		256
#define		MAX_TRANSPORTS			10
#define		SERVER_REPLY_FAILURE		0
#define		SERVER_REPLY_SUCCESS		1
#define		SERVER_REPLY_AUTHENTICATE 	2	
#define		RWBUFFER_SIZE			2048
#define		PM_LISTEN_PORT			"4444"
/*
// config file parser support defines
*/
/* allocate ADD_LINES entries at a time when loading the configuration */
#define ADD_LINES		20
#define SEPARATOR1              " \t\n"
#define SEPARATOR2              '.'

#define		min(a,b)		((a) < (b) ? (a) : (b))
#define		max(a,b)		((a) > (b) ? (a) : (b))

typedef void fp1();
typedef Bool fp2();

struct PMconn
{
  IceConn	ice_conn;
  int		proto_major_version;
  int		proto_minor_version;
  char *	vendor;
  char * 	release;
} PMconn;

struct ICE_setup_info
{
        
 int       opcode;
 int       versionCount;
 IcePaVersionRec PMVersions[1];
 void (*fp1) ();
 Bool (*fp2) ();
};

struct listen_port_info
{
  int	client_listen_fd;
  int	state;
};

struct pm_policy 
{
  int access_mask;
  struct hostent mask_info;
};

enum CLIENT_CONN_STATE {CLIENT_WAITING, SERVER_WAITING, SERVER_REPLY,
		        CONNECTION_READY};
enum PM_CONN_STATE {START, WAIT_SERVER_INFO, SENT_PORT_INFO, PM_EXCHANGE_DONE};
enum CONFIG_CHECK {FAILURE, SUCCESS};
enum CONFIG_TYPE {PM, REM_CLIENT};
enum LISTEN_STATE {AVAILABLE, IN_USE};
enum SERVICE_ID_TYPES {CLIENT, PMGR, FINDPROXY};

struct ice_data
{
  char * server_host_name;
} ice_data;

struct client_conn_buf
{
  char 			readbuf[RWBUFFER_SIZE];
  char 			writebuf[RWBUFFER_SIZE];
  int 			rbytes;
  int 			wbytes;
  int 			conn_to;
  int 			wclose;
  int    		state;  
  int			time_to_close;
  int			creation_time;
  int			fd;
};

struct pm_conn_buf
{
  char 			readbuf[RWBUFFER_SIZE];
  int 			rbytes;
  int    		state;  
  int			fd;
  IceConn  		ice_conn;
  int			creation_time;
  int			time_to_close;
} pm_conn_buf;


struct config
{
  int 			num_client_conns;
  int 			num_pm_conns;
  int 			num_servers;
  int			num_transports;
  int			idle_timeout;
  int 			pm_data_timeout;
  int 			client_listen_timeout;
  int 			client_data_timeout;
  struct timeval	select_timeout;
  char 			pm_listen_port[6];
  char *  		config_file_path;
  int			lines_allocated;
  int			rule_count;
  struct config_line **  config_file_data;
};

struct server_list
{
  char * 		x_server_hostport;
  char *		listen_port_string;
  int			client_listen_fd;
  int 			server_fd;
  int			done_accept;
  int			creation_time;
  int			time_to_close;
};	

struct clientDataStruct
{
  struct pm_conn_buf *    pm_conn_array[MAX_PM_CONNS];
  struct server_list *    server_array[MAX_SERVERS];
  struct listen_port_info port_array[MAX_CLIENT_LISTEN_PORTS];
  int 			  pm_connection_fd;
  int *			  nfds;
  fd_set *		  rinit;
  fd_set *		  winit;
  int			  major_opcode;
  struct config *	  config_info;
};

struct config_line
{
  char * 		permit_deny;
  char * 		source_hostname;
  unsigned int		source_host;
  char * 		source_netmask;
  unsigned int  	source_net;
  char * 		dest_hostname;
  unsigned int 	 	dest_host;
  char * 		dest_netmask;
  unsigned int	 	dest_net;
  char * 		operator;
  char * 		service;
  int	 		service_id;
};

/*
//FUNCTION PROTOTYPES 
*/
void  doProcessInputArgs(struct config * config_info, 
                         int argc, 
                         char * argv[]);

void  doInitDataStructs(struct config * config_info, 
                        struct client_conn_buf * client_conn_array[],
                        struct ice_data * ice_data,
			struct ICE_setup_info * PM_conn_setup,
			int pm_listen_array[]);

int   doSetupPMListen(char * pm_port, 
		      int pm_listen_array[],
		      IceListenObj ** listen_objects,
		      int * nfds,
		      fd_set * rinit);

int   doSetupRemClientListen(char ** listen_port_string,
                             struct clientDataStruct * program_data,
                             char * server_address);

int  doCheckServerList(char * serverAddress, 
		       struct clientDataStruct * program_data,
		       char ** listen_port_string);

int  doHandleConfigFile(struct config * config_info);

void  doSelect(struct config * config_info,
               int * nfds,
               int * nfds_ready,
               fd_set * readable,
               fd_set * writable);

void  doCheckTimeouts(struct config * config_info,
                      int * nfds,
                      int * nfds_ready,
                      fd_set * rinit,
                      fd_set * winit,
                      fd_set * readable,
                      fd_set * writable,
		      struct client_conn_buf * client_conn_array[]);

void  doProcessSelect(int * nfds,
                      int * nfds_ready,
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
		      IceListenObj ** listen_objects);

void  doProcessReadables(int fd_counter,
                      int * nfds,
                      int * nfds_ready,
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
		      IceListenObj ** listen_objects);

void  doProcessWritables(int fd_counter,
                      int * nfds,
                      int * nfds_ready,
                      fd_set * readable,
                      fd_set * writable,
                      fd_set * rinit,
                      fd_set * winit,
                      int  pm_listen_array[],
                      int * pm_conn_counter,
                      int * rem_listen_counter,
                      struct config * config_info,
                      struct client_conn_buf * client_conn_array[],
                      struct ice_data * ice_data);

int doConfigCheck(struct sockaddr_in * temp_sockaddr_in,
		  struct sockaddr_in * server_sockaddr_in,
                  struct config * config_info,
		  int context);
/*
// Need to put the ICE callback function declaration here
// (Don't forget call to the ICE library to register callback!)
*/

int doCopyFromTo(int fd_from,
                 int fd_to,
                 struct client_conn_buf * client_conn_array[],
                 fd_set * rinit,
                 fd_set * winit);

void FWPprocessMessages(IceConn ice_conn, 
			IcePointer * client_data,
			int opcode,
			unsigned long length,
			Bool swap);

Status FWPprotocolSetupProc(IceConn iceConn,
			   int major_version,
			   int minor_version,
			   char * vendor,
			   char * release,
			   IcePointer * clientDataRet,
			   char ** failureReasonRet);

Bool FWPHostBasedAuthProc(char * hostname);

int doServerConnectSetup(char * x_server_hostport, 
			 int * server_connect_fd, 
			 struct sockaddr_in * server_addr_in);

int doServerConnect(int * server_connect_fd, 
		    struct sockaddr_in * server_addr_in);

int doProcessLine(char *line,
                   struct config * config_info,
                   int line_number);

int doInitNewRule(struct config *);

int doConfigPermitDeny(struct config *, char*);

Bool doConfigRequireDisallow(int, char*);

int doVerifyHostMaskToken(char token[]);

void doInstallIOErrorHandler();

/*
 * Handy ICE message parsing macros
 */

/*
 * Pad to a 64 bit boundary
 */

#define PAD64(_bytes) ((8 - ((unsigned int) (_bytes) % 8)) % 8)

#define PADDED_BYTES64(_bytes) (_bytes + PAD64 (_bytes))


/*
 * Number of 8 byte units in _bytes.
 */

#define WORD64COUNT(_bytes) (((unsigned int) ((_bytes) + 7)) >> 3)


/*
 * Compute the number of bytes for a STRING representation
 */

#define STRING_BYTES(_str) (2 + (_str ? strlen (_str) : 0) + \
		     PAD64 (2 + (_str ? strlen (_str) : 0)))



#define SKIP_STRING(_pBuf, _swap) \
{ \
    CARD16 _len; \
    EXTRACT_CARD16 (_pBuf, _swap, _len); \
    _pBuf += _len; \
    if (PAD64 (2 + _len)) \
        _pBuf += PAD64 (2 + _len); \
}

/*
 * STORE macros
 */

#define STORE_CARD16(_pBuf, _val) \
{ \
    *((CARD16 *) _pBuf) = _val; \
    _pBuf += 2; \
}

#define STORE_STRING(_pBuf, _string) \
{ \
    int _len = _string ? strlen (_string) : 0; \
    STORE_CARD16 (_pBuf, _len); \
    if (_len) { \
        memcpy (_pBuf, _string, _len); \
        _pBuf += _len; \
    } \
    if (PAD64 (2 + _len)) \
        _pBuf += PAD64 (2 + _len); \
}


/*
 * EXTRACT macros
 */

#define EXTRACT_CARD16(_pBuf, _swap, _val) \
{ \
    _val = *((CARD16 *) _pBuf); \
    _pBuf += 2; \
    if (_swap) \
        _val = lswaps (_val); \
}

#define EXTRACT_STRING(_pBuf, _swap, _string) \
{ \
    CARD16 _len; \
    EXTRACT_CARD16 (_pBuf, _swap, _len); \
    _string = (char *) malloc (_len + 1); \
    memcpy (_string, _pBuf, _len); \
    _string[_len] = '\0'; \
    _pBuf += _len; \
    if (PAD64 (2 + _len)) \
        _pBuf += PAD64 (2 + _len); \
}


/*
 * Byte swapping
 */

/* byte swap a long literal */
#define lswapl(_val) ((((_val) & 0xff) << 24) |\
		   (((_val) & 0xff00) << 8) |\
		   (((_val) & 0xff0000) >> 8) |\
		   (((_val) >> 24) & 0xff))

/* byte swap a short literal */
#define lswaps(_val) ((((_val) & 0xff) << 8) | (((_val) >> 8) & 0xff))


#define CHECK_AT_LEAST_SIZE(_iceConn, _majorOp, _minorOp, _expected_len, _actual_len, _severity) \
    if ((((_actual_len) - SIZEOF (iceMsg)) >> 3) > _expected_len) \
    { \
       _IceErrorBadLength (_iceConn, _majorOp, _minorOp, _severity); \
       return; \
    }


#define CHECK_COMPLETE_SIZE(_iceConn, _majorOp, _minorOp, _expected_len, _actual_len, _pStart, _severity) \
    if (((PADDED_BYTES64((_actual_len)) - SIZEOF (iceMsg)) >> 3) \
        != _expected_len) \
    { \
       _IceErrorBadLength (_iceConn, _majorOp, _minorOp, _severity); \
       IceDisposeCompleteMessage (iceConn, _pStart); \
       return; \
    }
