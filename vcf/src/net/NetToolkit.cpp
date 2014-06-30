// NetToolkit.cpp

#include "FoundationKit.h"
#include "SocketEvent.h"
#include "NetToolkit.h"
#include "Socket.h"

#ifdef WIN32
#include "Win32SocketPeer.h"
#endif

using namespace VCF;
using namespace VCFNet;


NetToolkit NetToolkit::netToolkitInstance;

NetToolkit::NetToolkit()
{

}

NetToolkit::~NetToolkit()
{

}

NetToolkit* NetToolkit::getDefaultNetToolkit()
{
	return &NetToolkit::netToolkitInstance;
}

SocketPeer* NetToolkit::createSocketPeer( Socket* socket, const VCF::String& host, const int& port )
{
#ifdef WIN32
	return new Win32SocketPeer( socket, host, port );
#else
	return NULL;
#endif
}

VCF::String NetToolkit::getLocalMachineName()
{
	VCF::String result = "";
#ifdef WIN32
	result = Win32SocketPeer::getLocalMachineName();
#endif
		
	return result;
}

SocketPeer* NetToolkit::createSocketPeer( Socket* socket, const int& socketPeerID, const VCF::String& host, const int& port )
{
#ifdef WIN32
	return new Win32SocketPeer( socket, socketPeerID, host, port );
#else
	return NULL;
#endif
}

VCF::uint32 NetToolkit::getLocalIPAddress()
{
	return 0;
}