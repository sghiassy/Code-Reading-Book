//SelectionGripper.h
#include "ApplicationKit.h"
#include "SelectionGripper.h"
#include "SelectionGripperHandle.h"


using namespace VCF;

using namespace VCFBuilder;


SelectionGripper::SelectionGripper()
{

}

SelectionGripper::~SelectionGripper()
{

}

void SelectionGripper::clearSelectionGripper()
{
	if ( false == m_gripperHandles.empty() ) {
		SelectionGripperHandle* handle = m_gripperHandles[0];
		Control* parent = handle->getParent();
		Container* container = dynamic_cast<Container*>(parent);
		std::vector<SelectionGripperHandle*>::iterator it = m_gripperHandles.begin();
		while (it != m_gripperHandles.end() ) {
			handle = *it;
			container->remove( handle );
			delete handle;
			handle = NULL;
			it++;
		}
		m_gripperHandles.clear();
	}
}