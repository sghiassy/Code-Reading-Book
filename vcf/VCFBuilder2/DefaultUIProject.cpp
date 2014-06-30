//DefaultUIProject.h
#include "ApplicationKit.h"
#include "Project.h"
#include "DefaultUIProject.h"
#include "CodeGenerator.h"
#include "CPPCodeGenerator.h"
#include "GenericClass.h"
#include "CPPClass.h"
#include "Form.h"
#include "UIForm.h"
#include "VCFBuilderUI.h"

using namespace VCF;

using namespace VCFBuilder;


DefaultUIProject::DefaultUIProject()
{
	m_codeGenerator = new CPPCodeGenerator();

	
}

DefaultUIProject::~DefaultUIProject()
{

}

void DefaultUIProject::addForm( Form* newForm )
{
	Project::addForm( newForm );
	//talk to the code generator here
	getCodeGenerator()->addForm( newForm );
	
	Project::addClass( (CPPClass*)newForm->getFormClass() );
}

void DefaultUIProject::removeForm( Form* formToRemove )
{
	//std::vector<CPPClass*>::iterator found = std::find( m_projectClasses.begin(), m_projectClasses.end(), formToRemove->getFormClass() );
	//if ( found != m_projectClasses.end() ) {
	//	m_projectClasses.erase( found );
	//	}

	Project::removeForm( formToRemove );
}

void DefaultUIProject::addClass( CPPClass* newClass )
{
	Project::addClass( newClass );
	
	getCodeGenerator()->addClass( newClass );
}

void DefaultUIProject::removeClass( CPPClass* classToRemove )
{
	Project::removeClass( classToRemove );
}

void DefaultUIProject::addClassInstance( CPPClassInstance* newInstance )
{
	Project::addClassInstance( newInstance );
}

void DefaultUIProject::removeClassInstance( CPPClassInstance* instanceToRemove )
{
	Project::removeClassInstance( instanceToRemove );
}

GenericClass* DefaultUIProject::createNewClass( ProjectObject* parent )
{
	GenericClass* result = new CPPClass(parent);
	result->setOwningProject( this );
	return result;
}

Form* DefaultUIProject::createNewForm( ProjectObject* parent )
{
	Form* result = new UIForm( parent, this );
	result->setOwningProject( this );
	return result;
}	

void DefaultUIProject::intializeSettings()
{
	//create project dir
	File projectDir( m_projectPath );
	//copy over neccessary files
	File form1VFF( VCFBuilderUIApplication::getVCFBuilderUI()->getObjectRepositoryPath() + "\\Form1.vff" );
	form1VFF.copyTo( m_vffPath + "Form1.vff" );

	//create project objects
	
	Form* defaultForm = new UIForm( NULL, this );	
	defaultForm->setOwningProject( this );
	defaultForm->setName( "Form1" );
	addForm( defaultForm );
	
	CPPClass* appClass = new CPPClass(NULL);
	appClass->setOwningProject( this );
	appClass->setName( "MainApplication" );

	File mainAppIncl( VCFBuilderUIApplication::getVCFBuilderUI()->getObjectRepositoryPath() + "\\" + appClass->getName() + ".h" );
	mainAppIncl.copyTo( m_includePath + appClass->getName() + ".h" );

	File mainAppImpl( VCFBuilderUIApplication::getVCFBuilderUI()->getObjectRepositoryPath() + "\\" + appClass->getName() + ".cpp" );
	mainAppImpl.copyTo( m_srcPath + appClass->getName() + ".cpp" );

	addClass( appClass );
	
	m_mainForm = defaultForm;
}