//Project.h
#include "ApplicationKit.h"
#include "Project.h"
#include "Form.h"
#include "CPPClassInstance.h"
#include <algorithm>
#include "RuntimeException.h"
#include "ComponentNameManager.h"
#include "ComponentInstance.h"
#include "VCFBuilderUI.h"
#include "CodeGenerator.h"

using namespace VCF;

using namespace VCFBuilder;


Project::Project( const bool& createAsDefaultProject )
{
	init();	
}

Project::~Project()
{
	if ( NULL != m_codeGenerator ) {
		delete m_codeGenerator;
		m_codeGenerator = NULL;
	}
	delete m_controlHandler;
}

void Project::validate()
{
	ValidationEvent event( this );
	fireOnModelValidate( &event );
}

void Project::empty()
{
	m_projectObjects.clear();
	m_projectForms.clear();
	m_projectClasses.clear();
	m_projectInstances.clear();

	ModelEvent event( this );
	fireOnModelEmptied( &event );
}

void Project::init()
{
	INIT_EVENT_HANDLER_LIST(ProjectChanged)
	INIT_EVENT_HANDLER_LIST(ModelEmptied)
	INIT_EVENT_HANDLER_LIST(ModelValidate)

	m_codeGenerator = NULL;
	m_mainForm = NULL;
	m_objContainer.initContainer( m_projectObjects );
	m_formContainer.initContainer( m_projectForms );
	m_classContainer.initContainer( m_projectClasses );
	m_classInstContainer.initContainer( m_projectInstances );

	m_controlHandler = 
		new ControlEventHandler<Project>( this,Project::onControlParentChanged, "Project::onControlParentChanged" );	

	//String appFileName = Application::getRunningInstance()->getFileName();	

	m_classNamePrefix = "";
	m_memberPrefix = "m_";

	if ( true == m_projectPath.empty() ) {
		m_projectPath = VCFBuilderUIApplication::getVCFBuilderUI()->getDefaultProjectPath() + "\\" + getName() + "\\";
	}
	m_includePath = m_projectPath;
	m_resourcePath = m_projectPath;
	m_srcPath = m_projectPath;
	m_vffPath = m_projectPath;

}

void Project::setName( const String& name ) 
{
	m_projectName = name;
	m_projectPath = VCFBuilderUIApplication::getVCFBuilderUI()->getDefaultProjectPath() + "\\" + m_projectName + "\\";

	m_includePath = m_projectPath;
	m_resourcePath = m_projectPath;
	m_srcPath = m_projectPath;
	m_vffPath = m_projectPath;

	ProjectEvent event( this, PROJECT_SETTINGS_CHANGED );
	fireOnProjectChanged( &event );
}

void Project::setPathName( const String& projectPath ) 
{
	m_projectPath = projectPath;
	if ( m_projectPath[m_projectPath.size()-1] != '\\' ) {
		m_projectPath += "\\";
	}
	m_includePath = m_projectPath;
	m_resourcePath = m_projectPath;
	m_srcPath = m_projectPath;
	m_vffPath = m_projectPath;
	
	ProjectEvent event( this, PROJECT_SETTINGS_CHANGED );
	fireOnProjectChanged( &event );
}

Enumerator<ProjectObject*>* Project::getObjectsInProject()
{
	return m_objContainer.getEnumerator();
}

void Project::addForm( Form* newForm )
{
	m_projectObjects.push_back( newForm );
	m_projectForms.push_back( newForm );
	
}

void Project::removeForm( Form* formToRemove )
{
	std::vector<ProjectObject*>::iterator found = std::find( m_projectObjects.begin(), m_projectObjects.end(), formToRemove );
	if ( found != m_projectObjects.end() ) {
		m_projectObjects.erase( found );
	}
	
	std::vector<Form*>::iterator found2 = std::find( m_projectForms.begin(), m_projectForms.end(), formToRemove );
	if ( found2 != m_projectForms.end() ) {
		m_projectForms.erase( found2 );
	}
	
	delete formToRemove;
	formToRemove = NULL;
}

void Project::addClass( CPPClass* newClass )
{
	m_projectClasses.push_back( newClass );
	m_projectObjects.push_back( newClass );
}

void Project::removeClass( CPPClass* classToRemove )
{
	std::vector<ProjectObject*>::iterator found = std::find( m_projectObjects.begin(), m_projectObjects.end(), classToRemove );
	if ( found != m_projectObjects.end() ) {
		m_projectObjects.erase( found );
	}

	std::vector<CPPClass*>::iterator found2 = std::find( m_projectClasses.begin(), m_projectClasses.end(), classToRemove );
	if ( found2 != m_projectClasses.end() ) {
		m_projectClasses.erase( found2 );
	}

	delete classToRemove;
	classToRemove = NULL;
}

void Project::addClassInstance( CPPClassInstance* newInstance )
{
	m_projectInstances.push_back( newInstance );
	m_projectObjects.push_back( newInstance );
}

void Project::removeClassInstance( CPPClassInstance* instanceToRemove )
{
	std::vector<ProjectObject*>::iterator found = std::find( m_projectObjects.begin(), m_projectObjects.end(), instanceToRemove );
	if ( found != m_projectObjects.end() ) {
		m_projectObjects.erase( found );
	}

	std::vector<CPPClassInstance*>::iterator found2 = std::find( m_projectInstances.begin(), m_projectInstances.end(), instanceToRemove );
	if ( found2 != m_projectInstances.end() ) {
		m_projectInstances.erase( found2 );
	}

	delete instanceToRemove;

	instanceToRemove = NULL;
}

Enumerator<Form*>* Project::getForms()
{
	return m_formContainer.getEnumerator();
}

Enumerator<CPPClass*>* Project::getClasses()
{
	return m_classContainer.getEnumerator();
}

Enumerator<CPPClassInstance*>* Project::getClassInstances()
{
	return m_classInstContainer.getEnumerator();
}

void Project::setMainForm( Form* mainForm )
{
	std::vector<ProjectObject*>::iterator found = std::find( m_projectObjects.begin(), m_projectObjects.end(), mainForm );
	if ( found != m_projectObjects.end() ) {
		m_mainForm = mainForm;
	}
	else {
		throw RuntimeException( "Form not found in project. Please use a form within this project." );
	}
}

Component* Project::addNewComponent( Class* componentClass, Form* form )
{	
	Component* newComponent = NULL;
	componentClass->createInstance( (Object**)&newComponent );	
	if ( NULL != newComponent ){
		String componentName = ComponentNameManager::getComponentNameMgr()->getUniqueNameFromClassName( componentClass->getClassName() );
		componentName = m_memberPrefix + componentName;
		newComponent->setName(componentName); 
		newComponent->setComponentState( CS_DESIGNING );
		ComponentInstance* newCompInst = new ComponentInstance(form);

		newCompInst->setOwningProject( this );
		form->addComponentInstance( newCompInst );

		newCompInst->setComponent( newComponent );
		newCompInst->setName( newComponent->getName() );		

		String key = form->getName() + "::" + newComponent->getName();

		m_projectComponents[key] = newCompInst;

		//update the code
		m_codeGenerator->setOwningForm( form );
		m_codeGenerator->addComponent( newComponent );

		Control* control = dynamic_cast<Control*>( newComponent );
		if ( control != NULL ) {
			control->addControlParentChangedHandler( m_controlHandler );
		}

		ProjectEvent event( this, newCompInst, PIT_COMPONENT, PROJECT_ITEM_ADDED );
		fireOnProjectChanged( &event );
	}
	return newComponent;
}

void Project::addExistingComponent( const String& componentName, Form* form )
{	
	ComponentInstance* newCompInst = new ComponentInstance(form);
	
	newCompInst->setOwningProject( this );
	form->addComponentInstance( newCompInst );	
	
	newCompInst->setName( componentName );
	
	String key = form->getName() + "::" + componentName;

	m_projectComponents[key] = newCompInst;
	
	//update the code
	m_codeGenerator->setOwningForm( form );
	
	ProjectEvent event( this, newCompInst, PIT_COMPONENT, PROJECT_ITEM_ADDED );
	fireOnProjectChanged( &event );	
}

void Project::linkComponentToComponentInstance( Component* component, ComponentInstance* compInstance )
{
	Form* form = (Form*)compInstance->getParent();

	String key = form->getName() + "::" + component->getName();
	m_projectComponents[key] = compInstance;
}

void Project::removeComponent( Component* component, Form* owningForm )
{
	String key = owningForm->getName() + "::" + component->getName();

	std::map<String,ProjectObject*>::iterator found = m_projectComponents.find( key );
					
	ComponentInstance* foundComponentInst = NULL;

	if ( found != m_projectComponents.end() ) {
		ProjectObject* projObj = found->second;
		m_projectComponents.erase( found );
		foundComponentInst = (ComponentInstance*)projObj;
	}

	if ( NULL != foundComponentInst ) {
		Form* form = (Form*)foundComponentInst->getParent();
		form->removeComponentInstance( foundComponentInst );

		m_codeGenerator->setOwningForm( form );
		m_codeGenerator->removeComponent( component );


		ProjectEvent event( this, foundComponentInst, PIT_COMPONENT, PROJECT_ITEM_REMOVED );
		fireOnProjectChanged( &event );

		delete foundComponentInst;
	}
}

void Project::onControlParentChanged( ControlEvent* e )
{
	Control* control = (Control*)e->getSource();
	/*
	std::map<Component*,ProjectObject*>::iterator found = m_projectComponents.find( control );
	if ( found != m_projectComponents.end() ) {
		
	}
	*/
}