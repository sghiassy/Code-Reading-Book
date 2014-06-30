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

#include "ClassServerInstance.h"
#include "Proxy.h"
#include "ClassRegistry.h"
#include "Object.h"
#include "Socket.h"
#include "ClassRegistryEntry.h"
#include "System.h"

using namespace VCF;
using namespace VCFNet;
using namespace VCFRemote;



ClassServerInstance::ClassServerInstance()
{
	m_newInstanceProxy = NULL;

	EventHandler* sh = 
		new SocketEventHandler<ClassServerInstance>( this, ClassServerInstance::onDataReceived, "SocketHandler" );
	
}

ClassServerInstance::~ClassServerInstance()
{

}

Proxy* ClassServerInstance::createInstance( const VCF::String& className )
{
	Proxy* result = NULL;
	m_newInstanceProxy = NULL;

	m_classSvrReturned = false;
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();

	if ( NULL == classRegistry ) {
		//throw exception
	}
	
	Object* newObjInstance = NULL;
	classRegistry->createNewInstance( className, &newObjInstance );

	if ( NULL != newObjInstance ) {
		result = new Proxy( newObjInstance );
	}
	else { //we have to look elsewhere, first on this machine, then on the network
		Socket sock( LOCAL_CLASS_SERVER, CLASS_SERVER_PORT );
		EventHandler* socketHandler = getEventHandler( "SocketHandler" );
		sock.addDataReceivedHandler( socketHandler );
		try {
			sock.startListening();
			System::sleep( 20 );//HACK ALERT !!!! this seems to need to be here
			              //to let the threaded listener for the socket to 
						  //get started perhaps there is a better way ?????
			BasicOutputStream stream;
			
			int msgType = CLASS_SVR_MSG_NEW_INST;
			stream << msgType << className;
			sock.send( stream.getBuffer(), stream.getSize() );
			sock.checkForPendingData( VCFNET_INFINITEWAIT );
			result = m_newInstanceProxy;
			if ( result == NULL ) {
				//throw exception!!!
			}
		}
		catch( ... ) {
			
		}
	}

	return result;
}

Proxy* ClassServerInstance::createInstanceByClassID( const VCF::String& classID )
{
	Proxy* result = NULL;
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();

	if ( NULL == classRegistry ) {
		//throw exception
	}
	
	Object* newObjInstance = NULL;
	classRegistry->createNewInstanceFromClassID( classID, &newObjInstance );

	if ( NULL != newObjInstance ) {
		result = new Proxy( newObjInstance );
	}
	else { //we have to look elsewhere, first on this machine, then on the network
		
	}

	return result;
}

void ClassServerInstance::onDataReceived( VCFNet::SocketEvent* event )
{	
	this->m_classSvrReturned = true;
	m_data = event->getSocketData();
	printf( "ClassServerInstance::onDataReceived() Data recieved, size is %d bytes\n", event->getDataSize() );
	
	BasicInputStream bis(event->getSocketData(), event->getDataSize() );
	
	m_newInstanceProxy = NULL;
	int msgType = 0;
	bis >> msgType;
	switch ( msgType ) {
		case CLASS_SVR_MSG_NEW_PROXY : {
			String classID;
			bis >> classID;
			m_newInstanceProxy = new Proxy();
			bis >> m_newInstanceProxy;
		}
		break;
	}
	
}

VCF::VariantData* ClassServerInstance::invoke( VCF::OutputStream* marshalledData ) 
{
	VCF::VariantData* result = NULL;
	
	Socket sock( LOCAL_CLASS_SERVER, CLASS_SERVER_PORT );
	EventHandler* socketHandler = getEventHandler( "SocketHandler" );
	sock.addDataReceivedHandler( socketHandler );

	try {
		sock.startListening();
		System::sleep( 20 );//HACK ALERT !!!! this seems to need to be here
		//to let the threaded listener for the socket to 
						  //get started perhaps there is a better way ?????
		BasicOutputStream stream;

		System::print( "Sending invoke request ot server...\n" );

		int msgType = CLASS_SVR_MSG_PROXY_INVOKE;
		stream << msgType;
		stream.write( marshalledData->getBuffer(), marshalledData->getSize() );
		sock.send( stream.getBuffer(), stream.getSize() );
		sock.checkForPendingData( VCFNET_INFINITEWAIT );
		
	}
	catch( ... ) {
		
	}
	
	return result;
}