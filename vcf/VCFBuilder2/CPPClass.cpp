//CPPClass.h
#include "ApplicationKit.h"

#include "CPPClass.h"
#include "Project.h"
#include "FileStream.h"
#include "Form.h"
#include "FileUtils.h"
#include "TextOutputStream.h"
#include "ComponentInstance.h"
#include "ComponentInfo.h"
#include "CodeGenerator.h"


using namespace VCF;

using namespace VCFBuilder;


CPPClass::CPPClass(  ProjectObject* parent  ):
	GenericClass(parent)
{
	setParent( parent );
}

CPPClass::~CPPClass()
{

}

void CPPClass::setImplFileName( const String& implFileName )
{
	m_implFileName = implFileName;
}

void CPPClass::setDeclFileName( const String& declFileName )
{
	m_declFileName = declFileName;
}

void CPPClass::setName( const String& name )
{
	if ( m_projectHeaderDir.empty() ) {
		m_projectHeaderDir = this->m_owningProject->getIncludePath();
	}
	else {
		m_projectHeaderDir = StringUtils::getFileDir( m_declFileName );
	}

	if ( m_projectCPPDir.empty() ) {
		m_projectCPPDir = m_owningProject->getSrcPath();
	}
	else {
		m_projectCPPDir = StringUtils::getFileDir( m_implFileName );
	}
	String oldName = getName();
	ProjectObject::setName( name );	
	
	if ( m_projectHeaderDir[m_projectHeaderDir.size()-1] != '\\' ) {
		m_projectHeaderDir += "\\";
	}

	if ( m_projectCPPDir[m_projectCPPDir.size()-1] != '\\' ) {
		m_projectCPPDir += "\\";
	}
	String oldDeclName = m_declFileName;
	m_declFileName = m_projectHeaderDir + name + ".h";

	String oldImplName = m_implFileName;
	m_implFileName = m_projectCPPDir + name + ".cpp";

	try {
		if ( (false == oldDeclName.empty()) && (false == m_declFileName.empty()) ) {
			File declFile( oldDeclName );
			declFile.copyTo( m_declFileName );
			declFile.remove();
			
			File implFile( oldImplName );
			implFile.copyTo( m_implFileName );
			implFile.remove();
		}
	}
	catch ( BasicException& e ) {
		//handle it and move on
	}

	if ( (false == name.empty()) && (false == oldName.empty()) ) {
		CodeGenerator* codeGenerator = m_owningProject->getCodeGenerator();
		codeGenerator->setOwningClass( this );
		
		codeGenerator->changeClassName( name, oldName );
	}
}

