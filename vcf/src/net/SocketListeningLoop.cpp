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
#include "FoundationKit.h"
#include "Socket.h"
#include "SocketListeningLoop.h"


using namespace VCF;
using namespace VCFNet;

SocketListeningLoop::SocketListeningLoop( Socket* socket )
{
	m_socket = socket;

	m_socketDataReceivedHandler = 
		new SocketEventHandler<SocketListeningLoop>( this, SocketListeningLoop::onDataReceived );

	m_socketClientConnectedHandler = 
		new SocketEventHandler<SocketListeningLoop>( this, SocketListeningLoop::onClientConnected );

	m_socketClientDisconnectedHandler = 
		new SocketEventHandler<SocketListeningLoop>( this, SocketListeningLoop::onClientDisconnected );

	
	if ( NULL != m_socket ) {
		m_socket->addDataReceivedHandler( m_socketDataReceivedHandler );
		m_socket->addClientConnectedHandler( m_socketClientConnectedHandler );
		m_socket->addClientDisconnectedHandler( m_socketClientDisconnectedHandler );
	}
	m_socketIsServer = m_socket->isServer();
	m_serverClientConnected = false;
}

SocketListeningLoop::~SocketListeningLoop()
{
	this->m_canContinue = false;
	if ( NULL != m_socket ) {
		m_socket->removeDataReceivedHandler( m_socketDataReceivedHandler );
		m_socket->removeClientConnectedHandler( m_socketClientConnectedHandler );
		m_socket->removeClientDisconnectedHandler( m_socketClientDisconnectedHandler );
	}

	delete m_socketDataReceivedHandler;
	m_socketDataReceivedHandler = NULL;

	delete m_socketClientConnectedHandler;
	m_socketClientConnectedHandler = NULL;

	delete m_socketClientDisconnectedHandler;
	m_socketClientDisconnectedHandler = NULL;	
}

bool SocketListeningLoop::run()
{
	bool result = (NULL != this->m_socket);
	if ( false == result ) {
		return false;
	}
	//printf( "running SocketListeningLoop::run for socket@%x\n",m_socket );
	//printf( "\twaiting a sec...\n" );
	
	if ( true == m_socketIsServer ) {
		//continue to listen for incoming data packets from already connected clients
		
		while ( (true == this->canContinue()) && (false == m_socket->hasWaitingClients()) ) {
			//printf( "\tno clients yet...\n" );
			m_socket->checkForPendingData();
			sleep( 10 );		
		}
		
		if ( true == this->canContinue() ) {
			//somebody connected ! since we broke to the while loop - go get the client SocketObject
			Socket* clientSocket = m_socket->getClient();
			if ( NULL != clientSocket ) {		
				SocketEvent event( m_socket, clientSocket );
				m_socket->fireOnClientConnected( &event );
			}
		}
		
		m_socket->checkForPendingData();
	}
	else {
		//check for a connection		
		bool connection = m_socket->hasWaitingClients();
		if ( (true == connection) && ( false == m_serverClientConnected) ) {
			//first time connection
			m_serverClientConnected = true;
			Socket* clientSocket = m_socket->getClient();
			if ( NULL != clientSocket ) {		
				SocketEvent event( m_socket, clientSocket );
				m_socket->fireOnClientConnected( &event );
			}
		}
		
		if ( true == m_serverClientConnected ) {
			m_socket->checkForPendingData();
		}
	}
	sleep( 10 );
	return result;
}

void SocketListeningLoop::onDataReceived( SocketEvent* event )
{	
	//printf( "SocketListeningLoop::onDataReceived, event =%p\n", event );
}

void SocketListeningLoop::onClientConnected( SocketEvent* event )
{
	//printf( "SocketListeningLoop::onClientConnected, event =%p\n", event );
}

void SocketListeningLoop::onClientDisconnected( SocketEvent* event )
{
	//printf( "SocketListeningLoop::onClientDisconnected, event =%p\n", event );
}