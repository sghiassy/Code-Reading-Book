//TextEditorConfig.h

#ifndef _TEXTEDITORCONFIG_H__
#define _TEXTEDITORCONFIG_H__

#ifdef WIN32

#ifdef TEXTEDITOR_EXPORTS

#define	TEXTEDITOR_API		__declspec(dllexport)	

#else

#define	TEXTEDITOR_API		__declspec(dllimport)	

#endif //TEXTEDITOR_EXPORTS

#else 

#define	TEXTEDITOR_API

#endif //WIN32

#endif //_TEXTEDITORCONFIG_H__