//VCFBuilderApp.h

#ifndef _VCFBUILDERAPP_H__
#define _VCFBUILDERAPP_H__


#include "VCF.h"
#include "Enumerator.h"
#include "UndoRedoStack.h"
#include <vector>

using namespace VCF;


namespace VCFBuilder  {

class VCFBuilderUIApplication;

/**
*Class VCFBuilderApp documentation
*/
class VCFBuilderApp : public VCF::Application { 
public:
	VCFBuilderApp();

	virtual ~VCFBuilderApp();

	virtual bool initRunningApplication();

	/*
	void addProject( Project* project );

	void removeProject( Project* project );

	Enumerator<Project*>* getOpenProjects();

	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}

	Project* getCurrentOpenProject() {
		return m_currentOpenProject;
	}


	LISTENER_LIST(VCFBuilderAppListener);

	ADD_LISTENER(VCFBuilderAppListener);

	REMOVE_LISTENER(VCFBuilderAppListener);

	NOTIFIER(VCFBuilderAppListener, onProjectChanged, VCFBuilderAppEvent);

	static VCFBuilderApp* getVCFBuilderApp() {
		return (VCFBuilderApp*)Application::getRunningInstance();
	}

	String getObjectRepositoryPath() {
		return m_objectRepositoryPath;
	}

	String getProjectTemplatePath() {
		return m_projectTemplatePath;
	}

	String getDefaultProjectPath() {
		return m_defaultProjectPath;
	}
	*/
protected:
	VCFBuilderUIApplication* m_VCFBuidlerUILib;
private:
};



}; //end of namespace VCFBuilder

#endif //_VCFBUILDERAPP_H__


