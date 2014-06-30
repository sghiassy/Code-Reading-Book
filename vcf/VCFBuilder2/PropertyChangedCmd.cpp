//PropertyChangedCmd.h
#include "ApplicationKit.h"
#include "PropertyChangedCmd.h"
#include "VCFBuilderUI.h"
#include "MainUIPanel.h"
#include "Project.h"
#include "Form.h"
#include "ComponentInstance.h"
#include "CodeGenerator.h"


using namespace VCF;

using namespace VCFBuilder;


PropertyChangedCmd::PropertyChangedCmd( const String& propertyName, const String& componentName, VariantData* newValue )
{
	m_propertyName = propertyName;
	m_componentName = componentName;
	//m_newStringVal = newValue->toString();
	m_newValue = *newValue;
	Component* component = findComponent();
	if ( NULL != component ) {
		Class* clazz = component->getClass();
		if ( NULL != clazz ) {
			Property* property = clazz->getProperty( m_propertyName );
			if ( NULL != property ) {
				m_originalValue = *(property->get());
				//m_originalStringVal = m_originalValue.toString();
			}
		}
	}
	this->m_commandName = "Set \"" + propertyName + "\"";
}

PropertyChangedCmd::~PropertyChangedCmd()
{

}

void PropertyChangedCmd::setProperty( VariantData* value )
{
	Component* component = findComponent();
	if ( NULL != component ) {
		Class* clazz = component->getClass();
		if ( NULL != clazz ) {
			Property* property = clazz->getProperty( m_propertyName );
			if ( NULL != property ) {
				property->set( value );
				Control* control = dynamic_cast<Control*>( property->getSource() );
				if ( NULL != control ) {
					control->repaint();
				}
			}
		}
	}
}

void PropertyChangedCmd::undo()
{
	try {
		//m_originalValue.setFromString( m_originalStringVal );
		setProperty( &m_originalValue );
	}
	catch( PropertyChangeException& e ) {
		//display error message
	}
	catch( ... ) {

	}
}

void PropertyChangedCmd::redo()
{
	try {
		//m_newValue.setFromString( m_newStringVal );
		setProperty( &m_newValue );
	}
	catch( PropertyChangeException& e ) {
		//display error message
	}
	catch( ... ) {

	}
}

void PropertyChangedCmd::execute()
{
	try {		
		
		MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
		Form* activeForm = mainUIPanel->getActiveForm();
		String formName = activeForm->getName();

		setProperty( &m_newValue );

		if ( NULL != activeForm ) {
			
			Enumerator<ComponentInstance*>* comps = activeForm->getComponents();
			ComponentInstance* found = NULL;
			while ( comps->hasMoreElements() ) {
				ComponentInstance* compInst = comps->nextElement();
				if ( m_componentName == compInst->getName()	) {
					found = compInst;
					break;
				}
			}
			
			if ( NULL != found ) {
				if ( m_propertyName == "name" ) {
					m_componentName = (String)m_newValue;
					found->setName( m_componentName );
				}				
			}
			
			if ( m_propertyName == "name" ) {
				
				//update the code here
				CodeGenerator* codeGen = activeForm->getOwningProject()->getCodeGenerator();
				codeGen->setOwningForm( activeForm );
				if ( NULL != found ) {
					codeGen->changeMemberName( m_newValue, m_originalValue );
					Project* ownerProject = found->getOwningProject();				
					ProjectEvent event( ownerProject, found, PIT_COMPONENT, PROJECT_ITEM_CHANGED );
					ownerProject->fireOnProjectChanged( &event );

				}
				else if ( (String)m_originalValue == formName ) {
					activeForm->setName( m_newValue );

					Project* ownerProject = activeForm->getOwningProject();				
					ProjectEvent event( ownerProject, activeForm, PIT_FORM, PROJECT_ITEM_CHANGED );
					ownerProject->fireOnProjectChanged( &event );
				}
			}
		}		
	}
	catch( PropertyChangeException& e ) {
		//display error message
	}
	catch( ... ) {

	}
}

Component* PropertyChangedCmd::findTheComponent( const String& componentName, Container* container, Control* containerControl )
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

Component* PropertyChangedCmd::findComponent()
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