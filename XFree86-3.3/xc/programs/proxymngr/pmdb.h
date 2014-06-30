/* $XConsortium: pmdb.h /main/9 1996/10/29 10:03:33 swick $ */

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


#include <X11/ICE/ICElib.h>


typedef struct _server_list {
    char *serverAddress;
    struct _server_list *next;
} server_list;


typedef struct _request_list {
    char *serviceName;
    char *serverAddress;
    char *hostAddress;
    char *startOptions;
    PMconn *requestor;
    char *listData;
    int authLen;
    char *authName;
    char *authData;
    struct _request_list *next;
} request_list;


typedef struct _running_proxy {
    Bool active;
    PMconn *pmConn;
    request_list *requests;
    server_list *servers;
    Bool refused_service;
    struct _running_proxy *next;
} running_proxy;


typedef struct _proxy_service {
    char *serviceName;
    int proxyCount;
    running_proxy *proxyList;
    struct _proxy_service *next;
} proxy_service;


typedef struct {
    int count;
    int current;
    running_proxy **list;
} running_proxy_list;



proxy_service
*FindProxyService (
	char *serviceName,
	Bool createIf);

running_proxy *
StartNewProxy (
	char *serviceName,
	char *startCommand);

running_proxy *
ConnectToProxy (
	int pmOpcode,
	char *serviceName,
	char *proxyAddress);

Status
ActivateProxyService (
	char *serviceName,
	PMconn *proxy);

void
ProxyGone (
    IceConn proxyIceConn,
    Bool *activeReqs);

running_proxy_list *
GetRunningProxyList (
	char *serviceName,
	char *serverAddress);

void
FreeProxyList (
	running_proxy_list *list);

Status
PushRequestorQueue (
	running_proxy *proxy,
	PMconn *requestor,
	running_proxy_list *runList,
	char *serviceName,
	char *serverAddress,
	char *hostAddress,
	char *startOptions,
	int authLen,
	char *authName,
	char *authData);


Status
PeekRequestorQueue (
	PMconn *proxy,
	PMconn **requestor,
	running_proxy_list **runList,
	char **serviceName,
	char **serverAddress,
	char **hostAddress,
	char **startOptions,
	int *authLen,
	char **authName,
	char **authData);

PMconn*
PopRequestorQueue (
	PMconn *pmConn,
	Bool addServer,
	Bool freeProxyList);

running_proxy *
ProxyForPMconn(
	PMconn *pmConn);
