//TextEditor.cpp


#include "ApplicationKit.h"
#include "TextEditor.h"
#include "Win32Peer.h"


static TextEditorApplication* singleTextEditorApplicationInstance = NULL;

TextEditorApplication::TextEditorApplication()
{
	m_scintillaLib = NULL;
}

bool TextEditorApplication::initRunningApplication()
{
	bool result = LibraryApplication::initRunningApplication();	
	
	String dir = StringUtils::getFileDir( getFileName() );

	m_scintillaLib = new Library( dir + "\\SciLexer.DLL" );

	return result;
}

void TextEditorApplication::terminateRunningApplication()
{
	delete m_scintillaLib;
}

TextEditorApplication* TextEditorApplication::getTextEditorApplication()
{
	return singleTextEditorApplicationInstance;
}



BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch ( ul_reason_for_call ) {
		case DLL_PROCESS_ATTACH:  {

			singleTextEditorApplicationInstance = new TextEditorApplication();

			singleTextEditorApplicationInstance->getPeer()->setHandleID( (long)hModule );
			
			singleTextEditorApplicationInstance->setName( "TextEditor" );

			LibraryApplication::registerLibrary( singleTextEditorApplicationInstance );
			
			if ( false == singleTextEditorApplicationInstance->initRunningApplication() ) {
				singleTextEditorApplicationInstance->terminateRunningApplication();

				delete singleTextEditorApplicationInstance;

				singleTextEditorApplicationInstance = NULL;
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
			if ( NULL != singleTextEditorApplicationInstance ) {
				singleTextEditorApplicationInstance->terminateRunningApplication();
				delete singleTextEditorApplicationInstance;
				singleTextEditorApplicationInstance = NULL;
			}
		}
		break;
    }
    return TRUE;
}

