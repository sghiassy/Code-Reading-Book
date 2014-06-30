//SocketPeer.h

#ifndef _SOCKET_PEER_H__
#define _SOCKET_PEER_H__


namespace VCFNet
{

class Socket;

class NETKIT_API SocketPeer {
public:
	virtual ~SocketPeer(){};	

	virtual SocketState startListening() = 0;

	virtual int send( const char* bytes, const int& size ) = 0;	

	virtual SocketState stopListening() = 0;

	virtual void setPort( const int& port ) = 0;

	virtual int getPort() = 0;
	
	virtual void setHost( const VCF::String& host ) = 0;

	virtual VCF::String getHost() = 0;

	virtual bool hasWaitingClients() = 0;

	virtual Socket* getClient() = 0;
	
	virtual void checkForPendingData( const int& waitAndBlock ) = 0;

	virtual int getSocketPeerID() = 0;
};

};



#endif //_SOCKET_PEER_H__