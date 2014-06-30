//GridRuler.h

#ifndef _GRIDRULER_H__
#define _GRIDRULER_H__



#include "CustomControl.h"


using namespace VCF;


#define GRIDRULER_CLASSID		"1410be4f-9313-4d81-9a62-d88e2d48213f"


namespace VCFBuilder  {

/**
*Class GridRuler documentation
*/
class GridRuler : public VCF::CustomControl { 
public:
	BEGIN_CLASSINFO(GridRuler, "VCFBuilder::GridRuler", "VCF::CustomControl", GRIDRULER_CLASSID)
	PROPERTY(bool, "horizontalRuler", GridRuler::isHorizontalRuler, GridRuler::setHorizontalRuler, PROP_BOOL);
	END_CLASSINFO(GridRuler)

	GridRuler();

	virtual ~GridRuler();

	bool isHorizontalRuler();

	void setHorizontalRuler( const bool& horizontalRuler );

	virtual void paint( GraphicsContext* ctx );
protected:

private:
	bool m_horizontalRuler;
};


}; //end of namespace VCFBuilder

#endif //_GRIDRULER_H__


