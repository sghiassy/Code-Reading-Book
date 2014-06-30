//RemoveComponentCmd.h

#ifndef _REMOVECOMPONENTCMD_H__
#define _REMOVECOMPONENTCMD_H__



#include "AbstractCommand.h"


using namespace VCF;


namespace VCFBuilder  {

class Project;

class Form;

/**
*Class RemoveComponentCmd documentation
*/
class RemoveComponentCmd : public VCF::AbstractCommand { 
public:
	RemoveComponentCmd( const String& componentName, Project* project, Form* activeFormInst );

	virtual ~RemoveComponentCmd();

	virtual void undo();

	virtual void redo();

	virtual void execute();

protected:
	Class* m_componentClass;
	Component* m_component;
	Project* m_project;
	String m_componentName;
	String m_componentVFFString;
	String m_componentParentName;
	Form* m_activeFormInst;
	Container* findParentControl( Container* container );
	Component* findComponent();
	Component* findTheComponent( const String& componentName, Container* container, Control* containerControl );

	std::map<EventProperty*,String> m_connectedEvents;
private:
};


}; //end of namespace VCFBuilder

#endif //_REMOVECOMPONENTCMD_H__


