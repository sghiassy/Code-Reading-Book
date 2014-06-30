//SingleSelectionGripper.h

#ifndef _SINGLESELECTIONGRIPPER_H__
#define _SINGLESELECTIONGRIPPER_H__



#include "SelectionGripper.h"


using namespace VCF;


#define SINGLESELECTIONGRIPPER_CLASSID		"a2472d3d-84cd-4cad-a886-4b16a2f46c7f"


namespace VCFBuilder  {

/**
*Class SingleSelectionGripper documentation
*/
class SingleSelectionGripper : public SelectionGripper { 
public:
	BEGIN_CLASSINFO(SingleSelectionGripper, "VCFBuilder::SingleSelectionGripper", "VCFBuilder::SelectionGripper", SINGLESELECTIONGRIPPER_CLASSID)
	END_CLASSINFO(SingleSelectionGripper)

	SingleSelectionGripper();

	virtual ~SingleSelectionGripper();

	virtual void select();
	
	void onHandleMouseDown( MouseEvent* e );

	void onHandleMouseMove( MouseEvent* e );

	void onHandleMouseUp( MouseEvent* e );

	void dragSelectionRect( Control* control, Rect* rect );
protected:
	Control* m_selectedControl;

	void adjustSelectionRect( const long& tagID, Point* pt );

	void setHandlePosition( SelectionGripperHandle* handle, Rect* bounds );
private:
};


}; //end of namespace VCFBuilder

#endif //_SINGLESELECTIONGRIPPER_H__


