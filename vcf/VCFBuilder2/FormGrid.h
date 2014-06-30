//FormGrid.h

#ifndef _FORMGRID_H__
#define _FORMGRID_H__



#include "Panel.h"


using namespace VCF;


#define FORMGRID_CLASSID		"231319c3-8e49-4458-82e6-73e6eb39b27c"


namespace VCFBuilder  {

/**
*Class FormGrid documentation
*/
class FormGrid : public VCF::Panel { 
public:
	BEGIN_CLASSINFO(FormGrid, "VCFBuilder::FormGrid", "VCF::Panel", FORMGRID_CLASSID)
	END_CLASSINFO(FormGrid)

	FormGrid();

	virtual ~FormGrid();

	virtual void paint( GraphicsContext* ctx );

	bool isGridVisible() {
		return m_gridVisible;
	}

	void setIsGridVisible ( const bool& isGridVisible ) {
		m_gridVisible = isGridVisible;
		repaint();
	}
protected:
	bool m_gridVisible;
private:
};


}; //end of namespace VCFBuilder

#endif //_FORMGRID_H__


