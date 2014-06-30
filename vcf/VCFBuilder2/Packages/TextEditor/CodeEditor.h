//CodeEditor.h

#ifndef _CODEEDITOR_H__
#define _CODEEDITOR_H__

#include "TextEditorConfig.h"
#include "VCF.h"
#include "CustomControl.h"


namespace VCF {
	class TextModel;
};

using namespace VCF;


#define CODEEDITOR_CLASSID		"5836ad34-1065-4ffa-b6b1-2e3b62047309"


namespace VCFBuilder  {

/**
*Class CodeEditor documentation
*/
class TEXTEDITOR_API CodeEditor : public VCF::Control { 
public:
	BEGIN_CLASSINFO(CodeEditor, "VCFBuilder::CodeEditor", "VCF::CustomControl", CODEEDITOR_CLASSID)
	END_CLASSINFO(CodeEditor)

	CodeEditor();

	virtual void afterCreate( ComponentEvent* event );

	virtual bool isLightWeight();

	virtual void paint( GraphicsContext * context );

	virtual ~CodeEditor();

	TextModel* getTextModel() {
		return m_model;
	}
protected:

private:
	TextModel* m_model;
};


}; //end of namespace VCFBuilder

#endif //_CODEEDITOR_H__


