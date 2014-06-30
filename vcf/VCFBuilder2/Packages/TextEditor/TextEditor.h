//TextEditor.h

#ifndef _TEXTEDITOR_H__
#define _TEXTEDITOR_H__


#include "TextEditorConfig.h"
#include "LibraryApplication.h"


class TEXTEDITOR_API TextEditorApplication : public LibraryApplication {
public:
	TextEditorApplication();

	virtual ~TextEditorApplication(){};

	virtual bool initRunningApplication();

	virtual void terminateRunningApplication();

	static TextEditorApplication* getTextEditorApplication();

	Library* m_scintillaLib;
};



#endif //_TEXTEDITOR_H__