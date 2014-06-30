// Win32SocketPeer.cpp

#include <winsock2.h> //makes sure to link with Ws2_32.lib

#include "FoundationKit.h"
#include "Socket.h"
#include "Win32SocketPeer.h"
#include <algorithm>
#include "SocketException.h"

using namespace VCF;
using namespace VCFNet;

bool Win32SocketPeer::WinSockStarted = false;

struct MemStruct{
	int size;
	char* data;
};

Win32SocketPeer::Win32SocketPeer( Socket* socket, const String& host, const int& port )	{
	m_socket = socket;
	m_host = host;
	m_port = port;
	m_sock = 0;
	memset( &m_sin, 0, sizeof(m_sin) );
	if ( false == Win32SocketPeer::WinSockStarted ) {
		Win32SocketPeer::startup();		
	}
}

Win32SocketPeer::Win32SocketPeer( Socket* socket, const int& socketPeerID, const VCF::String& host, const int& port )
{
	m_socket = socket;
	m_host = host;
	m_port = port;
	m_sock = socketPeerID;
	memset( &m_sin, 0, sizeof(m_sin) );
	if ( false == Win32SocketPeer::WinSockStarted ) {
		Win32SocketPeer::startup();		
	}
}

Win32SocketPeer::~Win32SocketPeer(){
	
	std::vector<Socket*>::iterator it = m_connectedClients.begin();
	while ( it != m_connectedClients.end() ) {
		Socket* so = *it;
		printf( "deleting %x\n", so );
		delete so;
		it++;
	}

	if ( 0 != m_sock ) {
		closesocket(m_sock);
		m_sock = 0;
	}
	m_connectedClients.clear();	
	m_disconnectedClients.clear();
	m_socket = NULL;
}

SocketState Win32SocketPeer::stopListening(){
	SocketState result = SOCKET_CONNECT_ERROR;

	return result;
}

SocketState Win32SocketPeer::startListening(){	
	
	if ( SOCKET_CLOSED != this->m_socket->getState() ){ 
		return SOCKET_ALREADY_LISTENING_ERROR;
	}
	//printf("starting to listen for incoming sockets...\n");
	int length = 0;
	int msgsock = 0;

	/**
	*we want to create a socket in two different 
	*ways depending on how the Socket::m_isServer is set
	*if true then bind and listen 
	*otherwise connect ?
	*/

    //Create a socket if neccessary
	if ( m_sock <= 0  ) { 
		m_sock = ::socket(AF_INET,SOCK_STREAM,0);
		if (m_sock < 0) {
			m_sock = 0;
			return SOCKET_CANT_OPEN_SOCKET;
		}	
	}

	if ( true == this->m_socket->isServer() )  {		
		// Initialize the socket's address structure 
		m_sin.sin_family = AF_INET;
		m_sin.sin_addr.s_addr = INADDR_ANY;
		m_sin.sin_port = htons((short)m_port);
		
		// Assign an address to this socket
		int bindErr =  bind( m_sock, (sockaddr*)&m_sin, sizeof(m_sin) );
		if ( bindErr < 0)  {
			closesocket( m_sock );
			m_sock = 0;
			return SOCKET_BIND_FAILED;
		}    
		
		//Prepare the socket queue for connection requests 
		int listenErr = listen( m_sock, 5 );
		
	}
	else {
		SocketState state = connectTo( this->m_host, this->m_port );
		m_socket->setState( state );
	}
    
	return SOCKET_LISTENING;
}

int Win32SocketPeer::send( const char* bytes, const int& size ){
	int result = 0;
	if ( SOCKET_CLOSED == this->m_socket->getState() ){
		//connect
		SocketState state = connectTo( this->m_host, this->m_port );
		m_socket->setState( state );
	}
	result = ::send( m_sock, bytes, size, 0 );
	return result;
}

void Win32SocketPeer::startup()	{
	if ( true == Win32SocketPeer::WinSockStarted ) {
		return;
	}

	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData; 
	int err = WSAStartup(wVersionRequested, &wsaData); 
	if (err != 0) {		
		//throw exception
		WSACleanup();
		//throw SocketException( "Winsock dll load failed." );
	}
	
	if ( LOBYTE( wsaData.wVersion ) != 1 || 
		HIBYTE( wsaData.wVersion ) != 1 ) 	{ 
		WSACleanup(); 
		//throw SocketException( "Winsock version 1.1 not found." );
	}	

	Win32SocketPeer::WinSockStarted = true;
}
	
SocketState Win32SocketPeer::connectTo( const String& hostName, const int& port ){
	SocketState result = SOCKET_CONNECT_ERROR;

	m_host = hostName;
	const char* tmpHostName = hostName.c_str();
    struct hostent *hp;
	if ( 0 != m_sock ){ //attempt to close the previous connection
		closesocket( m_sock );
		m_sock = 0;
	}
    
    m_port = port;
    
	m_sock = socket (AF_INET, SOCK_STREAM, 0);
    if ( m_sock < 0 ) {		
		return SOCKET_CANT_OPEN_SOCKET;
    }

    // Initialize the socket address to the server's address. 
	memset( &m_sin, 0, sizeof(m_sin) );
    m_sin.sin_family = AF_INET;
	
    hp = gethostbyname( tmpHostName );    //to get host address 
    
	if (hp == NULL) {
		return SOCKET_CANT_OPEN_SOCKET;
    }
    
	memcpy( &(m_sin.sin_addr.s_addr), hp->h_addr, hp->h_length );
    m_sin.sin_port = htons((short)m_port);
	
    // Connect to the server.
	int connectErr = connect(m_sock, (sockaddr*)&m_sin,sizeof(m_sin));
    if ( connectErr < 0) {	
		int i = WSAGetLastError();
		closesocket(m_sock);
		return SOCKET_CONNECT_ERROR;
    }   

	return SOCKET_CONNECTED;
}

void Win32SocketPeer::setPort( const int& port )
{
	this->m_port = port;
	this->m_socket->setState( SOCKET_CLOSED );
	if ( 0 != m_sock ){ //attempt to close the previous connection
		closesocket( m_sock );
		m_sock = 0;
	}
}

int Win32SocketPeer::getPort()
{
	return m_port;
}

void Win32SocketPeer::setHost( const String& host )
{
	m_host = host;
	this->m_socket->setState( SOCKET_CLOSED );
	if ( 0 != m_sock ){ //attempt to close the previous connection
		closesocket( m_sock );
		m_sock = 0;
	}
}

String Win32SocketPeer::getHost()
{
	return m_host;
}

bool Win32SocketPeer::hasWaitingClients()
{
	bool result = false;
	
	struct timeval timeout ;
	fd_set fd ;
	
	FD_ZERO(&fd);
	
	FD_SET ( m_sock, &fd) ;
	timeout.tv_sec = 0 ;
	timeout.tv_usec = 0 ;
	
	int selectResult = SOCKET_ERROR;
	if ( this->m_socket->isServer() ) {
		selectResult = select( m_sock + 1, &fd, NULL, NULL, &timeout );
	}
	else {		
		selectResult = select( m_sock + 1, NULL, &fd, NULL, &timeout );
	}
	if ( SOCKET_ERROR == selectResult ){
		//throw exception
	}
	
	result = ( selectResult > 0 );
	
	if ( result )  {
		int i = 8;
		i++;
	}


	return result;
}

Socket* Win32SocketPeer::getClient()
{
	Socket* result = NULL;
	if ( true == this->m_socket->isServer() ) {
		int length = 0;
		int msgsock = 0;
		//Initialize the socket's address structure 
		
		m_sin.sin_family = AF_INET;
		m_sin.sin_addr.s_addr = INADDR_ANY;
		m_sin.sin_port = htons((short)m_port);
		
		//StringUtils::trace("Listening for connect requests on port " + StringUtils::toString(m_port) + "\n" );
		//printf( "Listening for connect requests on port %d\n", m_port );
		//Prepare the socket queue for connection requests 
		
		length = sizeof(m_sin);
		msgsock = accept( m_sock, (sockaddr*)&m_sin, &length );
		
		if (msgsock != SOCKET_ERROR) {
			result = new Socket( msgsock, m_host, m_port );
			this->m_connectedClients.push_back( result );			
		}
		else {
			//StringUtils::trace("Accept() failed on m_sock = " + StringUtils::toString((int)m_sock) + "\n" );
			//printf("Accept() failed on m_sock = %d\n", m_sock ); 		
		}
		
		//StringUtils::trace("Connection from host " + m_host + " port: " + StringUtils::toString(m_port) + "\n" );
		//printf("Connection from host %s port: %d\n", m_host.c_str(), m_port );	
	}
	
	else {		
		if ( true == m_connectedClients.empty() ) {
			result = new Socket(m_sock, m_host, m_port );
			this->m_connectedClients.push_back( result );
		}
	}
	return result;
}

void Win32SocketPeer::readDataFromClient( Socket* client, fd_set* readfd )
{
	if ( NULL == client ) {
		return;
	}

	int clientSock = client->getSocketPeer()->getSocketPeerID();
	if (FD_ISSET (clientSock, readfd)) {
		//StringUtils::trace( StringUtils::toString(*it) +  " sent data...\n" );
		//printf( "%d sent data...\n", client->m_socketID );
		// this guy sent something		
		
		char buf[VCFNET_MAX_SOCKET_SIZE];
		char* totalDataBuf = NULL;
		int totalSize = VCFNET_MAX_SOCKET_SIZE;
		std::vector<MemStruct*> dataList;
		memset( buf, 0 , VCFNET_MAX_SOCKET_SIZE );
		
		//read first chunk of data
		int recvResult = recv( clientSock, buf, VCFNET_MAX_SOCKET_SIZE, 0 );
		
		switch ( recvResult ) {
			case 0 : {
				//printf( "client closed connection - fire event\n" );
				//client closed connection - fire event
				SocketEvent event( m_socket, client );
				this->m_socket->fireOnClientDisconnected( &event );	
				m_disconnectedClients.push_back( client );				
			}
			break;
			
			case SOCKET_ERROR : {
				//socket error throw exception ????	
				if ( false == this->m_socket->isServer() ) {
					//possible problem - perhaps the server 
					int err = WSAGetLastError();
					switch ( err ) {
						case WSAECONNRESET : {
							SocketEvent event( m_socket, client );
							this->m_socket->fireOnClientDisconnected( &event );	
							m_disconnectedClients.push_back( client );							
						}
						break;
					}
				}
			}
			break;

			default : {
				//check to see if more data is still out there
				u_long res = 0;
				ioctlsocket( clientSock, FIONREAD, &res );
				char* tmp = NULL;
				if ( res > 0 ){
					while ( res > 0 ){ //continue to pull data
						MemStruct* memStruct = new MemStruct;
						memStruct->data = new char[res];
						memset( memStruct->data, 0, res );
						memStruct->size = res;
						recvResult = recv( clientSock, memStruct->data, res, 0 );
						if ( recvResult != SOCKET_ERROR ){														
							dataList.push_back( memStruct );
							totalSize += res;
							ioctlsocket( clientSock, FIONREAD, &res );
						}
						else {
							res = 0;
						}
					}
				}
				else {
					MemStruct* memStruct = new MemStruct;
					memStruct->data = new char[recvResult];
					memset( memStruct->data, 0, recvResult );
					memStruct->size = recvResult;
					memcpy(memStruct->data, buf, recvResult );
					dataList.push_back( memStruct );
					totalSize = recvResult;
				}
				if ( ! dataList.empty() ){
					totalDataBuf = new char[totalSize];
					memset( totalDataBuf, 0, totalSize );
					char* tmpBuf = totalDataBuf;
					int insertData = 0;
					std::vector<MemStruct*>::iterator dataIt = dataList.begin();
					while ( dataIt != dataList.end() ){
						MemStruct* memStruct = (*dataIt);
						tmp = memStruct->data;
						memcpy( tmpBuf, tmp, memStruct->size );
						tmpBuf += memStruct->size;
						delete [] memStruct->data;
						memStruct->data = NULL;
						delete memStruct;
						memStruct = NULL;
						dataIt++;
					}
				}
				else {
					totalDataBuf = buf;					
				}
				
				//finished reading - fire event
				SocketEvent event( m_socket, client, totalDataBuf, totalSize );
				this->m_socket->fireOnDataReceived( &event );
			}
			break;
		}
	}
}

void Win32SocketPeer::checkForPendingData( const int& waitAndBlock )
{
	//printf(  "Starting checkForPendingData()...\n" );
	
	if ( !m_connectedClients.empty() ){
		//StringUtils::trace( "checkForPendingData() has clients - looping...\n" );
		//printf( "checkForPendingData() has clients - looping...\n" );
		std::vector<Socket*>::iterator it = m_connectedClients.begin();

		fd_set readfd;			
		FD_ZERO(&readfd);
		
		struct timeval timeout;		
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		
		int maxSock = 0;
		while ( it != m_connectedClients.end() ){
			Socket* so = *it;
			int sockID = so->getSocketPeer()->getSocketPeerID();
			FD_SET ( sockID, &readfd) ;		
			if ( sockID > maxSock ){
				maxSock = sockID;	
			}
			it++;
		}
		//printf( "maxSock = %d\n", maxSock );
		struct timeval* timeoutVal = NULL;
		if ( (VCFNET_ASYNCHRONOUSWAIT == waitAndBlock) ) {
			timeoutVal = &timeout;
		}
		else if ( waitAndBlock > 0 ) {
			timeout.tv_sec = waitAndBlock / 1000; //waitAndBlock is in milliseconds
			timeoutVal = &timeout;
		}
		int i = select( maxSock + 1, &readfd, NULL, NULL, timeoutVal);
		
		//printf( "select( %d, ... ), returned %d\n", maxSock + 1, i );

		if ( (SOCKET_ERROR != i) && ( i > 0 ) ){ //no errors, and 1 or more sockets have data
			it = m_connectedClients.begin();
			
			while ( it != m_connectedClients.end() ){
				readDataFromClient( *it, &readfd );				
				//printf( "readDataFromClient( so->m_socketID=%d)\n", (*it)->m_socketID ); 
				it++;
			}

			it = this->m_disconnectedClients.begin();
			while ( it != m_disconnectedClients.end() ) {
				Socket* so = *it;
				std::vector<Socket*>::iterator found = std::find( m_connectedClients.begin(), m_connectedClients.end(), so );
				
				if ( found != m_connectedClients.end() ) {
					m_connectedClients.erase( found );
					delete so;
					so = NULL;
				}
				it++;
			}
			m_disconnectedClients.clear();
		}
		else {
			//printf( "select() failed - returned %d\n\terror was : %s", i, getWSAErrorString( WSAGetLastError() ) );
		}
	}
	//printf(  "finished checkForPendingData()...\n" );
}

VCF::String Win32SocketPeer::getLocalMachineName()
{
	Win32SocketPeer::startup();
	VCF::String result = "";
	struct hostent *hp;
	hp = gethostbyname( "localhost" );
	if ( NULL != hp ) {
		result = hp->h_name;
	}	

	return result;
}

int Win32SocketPeer::getSocketPeerID() 
{
	return this->m_sock;
}