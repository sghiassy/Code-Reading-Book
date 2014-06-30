//ObjectExplorer.h

#ifndef _OBJECTEXPLORER_H__
#define _OBJECTEXPLORER_H__



#include "TabbedPages.h"
#include "TableControl.h"
#include "AbstractScrollable.h"

using namespace VCF;


#define OBJECTEXPLORER_CLASSID		"7fc3d6b7-d6f2-4ca2-b192-1b795608ba2c"


namespace VCFBuilder  {

class SelectionManagerEvent;
/**
*Class ObjectExplorer documentation
*/
class ObjectExplorer : public VCF::TabbedPages { 
public:
	BEGIN_CLASSINFO(ObjectExplorer, "VCFBuilder::ObjectExplorer", "VCF::TabbedPages", OBJECTEXPLORER_CLASSID)
	END_CLASSINFO(ObjectExplorer)

	ObjectExplorer();

	virtual ~ObjectExplorer();

	void updateComponentProperties( Component* component );

	void updateComponentEvents( Component* component );
protected:
	void onSelectionManagerChanged( SelectionManagerEvent* e );	

	void clearEventEntries();

	void onEventItemDblClick( MouseEvent* e );

	void onEventItemChanged( ItemEditorEvent* e );

	TableItemEditor* m_prevEventItemEditor;

	ItemEditorEventHandler<ObjectExplorer>* m_itemEventHandler;

	TableControl* m_propertyTable;
	AbstractScrollable m_propertyScroller;

	TableControl* m_eventTable;	

	String m_selectedItemsOriginalEventHandlerCallback;
private:
};


}; //end of namespace VCFBuilder

#endif //_OBJECTEXPLORER_H__


