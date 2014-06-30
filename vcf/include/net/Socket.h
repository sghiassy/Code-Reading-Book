// Socket.h

#ifndef _SOCKET_H__
#define _SOCKET_H__


#include "SocketListener.h"
#include "SocketEvent.h"	

using namespace VCF;

namespace VCFNet
{
	
//common socket errors
#define VCFNET_STARTUP_FAILED		-1
#define VCFNET_CANT_OPEN_SOCKET		-2
#define VCFNET_HOSTNAME_ERROR		-3
#define VCFNET_CONNECT_ERROR		-4
#define VCFNET_CONNECT_SUCCESS		-5
#define VCFNET_BIND_FAILED			-6
#define VCFNET_SERVER_STARTED		-7
#define VCFNET_NOHOST				""
#define VCFNET_NOPORT				-1

#define VCFNET_INFINITEWAIT			-1
#define VCFNET_ASYNCHRONOUSWAIT		-2

enum SocketState{
	SOCKET_CLOSED_ERROR=0,
	SOCKET_READ_ERROR,
	SOCKET_WRITE_ERROR,
	SOCKET_OPEN_ERROR,
	SOCKET_NAME_ERROR,
	SOCKET_CREATE_ERROR,
	SOCKET_CONNECT_ERROR,
	SOCKET_WRITING,
	SOCKET_READING,
	SOCKET_CONNECTED,
	SOCKET_LISTENING,
	SOCKET_CANT_OPEN_SOCKET,
	SOCKET_HOSTNAME_ERROR,
	SOCKET_CLOSED,
	SOCKET_ALREADY_LISTENING_ERROR,
	SOCKET_BIND_FAILED
};

static VCF::String SocketStateNames[] ={"SOCKET_CLOSED_ERROR" ,
                            "SOCKET_READ_ERROR",
							"SOCKET_WRITE_ERROR",
							"SOCKET_OPEN_ERROR",
							"SOCKET_NAME_ERROR",
							"SOCKET_CREATE_ERROR",
							"SOCKET_CONNECT_ERROR",
							"SOCKET_WRITING",
							"SOCKET_READING",
							"SOCKET_CONNECTED",
							"SOCKET_LISTENING",
							"SOCKET_CANT_OPEN_SOCKET",
							"SOCKET_HOSTNAME_ERROR",
							"SOCKET_CLOSED",
							"SOCKET_ALREADY_LISTENING_ERROR",
							"SOCKET_BIND_FAILED" };

#define VCFNET_MAX_SOCKET_SIZE		512

class SocketPeer;

class SocketListeningLoop;

/**
*Socket Object represents a connected client to the listening socket.
*On Win32 the m_socketID represents a handle to a SOCKET structure
*this object is usually returns via a call to Socket::getClient()
*/
class NETKIT_API SocketObject {
public:
	SocketObject( const int& id=0 ) {
		m_socketID = id;
	}

	virtual ~SocketObject(){};

	int m_socketID;
};

class NETKIT_API Socket : public VCF::Object  
{
public:
	Socket( const int& port=0, const bool& startAsServer=false );

	Socket( const VCF::String& host, const int& port, const bool& startAsServer=false );

	Socket( const int& socketPeerID, const VCF::String& host, const int& port );

	virtual ~Socket();	

	int getPort(){
		return m_port;
	}

	void setPort( const int& port );

	VCF::String getHost(){
		return m_host;		
	}

	void setHost( const VCF::String& host );

	int send( const char* bytes, const int& size );

	SocketState startListening();

	SocketState stopListening();

	void init();

	SocketState getState(){
		return m_state;
	}

	void setState( const SocketState& state )	{
		m_state = state;
	}

	virtual VCF::String toString();

	bool hasWaitingClients();

	Socket* getClient();

	/**
	*checks if there is any data for the socket
	*@param int waitAndBlock repesents the number of 
	*milliseconds to wait and block for data. The default
	*value for this is VCFNET_ASYNCHRONOUSWAIT, which is 
	*/
	void checkForPendingData( const int& waitAndBlock=VCFNET_ASYNCHRONOUSWAIT );

	bool isServer() {
		return m_isServer;
	}

	void setAsServer( const bool& isServer ) {
		m_isServer = isServer;
	}

	EVENT_HANDLER_LIST(DataReceived)
	EVENT_HANDLER_LIST(ClientConnected)
	EVENT_HANDLER_LIST(ClientDisconnected)

	ADD_EVENT(DataReceived) 
	ADD_EVENT(ClientConnected) 
	ADD_EVENT(ClientDisconnected) 
	
	REMOVE_EVENT(DataReceived) 
	REMOVE_EVENT(ClientConnected) 
	REMOVE_EVENT(ClientDisconnected) 

	FIRE_EVENT(DataReceived,SocketEvent) 
	FIRE_EVENT(ClientConnected,SocketEvent) 
	FIRE_EVENT(ClientDisconnected,SocketEvent) 

	SocketPeer* getSocketPeer() {
		return m_socketPeer;
	}
protected:
	VCF::String m_host;
	int m_port;
protected:
	bool m_isServer;
	SocketPeer* m_socketPeer;
	SocketState m_state;	
	SocketListeningLoop* m_listeningThread;
};	

};

#endif 

