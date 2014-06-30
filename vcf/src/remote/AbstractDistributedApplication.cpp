//AbstractDistributedApplication.h
/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#include "RemoteObjectKit.h"
#include "AbstractDistributedApplication.h"
#include "VCFProcess.h"
#include "Proxy.h"
#include "VariantDataStream.h"

using namespace VCF;
using namespace VCFNet;
using namespace VCFRemote;

String defaultUsageString = "To register or unregister this application's classes, please use the -register or -unregister arguments";


DistributedApplication* DistributedApplication::runningDistributedApp = NULL;

void DistributedApplication::distributedAppMain( const int& argc, VCF::VCFChar** argv )
{
	DistributedApplication* app = DistributedApplication::getRunningDistributedApp();
	
	if ( NULL != app ) {
		std::vector<String> commandLine;
		for (int i=0;i<argc;i++) {
			commandLine.push_back( String(argv[i]) );
		}
		
		app->processCommandLine( commandLine );

		if ( true == app->initApplication() ){
			app->notifyDistributedClassServer( true );

			app->runDistributedApp(); //execute main app code

			app->notifyDistributedClassServer( false );
		}
		else { //something went wrong - try and clean up
			app->terminateAppplication();
		}
	}
	else {
		//throw exception
	}
}

DistributedApplication* DistributedApplication::getRunningDistributedApp()
{
	return DistributedApplication::runningDistributedApp;
}

AbstractDistributedApplication::AbstractDistributedApplication()
{
	DistributedApplication::runningDistributedApp = this;

	m_unMarshalledArgTypes["i"] = PROP_INT;
	m_unMarshalledArgTypes["l"] = PROP_LONG;
	m_unMarshalledArgTypes["+l"] = PROP_ULONG;
	m_unMarshalledArgTypes["h"] = PROP_SHORT;
	m_unMarshalledArgTypes["c"] = PROP_CHAR;
	m_unMarshalledArgTypes["d"] = PROP_DOUBLE;
	m_unMarshalledArgTypes["f"] = PROP_FLOAT;
	m_unMarshalledArgTypes["b"] = PROP_BOOL;
	m_unMarshalledArgTypes["s"] = PROP_STRING;
	m_unMarshalledArgTypes["o"] = PROP_OBJECT;

	m_instObjectContainer.initContainer( m_instObjectList );

	m_appInfo = new AppInfo( this );
	Process p;
	m_appInfo->setExecutableName( p.getName() );
	
	m_sock.setPort( CLASS_SERVER_PORT );
	m_sock.setHost( LOCAL_CLASS_SERVER );
	m_sock.setAsServer( false );
	
	m_listener = new DistributedAppListener(this);

	m_onDataReceivedHandler = 
		new SocketEventHandler<DistributedAppListener>( m_listener, DistributedAppListener::onDataReceived );

	m_onClientConnectedHandler = 
		new SocketEventHandler<DistributedAppListener>( m_listener, DistributedAppListener::onClientConnected );

	m_onClientDisconnectedHandler = 
		new SocketEventHandler<DistributedAppListener>( m_listener, DistributedAppListener::onClientDisconnected );

	
	m_sock.addDataReceivedHandler( m_onDataReceivedHandler );
	m_sock.addClientConnectedHandler( m_onClientConnectedHandler );
	m_sock.addClientDisconnectedHandler( m_onClientDisconnectedHandler );

	m_sock.startListening();
}

AbstractDistributedApplication::~AbstractDistributedApplication()
{
	m_sock.stopListening();
	
	m_sock.removeDataReceivedHandler( m_onDataReceivedHandler );
	m_sock.removeClientConnectedHandler( m_onClientConnectedHandler );
	m_sock.removeClientDisconnectedHandler( m_onClientDisconnectedHandler );

	delete m_listener;

	DistributedApplication::runningDistributedApp = NULL;
	delete m_appInfo;
	m_appInfo = NULL;
}

bool AbstractDistributedApplication::initApplication()
{
	bool result = true;

	return result;
}

void AbstractDistributedApplication::processCommandLine( std::vector<VCF::String>& commandLine )
{
	std::vector<VCF::String>::iterator it = commandLine.begin();
	while ( it != commandLine.end() ) {
		processCommandLineArgument( *it );
		it ++;
	}
}

void AbstractDistributedApplication::processCommandLineArgument( const VCF::String arg )
{		
	if ( arg == REGISTER_ARGUMENT ) {
		registerClasses();
	}
	else if ( arg == UNREGISTER_ARGUMENT ) {
		unRegisterClasses();
	}
}

void AbstractDistributedApplication::terminateAppplication()
{

}

bool AbstractDistributedApplication::registerClasses()
{
	bool result = false;

	return result;
}

bool AbstractDistributedApplication::unRegisterClasses()
{
	bool result = false;

	return result;
}

String AbstractDistributedApplication::getUsageString()
{
	String result = defaultUsageString;

	return result;
}

void AbstractDistributedApplication::onDataReceived( VCFNet::SocketEvent* event )
{
	BasicInputStream bis( event->getSocketData(), event->getDataSize() );
	int msgType = 0;
	bis >> msgType;
	
	//System::print( "AbstractDistributedApplication::onDataReceived\n"); 
	switch ( msgType ) {
		case DISTRIB_APP_MSG_NEW_INST : {
			String classID;
			bis >> classID;

			Object* obj = NULL;
			ClassRegistry* classReg = ClassRegistry::getClassRegistry();
			classReg->createNewInstanceFromClassID( classID, &obj );
			if ( NULL != obj ) {
				Socket* sock = event->getSender();			

				Proxy* proxy = new Proxy();
				proxy->getInstanceID()->setMemAddress( (int)obj );
				m_instObjectList[(int)obj] = obj;

				BasicOutputStream bos;
				int msg = CLASS_SVR_MSG_NEW_PROXY;
				bos << msg;
				bos << classID;
				bos << proxy;

				sock->send( bos.getBuffer(), bos.getSize() );
			}
		}
		break;

		case CLASS_SVR_MSG_PROXY_INVOKE : {
			//System::print( "Client App recieved invoke\n" );
			Proxy dummyProxy;
			bis >> &dummyProxy;
			String methodName;
			bis >> methodName;
			int argCount = 0;
			bis >> argCount;
			std::vector<VariantData*> argList;
			for (int i=0;i<argCount;i++){
				String argID;
				bis >> argID;
				std::map<VCF::String,int>::iterator found = 
					m_unMarshalledArgTypes.find(argID);
				if ( found != m_unMarshalledArgTypes.end() ) {
					VariantData* arg = new VariantData();
					switch ( found->second ) {
						case PROP_INT : {
							int tmp;
							bis >> tmp;
							*arg = tmp;
						}
						break;
					}					
					argList.push_back( arg );
				}
			}
			
			VariantData** methodArgs = (VariantData**)argList.begin();
			std::map<int,VCF::Object*>::iterator found = 
				m_instObjectList.find( dummyProxy.getInstanceID()->getMemAddress() );
			if ( found != m_instObjectList.end() ) {
				Object* obj = found->second;
				Class* clazz = obj->getClass();
				Method* method = clazz->getMethod( methodName );
				if ( NULL != method ) {
					//System::print( "Client App found requested method, preparing to invoke...\n" );
					VariantData* result = method->invoke( methodArgs );

					Socket* sock = event->getSender();
					BasicOutputStream bos;
					VariantDataOutputStream vdos( &bos );
					int msg = CLASS_SVR_MSG_PROXY_INVOKE_RETURNED;
					vdos << msg;
					vdos << &dummyProxy;
					vdos << methodName;
					if ( NULL != result ) {
						vdos.writeVariantData( result );
					}
					else {
						vdos << String("NULL");
					}
					sock->send( vdos.getBuffer(), vdos.getSize() );					
				}
			}			
		}
		break;
	}
}

void AbstractDistributedApplication::onClientConnected( VCFNet::SocketEvent* event )
{

}

void AbstractDistributedApplication::onClientDisconnected( VCFNet::SocketEvent* event )
{

}

void AbstractDistributedApplication::runDistributedApp()
{
	while ( true ) {
		System::sleep( 50 );
	}
}

bool AbstractDistributedApplication::notifyDistributedClassServer( const bool& startup )
{	
	bool result = false;

	if ( true == startup ) {
		BasicOutputStream bos;
		int msg = CLASS_SVR_DISTRIB_APP_STARTED;
		bos << msg;
		bos << m_appInfo;
		m_sock.send( bos.getBuffer(), bos.getSize() );
	}
	else {
		BasicOutputStream bos;
		int msg = CLASS_SVR_DISTRIB_APP_EXITED;
		bos << msg;
		bos << m_appInfo;
		m_sock.send( bos.getBuffer(), bos.getSize() );
	}
	return result;
}


Enumerator<Object*>* AbstractDistributedApplication::getInstantiatedObjects()
{
	return m_instObjectContainer.getEnumerator();
}

/**
*Appinfo class implmentation
*/
AppInfo::AppInfo( DistributedApplication* distributedApp )
{
	m_distributedApp = distributedApp;
}

AppInfo::~AppInfo() 
{
	
}

void AppInfo::saveToStream( OutputStream * stream )
{
	stream->write( m_appName );
}

void AppInfo::loadFromStream( InputStream * stream )
{
	stream->read( m_appName );
}


