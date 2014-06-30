//VCFBuilderUI.h

#ifndef _VCFBUILDERUI_H__
#define _VCFBUILDERUI_H__

#include "VCFBuilderUIConfig.h"


#include "LibraryApplication.h"
#include "Enumerator.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"


namespace VCF {
	class XMLParserEvent;
	class XMLNode;
};

namespace VCFBuilder  {


#define VCF_SOFTWARE_KEY		"Software\\VCF\\Builder\\"	
#define OBJECT_REPOS_VALUE		"ObjectRepository"
#define PROJECT_TEMPLATE_VALUE	"ProjectTemplatesDir"
#define DEFAULT_PROJECT_VALUE	"DefaultProjectDir"



class Project;
class MainUIPanel;

class VCFBUILDER_API VCFBuilderAppEvent : public Event {
public:
	VCFBuilderAppEvent( Object* source ):
	  Event( source ){};

	virtual ~VCFBuilderAppEvent(){};
	
	
};

template <class SOURCE_TYPE> class VCFBUILDER_API VCFBuilderAppEventHandler : public EventHandlerInstance<SOURCE_TYPE,VCFBuilderAppEvent> {
public:
	VCFBuilderAppEventHandler( SOURCE_TYPE* source,
		EventHandlerInstance<SOURCE_TYPE,VCFBuilderAppEvent>::OnEventHandlerMethod handlerMethod, 
		const String& handlerName="") :
			EventHandlerInstance<SOURCE_TYPE,VCFBuilderAppEvent>( source, handlerMethod, handlerName ) {
			
	}
	
	virtual ~VCFBuilderAppEventHandler(){};
};


class VCFBUILDER_API VCFBuilderUIApplication : public LibraryApplication {
public:
	
	VCFBuilderUIApplication();

	virtual ~VCFBuilderUIApplication();

	virtual bool initRunningApplication();

	void addProject( Project* project );

	void addNewProject( Project* project, const String& projectTemplateFilename );

	void addExistingProject( const String& filename );	

	void closeProject( Project* project );

	void removeProject( Project* project );

	Enumerator<Project*>* getOpenProjects();

	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}

	Project* getCurrentOpenProject() {
		return m_currentOpenProject;
	}


	EVENT_HANDLER_LIST(ProjectChanged)
	ADD_EVENT(ProjectChanged) 
	REMOVE_EVENT(ProjectChanged) 
	FIRE_EVENT(ProjectChanged,VCFBuilderAppEvent)

	static VCFBuilderUIApplication* getVCFBuilderUI();
	
	String getObjectRepositoryPath() {
		return m_objectRepositoryPath;
	}

	String getProjectTemplatePath() {
		return m_projectTemplatePath;
	}

	String getDefaultProjectPath() {
		return m_defaultProjectPath;
	}

	MainUIPanel* getMainUIPanel() {
		return m_mainUIPanel;
	}

	void loadProjectFromTemplate( const String& filename, Project* project );

	void loadProject( const String& filename, Project* project );

	void saveProject( const String& filename, Project* project );

	void initMainWindow();

	virtual void idleTime();
protected:
	String m_objectRepositoryPath;
	String m_projectTemplatePath;
	String m_defaultProjectPath;
	Project* m_currentOpenProject;
	std::vector<Project*> m_openProjects;
	std::vector<Project*> m_removedProjects;
	EnumeratorContainer<std::vector<Project*>,Project*> m_container;

	UndoRedoStack m_commandStack;

	MainUIPanel* m_mainUIPanel;
	Project* m_currentProjectBeingParsed;

	std::vector<XMLNode*> m_parsedProjectUnitNodes;
	XMLNode* getProjectUnitNodeByName( const String& name );

	void onTemplateProjectXMLNodeFound( XMLParserEvent* e );

	void onProjectXMLNodeFound( XMLParserEvent* e );
};


}; //end of namespace VCFBuilder

#endif //_VCFBUILDERUI_H__