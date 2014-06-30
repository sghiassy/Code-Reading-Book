// Socket.cpp:

#include "FoundationKit.h"
#include "Socket.h"
#include "SocketPeer.h"
#include "NetToolkit.h"
#include "SocketListeningLoop.h"

using namespace VCFNet;
using namespace VCF;

Socket::Socket( const int& port, const bool& startAsServer ) 
{
	m_host = "localhost";
	m_port = port;
	m_isServer = startAsServer;
	init();
}

Socket::Socket( const String& host, const int& port, const bool& startAsServer )	
{
	m_host = host;
	m_port = port;
	m_isServer = startAsServer;
	init();
}

Socket::Socket( const int& socketPeerID, const VCF::String& host, const int& port )
{
	m_host = host;
	m_port = port;
	m_isServer = false;
	m_socketPeer = NULL;
	m_listeningThread = NULL;
	NetToolkit* netKit = NetToolkit::getDefaultNetToolkit();
	m_socketPeer = netKit->createSocketPeer( this, socketPeerID, m_host, m_port ); 
}

Socket::~Socket()
{
	if ( SOCKET_LISTENING == m_state ) {
		stopListening();
	}
	delete m_socketPeer;
	m_socketPeer = NULL;
}

void Socket::init(){	
	m_state = SOCKET_CLOSED;
	m_socketPeer = NULL;
	m_listeningThread = NULL;
	NetToolkit* netKit = NetToolkit::getDefaultNetToolkit();
	m_socketPeer = netKit->createSocketPeer( this, m_host, m_port ); 
}

void Socket::setPort( const int& port )
{
	m_port = port;
	m_socketPeer->setPort( m_port );
}

void Socket::setHost( const String& host )
{
	m_host = host;
	m_socketPeer->setHost( host );
}

int Socket::send( const char* bytes, const int& size )
{
	return m_socketPeer->send( bytes, size );
}

SocketState Socket::startListening()
{
	m_state = m_socketPeer->startListening();
	if ( SOCKET_LISTENING == m_state ) {
		//start thread loop
		m_listeningThread = new SocketListeningLoop(this);
		m_listeningThread->start();
	}
	return m_state;
}

SocketState Socket::stopListening()
{
	if ( NULL != m_listeningThread ) {
		delete m_listeningThread;
	}
	m_listeningThread = NULL;
	m_state = m_socketPeer->stopListening();
	return m_state;
}

String Socket::toString()
{
	return "Sockect State is \"" + SocketStateNames[(int)m_state] + "\"";
}

bool Socket::hasWaitingClients() 
{
	return m_socketPeer->hasWaitingClients();
}

Socket* Socket::getClient() 
{
	Socket* result = m_socketPeer->getClient();		
	return result;
}

void Socket::checkForPendingData( const int& waitAndBlock ) 
{
	m_socketPeer->checkForPendingData( waitAndBlock );
}