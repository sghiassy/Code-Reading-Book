//TranslateControlCmd.h
#include "ApplicationKit.h"
#include "TranslateControlCmd.h"
#include "VCFBuilderUI.h"
#include "MainUIPanel.h"


using namespace VCF;

using namespace VCFBuilder;


TranslateControlCmd::TranslateControlCmd( const String& controlName, Rect* oldBounds, Rect* newBounds )
{
	m_controlName = controlName;
	m_oldBounds = *oldBounds;
	m_newBounds = *newBounds;
	m_commandName = "Move \"" + controlName + "\"";
}

TranslateControlCmd::~TranslateControlCmd()
{

}

Control* findTheControl( const String& controlName, Container* container )
{
	Control* result = NULL;
	
	result = container->findControl( controlName );
	if ( NULL == result ) {
		Enumerator<Control*>* controls = container->getChildren();
		if ( NULL != controls ) {
			while ( true == controls->hasMoreElements() ) {
				Control* childControl = controls->nextElement();
				Container* childContainer = dynamic_cast<Container*>(childControl);
				if ( NULL != childContainer ) {
					result = findTheControl( controlName, childContainer );
					if ( NULL != result ) {
						break;
					}
				}
			}
		}
	}
	return result;
}

Control* TranslateControlCmd::findControl()
{
	Control* result = NULL;
	MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
	Frame* activeFrame = mainUIPanel->getActiveFrameControl();
	if ( m_controlName == activeFrame->getName() ) {
		result = activeFrame;
	}
	else {
		result = findTheControl( m_controlName, dynamic_cast<Container*>(activeFrame) );
	}
	return result;
}

void TranslateControlCmd::undo()
{
	Control* control = findControl();
	if ( NULL != control ) {
		control->setBounds( &m_oldBounds );
	}
}

void TranslateControlCmd::redo()
{
	Control* control = findControl();
	if ( NULL != control ) {
		control->setBounds( &m_newBounds );
	}
}

void TranslateControlCmd::execute()
{
	Control* control = findControl();
	if ( NULL != control ) {
		control->setBounds( &m_newBounds );
	}
}


SizeControlCmd::SizeControlCmd(  const String& controlName, Rect* oldBounds, Rect* newBounds ):
	TranslateControlCmd( controlName, oldBounds, newBounds )
{
	m_commandName = "Size \"" + controlName + "\"";	
}