//Form.h
#include "ApplicationKit.h"
#include "Form.h"
#include "RuntimeException.h"
#include "Project.h"
#include <algorithm>
#include "ComponentInstance.h"
#include "FileStream.h"
#include "VFFOutputStream.h"	
#include "TextOutputStream.h"
#include "FileUtils.h"
#include "CodeGenerator.h"


using namespace VCF;

using namespace VCFBuilder;


Form::Form( ProjectObject* parent, Project* owningProject )
{		
	setParent( parent );
	m_owningProject = owningProject;
	m_componentContainer.initContainer( m_components );
	m_rootComponent = NULL;	
	m_formClass = NULL;
}	

Form::~Form()
{
	
}

void Form::destroy()
{
	if ( NULL != m_formClass ) {
		delete m_formClass;
	}
}

void Form::setName( const String& name )
{
	String oldName = getName();
	ProjectObject::setName( name );
	if ( NULL == m_owningProject ) {
		throw RuntimeException( MAKE_ERROR_MSG_2("VCFBuilder::Form has no owning project") );
	}
	if ( true == m_owningProject->getVFFPath().empty() ) {
		throw RuntimeException( MAKE_ERROR_MSG_2("VCFBuilder::Form owning project doesn't have a valid VFF path") );
	}
	String oldVFFFilename = m_vffFileName;
	m_vffFileName = m_owningProject->getVFFPath() + name + ".vff";

	try {
		File vff( oldVFFFilename );
		vff.copyTo( m_vffFileName );
		vff.remove();
	}
	catch ( BasicException& e ) {
		//handle it and move on
	}
	

	CodeGenerator* codeGenerator = m_owningProject->getCodeGenerator();
	codeGenerator->setOwningForm( this );
	//the code generator should change the form name
	//and the forms class name as well
	
	codeGenerator->changeFormName( name, oldName );

	getFormClass()->setOwningProject( m_owningProject );
	getFormClass()->setName( name );	
}

void Form::addComponentInstance( ComponentInstance* componentInstance )
{
	m_components.push_back( componentInstance );
}

void Form::setRootComponent( Component* rootComponent )
{
	bool resetComponentVariables = (rootComponent == m_rootComponent);
	m_rootComponent = rootComponent;
	
	if ( true == resetComponentVariables ) {
		std::vector<ComponentInstance*>::iterator it = m_components.begin();
		while ( it != m_components.end() ) {
			ComponentInstance* componentInst = *it;
			if ( NULL == componentInst->getComponent() ) {
				Component* compVariable = NULL;
				if ( true == m_rootComponent->bindVariable( &compVariable, componentInst->getName() ) ) {
					componentInst->setComponent( compVariable );
				}
			}
			it ++;
		}
	}
}


void Form::removeComponentInstance( ComponentInstance* componentInstance )
{
	std::vector<ComponentInstance*>::iterator found = std::find( m_components.begin(), m_components.end(), componentInstance );
	if ( found != m_components.end() ) {
		m_components.erase( found );
	}
}

Enumerator<ComponentInstance*>* Form::getComponents()
{
	return m_componentContainer.getEnumerator();
}

void Form::saveVFF()
{
	try {
		if ( NULL == m_rootComponent ) {
			throw InvalidPointerException( MAKE_ERROR_MSG_2("VCFBuilder::Form doesn't have a valid root component") );
		}
		FileStream fs( m_vffFileName, FS_WRITE );
		TextOutputStream tos( &fs );
		VFFOutputStream vif( &tos );
		vif.writeComponent( m_rootComponent );
		/*
		m_formClass.setName( getName() );
		
		m_formClass.updateCodeFromForm( this );
		*/
	}
	catch ( InvalidPointerException& e ) {
		Dialog::showMessage( "Unable to save file because of error:\n" + e.getMessage(), MS_ERROR, "VCFBuilder Error" )	;
	}
	catch ( BasicException& e ) {
		Dialog::showMessage( "Unable to save file: " + m_vffFileName, MS_ERROR, "VCFBuilder Error" )	;
	}
}

void Form::addComponentEventHandler( Component* component, EventProperty* event, const String& eventHandlerMethodName )
{
	String key = component->getName() + "::" + event->getEventMethodName();
	String value = eventHandlerMethodName;
	m_registeredEventHandlerMap.insert( std::multimap<String,String>::value_type(key,value) );

}

void Form::addComponentEventHandler( const String& componentName, const String& eventMethod, const String& eventHandlerMethodName )
{
	String key = componentName + "::" + eventMethod;
	String value = eventHandlerMethodName;
	m_registeredEventHandlerMap.insert( std::multimap<String,String>::value_type(key,value) );
}

void Form::removeComponentEventHandler( Component* component, EventProperty* event, const String& eventHandlerMethodName )
{
	String key = component->getName() + "::" + event->getEventMethodName();	
	
	std::multimap<String,String>::iterator found = m_registeredEventHandlerMap.find( key );
	if ( found != m_registeredEventHandlerMap.end() ) {
		m_registeredEventHandlerMap.erase( found );
	}
}


String Form::findComponentEventHandler( Component* component, EventProperty* event )
{
	String result;
	
	String key = component->getName() + "::" + event->getEventMethodName();
	
	std::multimap<String,String>::iterator found = m_registeredEventHandlerMap.find( key );
	if ( found != m_registeredEventHandlerMap.end() ) {
		result = found->second;
	}
	
	return result;
}
