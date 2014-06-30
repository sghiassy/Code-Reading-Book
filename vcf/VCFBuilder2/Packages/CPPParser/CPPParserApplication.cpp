//CPPParserApplication.cpp


#include "ApplicationKit.h"
#include "LibraryApplication.h"
#include "Win32Peer.h"


class CPPParserApplication : public LibraryApplication {
public:

	virtual bool initRunningApplication(){
		bool result = LibraryApplication::initRunningApplication();	
		

		return result;
	}

};


static CPPParserApplication* singleCPPParserApplicationInstance = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch ( ul_reason_for_call ) {
		case DLL_PROCESS_ATTACH:  {			

			singleCPPParserApplicationInstance = new CPPParserApplication();

			singleCPPParserApplicationInstance->getPeer()->setHandleID( (long)hModule );
			
			singleCPPParserApplicationInstance->setName( "CPPParser" );

			LibraryApplication::registerLibrary( singleCPPParserApplicationInstance );

			if ( false == singleCPPParserApplicationInstance->initRunningApplication() ) {
				singleCPPParserApplicationInstance->terminateRunningApplication();
				delete singleCPPParserApplicationInstance;
				singleCPPParserApplicationInstance = NULL;
			}
		}
		break;

		case DLL_THREAD_ATTACH: {

		}
		break;

		case DLL_THREAD_DETACH:  {

		}
		break;

		case DLL_PROCESS_DETACH:  {
			if ( NULL != singleCPPParserApplicationInstance ) {
				singleCPPParserApplicationInstance->terminateRunningApplication();
				delete singleCPPParserApplicationInstance;
				singleCPPParserApplicationInstance = NULL;
			}
		}
		break;
    }
    return TRUE;
}

