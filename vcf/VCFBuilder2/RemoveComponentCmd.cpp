//RemoveComponentCmd.h
#include "ApplicationKit.h"
#include "RemoveComponentCmd.h"
#include "VCFBuilderUI.h"
#include "Project.h"
#include "TextOutputStream.h"
#include "VFFOutputStream.h"
#include "VFFInputStream.h"
#include "MainUIPanel.h"
#include "Form.h"
#include "CodeGenerator.h"

using namespace VCF;

using namespace VCFBuilder;


RemoveComponentCmd::RemoveComponentCmd( const String& componentName, Project* project, Form* activeFormInst )
{
	setName( "Delete" );
	m_componentName = componentName;
	m_project = project;
	m_activeFormInst = activeFormInst;
	m_component = this->findComponent();
	if ( NULL != m_component ) {
		m_componentClass = m_component->getClass();
		if ( NULL != m_componentClass ) {
			Enumerator<EventProperty*>* events = m_componentClass->getEvents();			
			MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
			Form* activeForm = mainUIPanel->getActiveForm();
			while ( true == events->hasMoreElements() ){
				EventProperty* event = events->nextElement();
				String eventMethodName = activeForm->findComponentEventHandler( m_component, event );
				if ( false == eventMethodName.empty() ) {
					m_connectedEvents[event] = eventMethodName;
				}
			}
		}
	}
	else {
		//throw exception !!
	}
}

RemoveComponentCmd::~RemoveComponentCmd()
{

}

void RemoveComponentCmd::undo()
{
	m_component = m_project->addNewComponent( m_componentClass, m_activeFormInst );

	BasicInputStream bis( m_componentVFFString );
	VFFInputStream vis( &bis );
	vis.readComponentInstance( m_component );

	MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
	Frame* activeFrame = mainUIPanel->getActiveFrameControl();

	Control* control = dynamic_cast<Control*>( m_component );
	if ( (NULL != activeFrame) && (NULL != control) ) {
		Container* parent = findParentControl( activeFrame );
		if ( NULL != parent ) {
			parent->add( control );			
			
			Form* activeForm = mainUIPanel->getActiveForm();

			try {								
				CodeGenerator* codeGen = m_project->getCodeGenerator();
				std::map<EventProperty*,String>::iterator it = m_connectedEvents.begin();
				while ( it != m_connectedEvents.end() ) {
					String eventCallBack = it->second;
					activeForm->addComponentEventHandler( m_component, it->first, eventCallBack );
					codeGen->addEvent( eventCallBack, m_component, it->first );
					it ++;
				}
			}
			catch ( BasicException& e ) {
				StringUtils::trace( "unable to add the event to " + activeForm->getName() + "\n") ;
				throw e;		
			}
			
		}
		else {
			//throw exception
		}
	}
}

void RemoveComponentCmd::redo()
{
	m_componentVFFString = "";
	TextOutputStream tos;
	VFFOutputStream vof( &tos );
	vof.writeComponent( m_component );
	
	m_componentVFFString = tos.getTextBuffer();
	

	MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
	Form* activeForm = mainUIPanel->getActiveForm();

	try {				
		CodeGenerator* codeGen = m_project->getCodeGenerator();
		std::map<EventProperty*,String>::iterator it = m_connectedEvents.begin();
		while ( it != m_connectedEvents.end() ) {
			String eventCallBack = it->second;
			activeForm->removeComponentEventHandler( m_component, it->first, eventCallBack );
			codeGen->removeEvent( eventCallBack, m_component, it->first );
			it ++;
		}

		Control* control = dynamic_cast<Control*>( m_component );
		m_componentParentName = control->getParent()->getName();
		m_project->removeComponent( m_component, activeForm );

	}
	catch ( BasicException& e ) {
		StringUtils::trace( "unable to remove the event from " + activeForm->getName() + "\n") ;
		throw e;		
	}
}

void RemoveComponentCmd::execute()
{
	redo();
}

Component* RemoveComponentCmd::findTheComponent( const String& componentName, Container* container, Control* containerControl )
{
	Component* result = NULL;
	result = containerControl->findComponent( componentName );
	if ( NULL == result ) {
		result = container->findControl( componentName );
		if ( NULL == result ) {
			Enumerator<Control*>* controls = container->getChildren();
			if ( NULL != controls ) {
				while ( true == controls->hasMoreElements() ) {
					Control* childControl = controls->nextElement();
					result = childControl->findComponent( componentName );
					if ( NULL == result ) {
						Container* childContainer = dynamic_cast<Container*>(childControl);
						if ( NULL != childContainer ) {
							result = findTheComponent( componentName, childContainer, childControl );
							if ( NULL != result ) {
								break;
							}
						}
					}
					else {
						break;
					}
				}
			}
		}
	}
	return result;
}

Component* RemoveComponentCmd::findComponent()
{
	Component* result = NULL;
	MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
	Frame* activeFrame = mainUIPanel->getActiveFrameControl();
	if ( m_componentName == activeFrame->getName() ) {
		result = activeFrame;
	}
	else {
		result = findTheComponent( m_componentName, dynamic_cast<Container*>(activeFrame), activeFrame );
	}
	return result;
}

Container* RemoveComponentCmd::findParentControl( Container* container ) 
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