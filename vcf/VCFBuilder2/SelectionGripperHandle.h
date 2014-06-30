//SelectionGripperHandle.h

#ifndef _SELECTIONGRIPPERHANDLE_H__
#define _SELECTIONGRIPPERHANDLE_H__



#include "CustomControl.h"


using namespace VCF;


#define SELECTIONGRIPPERHANDLE_CLASSID		"0d6ea590-46d4-4e0f-8ca9-dce4b38e67a6"


namespace VCFBuilder  {

/**
*Class SelectionGripperHandle documentation
*/
class SelectionGripperHandle : public VCF::CustomControl { 
public:
	BEGIN_CLASSINFO(SelectionGripperHandle, "VCFBuilder::SelectionGripperHandle", "VCF::CustomControl", SELECTIONGRIPPERHANDLE_CLASSID)
	END_CLASSINFO(SelectionGripperHandle)

	SelectionGripperHandle();

	virtual ~SelectionGripperHandle();

	virtual void paint( GraphicsContext* ctx );

	virtual double getPreferredWidth();

	virtual double getPreferredHeight();
protected:

private:
};


}; //end of namespace VCFBuilder

#endif //_SELECTIONGRIPPERHANDLE_H__


