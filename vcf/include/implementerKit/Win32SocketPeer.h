/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

// Win32SocketPeer.h

#ifndef _WIN32SOCKETPeer_H__
#define _WIN32SOCKETPeer_H__


#include "SocketPeer.h"


namespace VCFNet
{

class SocketObject;

class Socket;

#define MAX_PENDING_CONNECTS SOMAXCONN

class NETKIT_API Win32SocketPeer : public SocketPeer {
public:
	Win32SocketPeer( Socket* socket, const VCF::String& host, const int& port );

	Win32SocketPeer( Socket* socket, const int& socketPeerID, const VCF::String& host, const int& port );

	virtual ~Win32SocketPeer();

	virtual SocketState startListening();

	virtual int send( const char* bytes, const int& size );
	

	virtual SocketState stopListening();

	virtual void setPort( const int& port );

	virtual int getPort();
	
	virtual void setHost( const VCF::String& host );

	virtual VCF::String getHost();

	static void startup();

	SocketState connectTo( const VCF::String& hostName, const int& port );

	virtual bool hasWaitingClients();

	virtual Socket* getClient();

	virtual void checkForPendingData( const int& waitAndBlock );

	virtual int getSocketPeerID();

	static VCF::String getLocalMachineName();
protected:
	void readDataFromClient( Socket* client, fd_set* readfd );
private:
	struct sockaddr_in m_sin;
	std::vector<Socket*> m_connectedClients;
	std::vector<Socket*> m_disconnectedClients;
	Socket* m_socket;
	VCF::String m_host;
	int m_port;
	SOCKET  m_sock;
	static bool WinSockStarted;
};


};

#endif 
