//NewProjectCmd.h

#ifndef _NEWPROJECTCMD_H__
#define _NEWPROJECTCMD_H__



#include "AbstractCommand.h"


using namespace VCF;


namespace VCFBuilder  {

class Project;

/**
*Class NewProjectCmd documentation
*/
class NewProjectCmd : public VCF::AbstractCommand { 
public:
	NewProjectCmd( Project* project, const String& projectTemplateFilename );

	virtual ~NewProjectCmd();

	virtual void undo();

	virtual void redo();

	virtual void execute();
protected:
	Project* m_project;
	String m_projectTemplateFilename;
private:
};


}; //end of namespace VCFBuilder

#endif //_NEWPROJECTCMD_H__


