//GridRulerBtn.h

#ifndef _GRIDRULERBTN_H__
#define _GRIDRULERBTN_H__



#include "CustomControl.h"


using namespace VCF;


#define GRIDRULERBTN_CLASSID		"3e01b453-e1ab-47cb-a2cb-6f57082e56e3"


namespace VCFBuilder  {

/**
*Class GridRulerBtn documentation
*/
class GridRulerBtn : public VCF::CustomControl { 
public:
	BEGIN_CLASSINFO(GridRulerBtn, "VCFBuilder::GridRulerBtn", "VCF::CustomControl", GRIDRULERBTN_CLASSID)
	END_CLASSINFO(GridRulerBtn)

	GridRulerBtn();

	virtual ~GridRulerBtn();

	virtual void paint( GraphicsContext* ctx );
protected:

private:
};


}; //end of namespace VCFBuilder

#endif //_GRIDRULERBTN_H__


