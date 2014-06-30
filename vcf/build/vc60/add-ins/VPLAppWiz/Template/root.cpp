//$$root$$.cpp


#ifdef WIN32
#include "Win32Peer.h"
#endif

#include "Application.h"

#include $$ROOT_INCLUDE$$

using namespace VCF;

#ifdef WIN32
/**
*this is a DLL main entry point, again this is only here for microsoft implementations
*
*@param HANDLE the module that is loading the DLL
*@param DWORD a code that represents the reason the DLL is being loaded
*@param LPVOID some point to something reserved
*/
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:{
			
		}
		break;

		case DLL_THREAD_ATTACH:{

		}
		break;

		case DLL_THREAD_DETACH:{

		}
		break;

		case DLL_PROCESS_DETACH:{
			
		}
		break;
    }
    return TRUE;
}

#endif //WIN32



extern "C" $$ROOT_$$_API void initPackage()
{
	
}


