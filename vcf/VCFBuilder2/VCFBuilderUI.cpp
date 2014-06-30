//VCFBuilderUI.cpp

//
#include "ApplicationKit.h"
#include "VCFBuilderUI.h"
#include "MainWindow.h"
#include "Project.h"
#include <algorithm>
#include "Desktop.h"
#include "MainUIPanel.h"

#include "XMLParser.h"
#include "Form.h"

#include "FileStream.h"
#include "SelectionManager.h"

#include "DefaultUIProject.h"
#include "ComponentEditor.h"
#include "AbstractComponentEditor.h"
#include "DefaultComponentEditor.h"
#include "ComponentEditorManager.h"

using namespace VCF;

using namespace VCFBuilder;


static VCFBuilderUIApplication* singleVCFBuilderUIApplicationInstance = NULL;



VCFBuilderUIApplication::VCFBuilderUIApplication()
{
	INIT_EVENT_HANDLER_LIST(ProjectChanged)

	m_container.initContainer( m_openProjects );
	
	m_currentProjectBeingParsed = NULL;
}

VCFBuilderUIApplication::~VCFBuilderUIApplication()
{

}

bool VCFBuilderUIApplication::initRunningApplication(){
	bool result = LibraryApplication::initRunningApplication();	

	//registers component editors
		
	ComponentEditorManager::registerComponentEditor( new DefaultComponentEditor(), REGISTER_FOR_ALL_COMPONENTS );

	Registry reg;
	reg.setRoot( RKT_CURRENT_USER );
	if ( true == reg.openKey( VCF_SOFTWARE_KEY ) ) {
		m_objectRepositoryPath = reg.getStringValue( OBJECT_REPOS_VALUE );
		this->m_projectTemplatePath = reg.getStringValue( PROJECT_TEMPLATE_VALUE );
		this->m_defaultProjectPath = reg.getStringValue( DEFAULT_PROJECT_VALUE );
	}

	return result;
}

void VCFBuilderUIApplication::initMainWindow()
{
	m_mainUIPanel = new MainUIPanel();	
}

VCFBuilderUIApplication* VCFBuilderUIApplication::getVCFBuilderUI()
{
	VCFBuilderUIApplication* result = singleVCFBuilderUIApplicationInstance;

	return result;
}

Enumerator<Project*>* VCFBuilderUIApplication::getOpenProjects()
{
	return m_container.getEnumerator();
}

void VCFBuilderUIApplication::addProject( Project* project )
{
	m_openProjects.push_back( project );
	m_currentOpenProject = project;

	std::vector<Project*>::iterator found = std::find( m_removedProjects.begin(), m_removedProjects.end(), project );
	if ( found != m_removedProjects.end() ) {
		m_removedProjects.erase( found );
	}
	VCFBuilderAppEvent event( this );
	fireOnProjectChanged( &event );
}

void VCFBuilderUIApplication::addExistingProject( const String& filename )
{
	DefaultUIProject* project = new DefaultUIProject(); //default for now

	loadProject( filename, project );

	addProject( project );
}

void VCFBuilderUIApplication::addNewProject( Project* project, const String& projectTemplateFilename )
{
	//load the project from disk here	

	loadProjectFromTemplate( projectTemplateFilename, project );

	addProject( project );
}

void VCFBuilderUIApplication::closeProject( Project* project )
{
	removeProject( project );
	getCommandStack()->clearCommands();
}

void VCFBuilderUIApplication::removeProject( Project* project )
{
	std::vector<Project*>::iterator found = std::find( m_openProjects.begin(), m_openProjects.end(), project );
	if ( found != m_openProjects.end() ) {
		m_openProjects.erase( found );
		std::vector<Project*>::iterator found = std::find( m_removedProjects.begin(), m_removedProjects.end(), project );
		if ( found != m_removedProjects.end() ) {
			m_removedProjects.push_back( project );
		}
	}
	int sz = m_openProjects.size();
	if ( sz >= 1 ) {
		m_currentOpenProject = m_openProjects[sz-1];
	}
	else {
		m_currentOpenProject = NULL;
	}
	VCFBuilderAppEvent event( this );
	fireOnProjectChanged( &event );
}

void VCFBuilderUIApplication::loadProjectFromTemplate( const String& filename, Project* project )
{
	m_parsedProjectUnitNodes.clear();

	File projectDir( project->getPathName() );

	m_currentProjectBeingParsed = project;
	XMLParser parser;
	FileStream fs( filename, FS_READ );
	parser.addNodeFoundHandler( 
		new XMLParserHandler<VCFBuilderUIApplication>( this, 
														VCFBuilderUIApplication::onTemplateProjectXMLNodeFound, 
														"AppXMLParserHandler" ) );
	parser.parse( &fs );

	m_currentProjectBeingParsed = NULL;
}

void VCFBuilderUIApplication::loadProject( const String& filename, Project* project )
{
	//create the projects path directory if neccessary
	m_parsedProjectUnitNodes.clear();	

	m_currentProjectBeingParsed = project;
	
	String path = StringUtils::getFileDir( filename );
	
	m_currentProjectBeingParsed->setPathName( path );

	XMLParser parser;
	FileStream fs( filename, FS_READ );
	parser.addNodeFoundHandler( 
		new XMLParserHandler<VCFBuilderUIApplication>( this, 
														VCFBuilderUIApplication::onProjectXMLNodeFound, 
														"AppXMLParserHandler" ) );
	parser.parse( &fs );

	m_currentProjectBeingParsed = NULL;
	
}

void VCFBuilderUIApplication::onTemplateProjectXMLNodeFound( XMLParserEvent* e )
{
	if ( NULL != m_currentProjectBeingParsed ) {
		XMLNode* node = e->getNode();
		if ( NULL != node ) {
			//only care about units and resources for now
			if ( node->getName() == "unit" ) {
				//Form* form  = m_currentProjectBeingParsed->createNewForm( NULL );
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLAttr* fileAttr = node->getAttrByName( "filename" );
				if ( (NULL != nameAttr) && (NULL != fileAttr) ) {
					File unit( fileAttr->getValue() );	
					String copyToPath = m_currentProjectBeingParsed->getIncludePath() + nameAttr->getValue();
					
					unit.copyTo( copyToPath );
					m_parsedProjectUnitNodes.push_back( node );
				}				
			}			
			else if ( node->getName() == "res" ) {
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLAttr* fileAttr = node->getAttrByName( "filename" );
				if ( (NULL != nameAttr) && (NULL != fileAttr) ) {
					File res( fileAttr->getValue() );	
					String copyToPath = m_currentProjectBeingParsed->getResourcePath() + nameAttr->getValue();
					
					res.copyTo( copyToPath );
				}	
			}
			else if ( node->getName() == "class" ) {
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLNode* declUnitNode = getProjectUnitNodeByName( nameAttr->getValue() + ".h" );
				XMLNode* implUnitNode = getProjectUnitNodeByName( nameAttr->getValue() + ".cpp" );
				
				GenericClass* aClass = m_currentProjectBeingParsed->createNewClass( NULL );
				CPPClass* cppClass = dynamic_cast<CPPClass*>( aClass );

				cppClass->setName( nameAttr->getValue() );
				cppClass->setOwningProject( m_currentProjectBeingParsed );
				
				cppClass->setDeclFileName( m_currentProjectBeingParsed->getIncludePath() + nameAttr->getValue() + ".h" );	

				
				cppClass->setImplFileName( m_currentProjectBeingParsed->getSrcPath() + nameAttr->getValue() + ".cpp" );	

				m_currentProjectBeingParsed->addClass( cppClass );

			}
			else if ( node->getName() == "form" ) {
				XMLAttr* vffAttr = node->getAttrByName( "vff" );
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLAttr* declUnitAttr = node->getAttrByName( "declUnit" );
				XMLAttr* implUnitAttr = node->getAttrByName( "implUnit" );				
				if ( (NULL != declUnitAttr) && (NULL != implUnitAttr) && (NULL != nameAttr) && (NULL != vffAttr) ) { 
					File vff( vffAttr->getValue() );	
					String copyToPath = m_currentProjectBeingParsed->getVFFPath() + nameAttr->getValue() + ".vff";
					
					vff.copyTo( copyToPath );

					XMLNode* unitNode = getProjectUnitNodeByName( declUnitAttr->getValue() );

					XMLAttr* defaultFormAttr = node->getAttrByName( "defaultForm" );					
					
					Form* form = m_currentProjectBeingParsed->createNewForm( NULL );
					form->setName( nameAttr->getValue() );
					m_currentProjectBeingParsed->addForm( form );
					
					if ( NULL != defaultFormAttr ) {
						if ( defaultFormAttr->getValue() == "true" ) {
							m_currentProjectBeingParsed->setMainForm( form );
						}
					}
				}
			}
		}
	}
}

void VCFBuilderUIApplication::onProjectXMLNodeFound( XMLParserEvent* e )
{
	if ( NULL != m_currentProjectBeingParsed ) {
		XMLNode* node = e->getNode();
		if ( NULL != node ) {
			//only care about units and resources for now
			if ( node->getName() == "unit" ) {
				//Form* form  = m_currentProjectBeingParsed->createNewForm( NULL );
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLAttr* fileAttr = node->getAttrByName( "filename" );
				if ( (NULL != nameAttr) && (NULL != fileAttr) ) {
					m_parsedProjectUnitNodes.push_back( node );
				}				
			}
			else if ( node->getName() == "project" ) {
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				m_currentProjectBeingParsed->setName( nameAttr->getValue() );
				
				XMLAttr* pathAttr = node->getAttrByName( "path" );
				if ( NULL != pathAttr ) {
					m_currentProjectBeingParsed->setPathName( pathAttr->getValue() );
				}
			}
			else if ( node->getName() == "res" ) {
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLAttr* fileAttr = node->getAttrByName( "filename" );
				if ( (NULL != nameAttr) && (NULL != fileAttr) ) {
					
				}	
			}
			else if ( node->getName() == "class" ) {
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLNode* declUnitNode = getProjectUnitNodeByName( nameAttr->getValue() + ".h" );
				XMLNode* implUnitNode = getProjectUnitNodeByName( nameAttr->getValue() + ".cpp" );
				
				GenericClass* aClass = m_currentProjectBeingParsed->createNewClass( NULL );
				
				CPPClass* cppClass = dynamic_cast<CPPClass*>( aClass );//ick fix this ....

				cppClass->setName( nameAttr->getValue() );
				cppClass->setOwningProject( m_currentProjectBeingParsed );

				XMLAttr* attr = declUnitNode->getAttrByName( "filename" );
				cppClass->setDeclFileName( attr->getValue() );	

				attr = implUnitNode->getAttrByName( "filename" );
				cppClass->setImplFileName( attr->getValue() );	

				m_currentProjectBeingParsed->addClass( cppClass );

			}
			else if ( node->getName() == "form" ) {
				XMLAttr* vffAttr = node->getAttrByName( "vff" );
				XMLAttr* nameAttr = node->getAttrByName( "name" );
				XMLAttr* declUnitAttr = node->getAttrByName( "declUnit" );
				XMLAttr* implUnitAttr = node->getAttrByName( "implUnit" );				
				if ( (NULL != declUnitAttr) && (NULL != implUnitAttr) && (NULL != nameAttr) && (NULL != vffAttr) ) { 
					

					XMLNode* unitNode = getProjectUnitNodeByName( declUnitAttr->getValue() );

					XMLAttr* defaultFormAttr = node->getAttrByName( "defaultForm" );					
					
					Form* form = m_currentProjectBeingParsed->createNewForm( NULL );
					form->setName( nameAttr->getValue() );
					m_currentProjectBeingParsed->addForm( form );
					
					if ( NULL != defaultFormAttr ) {
						if ( defaultFormAttr->getValue() == "true" ) {
							m_currentProjectBeingParsed->setMainForm( form );
						}
					}
				}
			}
		}
	}
}

XMLNode* VCFBuilderUIApplication::getProjectUnitNodeByName( const String& name )
{
	XMLNode* result = NULL;
	
	std::vector<XMLNode*>::iterator it = m_parsedProjectUnitNodes.begin();
	while ( it != m_parsedProjectUnitNodes.end() ) {
		XMLNode* node = *it;
		XMLAttr* nameAttr = node->getAttrByName( "name" );
		if ( nameAttr->getValue() == name ) {
			result = node;
			break;
		}
		it ++;
	}

	return result;
}


void VCFBuilderUIApplication::saveProject( const String& filename, Project* project )
{
	XMLNode* projectNode = new XMLNode( "project" );
	projectNode->addAttr( "name", project->getName() );
	projectNode->addAttr( "type", "cpp" );
	projectNode->addAttr( "date", "?" );
	projectNode->addAttr( "clsid", project->getClassName() );
	projectNode->addAttr( "path", project->getPathName() );
	XMLNode* settingsNode = projectNode->addChildNode( "settings" );

	XMLNode* sourcesNode = projectNode->addChildNode( "sources" );		

	XMLNode* formsNode = projectNode->addChildNode( "forms" );

	std::vector<String> formNames;

	Enumerator<Form*>* forms = project->getForms();
	while ( true == forms->hasMoreElements() ) {
		Form* form = forms->nextElement();

		XMLNode* formNode = formsNode->addChildNode( "form" );

		formNames.push_back( form->getName() );

		formNode->addAttr( "name", form->getName() );

		formNode->addAttr( "vff", form->getVFFFileName() );

		formNode->addAttr( "declUnit", form->getName() + ".h" );
		formNode->addAttr( "implUnit", form->getName() + ".cpp" );
	}

	XMLNode* classesNode = projectNode->addChildNode( "classes" );
	Enumerator<CPPClass*>* classes = project->getClasses();
	while ( true == classes->hasMoreElements() ) {
		CPPClass* aclass = classes->nextElement();
		std::vector<String>::iterator found = std::find( formNames.begin(), formNames.end(), aclass->getName() );
		if ( found == formNames.end() ) {
			
			XMLNode* classNode = classesNode->addChildNode( "class" );
			classNode->addAttr( "name", aclass->getName() );
			classNode->addAttr( "unit", aclass->getName() + ".h" );
			
			XMLNode* unitNode = sourcesNode->addChildNode( "unit" );
			String fname =  StringUtils::getFileName( aclass->getDeclFileName() );
			unitNode->addAttr( "name", fname );
			unitNode->addAttr( "filename", aclass->getDeclFileName() );
			
			unitNode = sourcesNode->addChildNode( "unit" );
			fname =  StringUtils::getFileName( aclass->getImplFileName() );
			unitNode->addAttr( "name", fname );
			unitNode->addAttr( "filename", aclass->getImplFileName() );
		}
	}

	XMLNode* resourcesNode = projectNode->addChildNode( "resources" );

	String s = projectNode->toString();

	FileStream fs( filename, FS_WRITE );
	fs << s ;

	delete projectNode;
}

void VCFBuilderUIApplication::idleTime()
{
	SelectionManager::getSelectionMgr()->cleanUpDeletableControls();
}






#ifdef WIN32

#include "Win32Peer.h"

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch ( ul_reason_for_call ) {
		case DLL_PROCESS_ATTACH:  {			

			singleVCFBuilderUIApplicationInstance = new VCFBuilderUIApplication();

			singleVCFBuilderUIApplicationInstance->getPeer()->setHandleID( (long)hModule );
			
			singleVCFBuilderUIApplicationInstance->setName( "VCFBuilderUI" );

			LibraryApplication::registerLibrary( singleVCFBuilderUIApplicationInstance );

			if ( false == singleVCFBuilderUIApplicationInstance->initRunningApplication() ) {
				singleVCFBuilderUIApplicationInstance->terminateRunningApplication();

				delete singleVCFBuilderUIApplicationInstance;

				singleVCFBuilderUIApplicationInstance = NULL;
			}
		}
		break;

		case DLL_THREAD_ATTACH: {

		}
		break;

		case DLL_THREAD_DETACH:  {

		}
		break;

		case DLL_PROCESS_DETACH:  {
			if ( NULL != singleVCFBuilderUIApplicationInstance ) {
				singleVCFBuilderUIApplicationInstance->terminateRunningApplication();
				delete singleVCFBuilderUIApplicationInstance;
				singleVCFBuilderUIApplicationInstance = NULL;
			}
		}
		break;
    }
    return TRUE;
}

#endif //WIN32

