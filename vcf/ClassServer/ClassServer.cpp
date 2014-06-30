////ClassServer.cpp

#include "FoundationKit.h"
#include "RemoteObjectKit.h"

#include "Socket.h"
#include "ClassRegistryEntry.h"
#include "AbstractDistributedApplication.h"
#include "ErrorLog.h"
#include "NetToolkit.h"
#include "VCFProcess.h"
#include "ProcessException.h"
#include "Proxy.h"
#include <map>

using namespace VCF;
using namespace VCFNet;
using namespace VCFRemote;


class ServerListener : public Object, public SocketListener  {
public:
	ServerListener() {};

	virtual ~ServerListener() {		
		m_pendingObjectServers.clear();
		m_runningObjectServers.clear();
	};

	virtual void onDataReceived( SocketEvent* event )	{
		BasicInputStream bis( event->getSocketData(), event->getDataSize() );
		int msgType = 0;
		bis >> msgType;
		switch ( msgType ) {
			case CLASS_SVR_DISTRIB_APP_STARTED : {
				//System::print( "CLASS_SVR_DISTRIB_APP_STARTED\n" );
				AppInfo* appInfo = new AppInfo();
				bis >> appInfo;
				std::map<VCF::String,ClassRegistryEntry*>::iterator found = 
					m_pendingObjectServers.find( appInfo->getExecutableName() );
				
				if ( found != m_pendingObjectServers.end() ){
					Socket* sock = event->getSender();						
					ClassRegistryEntry* entry = found->second;
					
					BasicOutputStream bos;
					bos << DISTRIB_APP_MSG_NEW_INST;
					bos << entry->getClassID();
					
					m_runningObjectServers[appInfo->getExecutableName()] = appInfo;
					m_pendingObjectServers.erase( found );
					
					sock->send( bos.getBuffer(), bos.getSize() );
				}
				
			}
			break;

			case CLASS_SVR_DISTRIB_APP_EXITED : {
				//System::print( "CLASS_SVR_DISTRIB_APP_EXITED\n" );
				AppInfo appInfo;
				bis >> &appInfo;
				std::map<VCF::String,AppInfo*>::iterator found = 
					m_runningObjectServers.find( appInfo.getExecutableName() );
				
				if ( found != m_runningObjectServers.end() ){
					AppInfo* info = found->second;
					delete info;
					info = NULL;
					m_runningObjectServers.erase( found );
				}
			}
			break;

			case CLASS_SVR_MSG_PROXY_INVOKE : {				
				//System::print( "CLASS_SVR_MSG_PROXY_INVOKE\n" );
				
				Proxy dummyProxy;
				bis >> &dummyProxy;	
				String methodName;
				bis >> methodName;
				std::map<VCF::String,Socket*>::iterator found = 
					m_proxySockets.find( dummyProxy.getInstanceID()->toString() );
				
				//System::print( "Instance ID: %s\n", dummyProxy.getInstanceID()->toString().c_str() );

				if ( found != m_proxySockets.end() ){
					String pendingName = dummyProxy.getInstanceID()->toString();
					pendingName += "." + methodName;
					m_waitingSockets [pendingName] = event->getSender();

					Socket* sock = found->second;
					//System::print( "Sending invoke data to sock" );
					sock->send( event->getSocketData(), event->getDataSize() );
				}
			}
			break;

			case CLASS_SVR_MSG_NEW_PROXY : {
				String classID;
				bis >> classID;
				Proxy dummyProxy;
				bis >> &dummyProxy;
				
				//System::print( "CLASS_SVR_MSG_NEW_PROXY Instance ID: %s\n", dummyProxy.getInstanceID()->toString().c_str() );

				std::map<VCF::String,Socket*>::iterator found = 
					m_waitingSockets.find( classID );
				if ( found != m_waitingSockets.end() ){
					Socket* sock = found->second;
					//establish a link between the socket and the newly created 
					//proxy
					m_proxySockets[ dummyProxy.getInstanceID()->toString() ] = 
						event->getSender();

					sock->send( event->getSocketData(), event->getDataSize() );

					m_waitingSockets.erase( found );
				}
			}
			break;
			
			case CLASS_SVR_MSG_PROXY_INVOKE_RETURNED :{
				Proxy dummyProxy;
				bis >> &dummyProxy;
				String methodName;
				bis >> methodName;
				String pendingName = dummyProxy.getInstanceID()->toString() + "." + methodName;
				std::map<VCF::String,Socket*>::iterator found = 
					m_waitingSockets.find( pendingName );
				if ( found != m_waitingSockets.end() ) {
					Socket* sock = found->second;
					sock->send( event->getSocketData(), event->getDataSize() );
				}
			}
			break;
			case CLASS_SVR_MSG_NEW_INST : {
				String className;
				bis >> className;
				//printf( "recieved new instance request for class %s\n", className.c_str() );
				/**
				*OK so we have a request to create a new class instance
				*we open a ClassRegistryStream, and search for the requested class
				*if a match is found then a Entry will be returned to us.
				*
				*If the entry indicates the server is in process, then
				*we send a message back to the ClassServerInstance (who is 
				*the one who started this all off) to load the VPL through
				*a call to Application::loadVPL() using the entry->getLocalServerPath()
				*to indicate the lib to load.
				*
				*If the entry is not in process then we have a bit more work to do.
				*If the entry is a local executable then we do the following:
				*We launch the Process specified in the entry->getLocalServerPath()
				*Since the Process must be an app that implements the AbstractDistributedApplication
				*class, once launched it will connect into the ClassServer
				*m_localServerPath, and connect
				*/
				DistributedClassRegistry* dcr = DistributedClassRegistry::getDistributedClassRegistry();
				ClassRegistryEntry* entry = dcr->findEntry( className );
				if ( NULL != entry ) {
					Socket* sender = event->getSender();					
					/*
					BasicOutputStream bos;
					bos << entry;

					sender->send( bos.getBuffer(), bos.getSize() );					
					*/
					if ( (false == entry->isInProcess()) && (false == entry->isRemote()) ) {
						try {
							//m_runningObjectServers.find( entry->getLocalServerPath() );
							Process p;

							m_waitingSockets[entry->getClassID()] = sender;
							m_pendingObjectServers[entry->getLocalServerPath()] = entry;

							if ( false == p.createProcess( entry->getLocalServerPath() ) ) {
								std::map<VCF::String,ClassRegistryEntry*>::iterator found = 
									m_pendingObjectServers.find( entry->getLocalServerPath() );

								if ( found != m_pendingObjectServers.end() ){
									m_pendingObjectServers.erase( found );
								}

								std::map<VCF::String,Socket*>::iterator found2 = 
									m_waitingSockets.find( entry->getClassID() );
								if ( found2 != m_waitingSockets.end() ){
									m_waitingSockets.erase( found2 );
								}
								throw ProcessException();
							}
							else {
								
							}
						}
						catch ( BasicException& e ) {
							System::errorPrint( &e );
							//throws a DistributedException through
							//the use of the AppServer
						} 
					}
				}
			}
			break;
		}
	}

	virtual void onClientConnected( SocketEvent* event ) {

	}

	virtual void onClientDisconnected( SocketEvent* event ) {

	}

	std::map<VCF::String,ClassRegistryEntry*> m_updatedEntries;
	std::map<VCF::String,ClassRegistryEntry*> m_pendingObjectServers;
	std::map<VCF::String,Socket*> m_waitingSockets;
	std::map<VCF::String,Socket*> m_proxySockets;
	std::map<VCF::String,AppInfo*> m_runningObjectServers;
};

class ClassServer  : Object {
public:
	ClassServer() {
		m_servListener = NULL;
	}	

	virtual ~ClassServer() {
		
	}
	
	void main( int argc, char** argv ) {
		ErrorLog errLog;

		//System::print( "Class Server started ...\n" );
		Socket serverSocket( LOCAL_CLASS_SERVER, CLASS_SERVER_PORT, true );
		m_servListener = new ServerListener();
		serverSocket.addSocketListener( m_servListener );
		serverSocket.addEventHandler( "servListener", m_servListener );
		//System::print( "Socket started...\n" );
		
		serverSocket.startListening();
		
		while ( true ) {
			System::sleep( 25 );
		}
	}
	
	ServerListener* m_servListener;

	
};








int main( int argc, char** argv ){

	initFoundationKit();
	ClassServer classSvr;
	
	classSvr.main( argc, argv );	

	return 0;
}










