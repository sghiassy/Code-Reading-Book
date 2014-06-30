//SelectionGripperHandle.h
#include "ApplicationKit.h"
#include "SelectionGripperHandle.h"

using namespace VCF;

using namespace VCFBuilder;


SelectionGripperHandle::SelectionGripperHandle():
	CustomControl( true )
{
	setWidth( getPreferredWidth() );
	setHeight( getPreferredHeight() );
	setColor( Color::getColor("black") );
}

SelectionGripperHandle::~SelectionGripperHandle()
{

}

void SelectionGripperHandle::paint( GraphicsContext* ctx )
{
	CustomControl::paint( ctx );
}

double SelectionGripperHandle::getPreferredWidth()
{
	return 5;
}

double SelectionGripperHandle::getPreferredHeight()
{
	return 5;
}