//SelectionGripper.h

#ifndef _SELECTIONGRIPPER_H__
#define _SELECTIONGRIPPER_H__


namespace VCFBuilder  {


#define SELECTIONGRIPPER_CLASSID		"8c7db002-a397-4ec6-b4a6-21bce6323bef"

class SelectionGripperHandle;

/**
*Class SelectionGripper documentation
*/
class SelectionGripper : public VCF::ObjectWithEvents { 
public:
	enum HandleType {
		HT_NW_HANDLE=0,
		HT_N_HANDLE,
		HT_NE_HANDLE,
		HT_E_HANDLE,
		HT_SE_HANDLE,
		HT_S_HANDLE,
		HT_SW_HANDLE,
		HT_W_HANDLE
	};

	BEGIN_ABSTRACT_CLASSINFO(SelectionGripper, "VCFBuilder::SelectionGripper", "VCF::Object", SELECTIONGRIPPER_CLASSID)
	END_CLASSINFO(SelectionGripper)

	SelectionGripper();

	virtual ~SelectionGripper();

	virtual void select() = 0;	
	
	virtual void clearSelectionGripper();
protected:
	std::vector<SelectionGripperHandle*> m_gripperHandles;
	VCF::Rect m_dragRect;
	VCF::Point m_dragPt;	
private:
};


}; //end of namespace VCFBuilder

#endif //_SELECTIONGRIPPER_H__


