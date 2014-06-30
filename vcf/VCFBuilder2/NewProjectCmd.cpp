//NewProjectCmd.h
#include "ApplicationKit.h"
#include "NewProjectCmd.h"
#include "VCFBuilderUI.h"


using namespace VCF;

using namespace VCFBuilder;


NewProjectCmd::NewProjectCmd( Project* project, const String& projectTemplateFilename )
{
	m_project = project;
	m_projectTemplateFilename = projectTemplateFilename;
	setName( "New Project" );
}

NewProjectCmd::~NewProjectCmd()
{

}

void NewProjectCmd::undo()
{
	VCFBuilderUIApplication::getVCFBuilderUI()->removeProject( m_project );
}

void NewProjectCmd::redo()
{
	VCFBuilderUIApplication::getVCFBuilderUI()->addNewProject( m_project, m_projectTemplateFilename );
}

void NewProjectCmd::execute()
{
	VCFBuilderUIApplication::getVCFBuilderUI()->addNewProject( m_project, m_projectTemplateFilename );
}