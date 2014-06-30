//NewComponentCmd.h

#ifndef _NEWCOMPONENTCMD_H__
#define _NEWCOMPONENTCMD_H__



#include "AbstractCommand.h"


using namespace VCF;


namespace VCFBuilder  {

class Project;

class Form;

/**
*Class NewComponentCmd documentation
*/
class NewComponentCmd : public VCF::AbstractCommand { 
public:
	NewComponentCmd( Class* componentClass, Project* project, Form* activeFormInst );

	virtual ~NewComponentCmd();

	virtual void undo();

	virtual void redo();

	virtual void execute();
protected:
	Class* m_componentClass;
	Project* m_project;
	Form* m_activeFormInst;
	Component* m_newComponent;
	String m_componentVFFString;
	String m_componentParentName;
	Container* findParentControl( Container* container );
private:
};


}; //end of namespace VCFBuilder

#endif //_NEWCOMPONENTCMD_H__


