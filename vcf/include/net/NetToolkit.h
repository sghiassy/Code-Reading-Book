// NetToolkit.h

#ifndef _NETTOOLKIT_H__
#define _NETTOOLKIT_H__



namespace VCFNet
{

class Socket;

class SocketPeer;

class NETKIT_API NetToolkit : public VCF::Object  
{
public:
	NetToolkit();
	virtual ~NetToolkit();
	
	static NetToolkit* getDefaultNetToolkit();

	VCF::String getLocalMachineName();

	VCF::uint32 getLocalIPAddress();

	virtual SocketPeer* createSocketPeer( Socket* socket, const VCF::String& host, const int& port );

	virtual SocketPeer* createSocketPeer( Socket* socket, const int& socketPeerID, const VCF::String& host, const int& port );
protected:
	static NetToolkit netToolkitInstance;
};


};

#endif
