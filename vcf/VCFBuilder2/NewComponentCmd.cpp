//NewComponentCmd.h
#include "ApplicationKit.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "NewComponentCmd.h"
#include "VCFBuilderUI.h"
#include "Project.h"
#include "VFFOutputStream.h"
#include "VFFInputStream.h"
#include "MainUIPanel.h"

using namespace VCF;

using namespace VCFBuilder;


NewComponentCmd::NewComponentCmd( Class* componentClass, Project* project, Form* activeFormInst )
{
	//need to get the component name and use that to retreive the component
	//pointer....
	m_componentClass = componentClass;
	m_activeFormInst = activeFormInst;
	m_project = project;
	m_newComponent = NULL;
	m_componentParentName = "";
	this->m_commandName = "Add new \"" + componentClass->getClassName() + "\" component"; 
}

NewComponentCmd::~NewComponentCmd()
{

}

void NewComponentCmd::undo()
{
	m_componentVFFString = "";
	TextOutputStream tos;
	VFFOutputStream vof( &tos );
	vof.writeComponent( m_newComponent );
	
	m_componentVFFString = tos.getTextBuffer();

	Control* control = dynamic_cast<Control*>( m_newComponent );
	m_componentParentName = control->getParent()->getName();
	m_project->removeComponent( m_newComponent, m_activeFormInst );
}

Container* NewComponentCmd::findParentControl( Container* container ) 
{
	Container* result = NULL;
	Control* foundControl = container->findControl( m_componentParentName );
	if ( foundControl == NULL ) {
		Enumerator<Control*>* controls = container->getChildren();
		if ( NULL != controls ) {
			while ( true == controls->hasMoreElements() ) {
				Container* childContainer = dynamic_cast<Container*>( controls->nextElement() );
				if ( NULL != childContainer ) {
					result = findParentControl( childContainer );
					if ( NULL != result ) {
						break;
					}
				}	
			}			
		}
	}
	else  {
		result = dynamic_cast<Container*>( foundControl );
	}
	return result;
}

void NewComponentCmd::redo()
{	
	m_newComponent = m_project->addNewComponent( m_componentClass, m_activeFormInst );

	BasicInputStream bis( m_componentVFFString );
	VFFInputStream vis( &bis );
	vis.readComponentInstance( m_newComponent );

	MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
	Frame* activeForm = mainUIPanel->getActiveFrameControl();

	Control* control = dynamic_cast<Control*>( m_newComponent );
	if ( (NULL != activeForm) && (NULL != control) ) {
		Container* parent = findParentControl( activeForm );
		if ( NULL != parent ) {
			parent->add( control );
		}
		else {
			//throw exception
		}
	}
}

void NewComponentCmd::execute()
{
	m_newComponent = m_project->addNewComponent( m_componentClass, m_activeFormInst );
}