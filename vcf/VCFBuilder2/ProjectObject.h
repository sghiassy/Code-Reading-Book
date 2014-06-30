//ProjectObject.h

#ifndef _PROJECTOBJECT_H__
#define _PROJECTOBJECT_H__



#include "ClassInfo.h"


using namespace VCF;


#define PROJECTOBJECT_CLASSID		"fb51f0a8-7b14-441a-afe1-07bbd9be5d02"


namespace VCFBuilder  {

class Project;

class ProjectObjectTraverser;
/**
*Class ProjectObject documentation
*/
class ProjectObject : public VCF::Object { 
public:
	BEGIN_ABSTRACT_CLASSINFO(ProjectObject, "VCFBuilder::ProjectObject", "VCF::Object", PROJECTOBJECT_CLASSID)
	END_CLASSINFO(ProjectObject)

	ProjectObject();

	virtual ~ProjectObject();

	String getName() {
		return m_name;
	}

	virtual void setName( const String& name ) {
		m_name = name;
	}

	Project* getOwningProject() {
		return m_owningProject;
	}

	void setOwningProject( Project* owningProject ) {
		m_owningProject = owningProject;
	}

	ProjectObject* getParent(){
		return m_parentObject;
	}

	void setParent( ProjectObject* parent ) {
		m_parentObject = parent;
	}

	virtual void acceptTraversal( ProjectObjectTraverser* traverser );
protected:
	String m_name;
	Project* m_owningProject;
	ProjectObject* m_parentObject;
private:
};


}; //end of namespace VCFBuilder

#endif //_PROJECTOBJECT_H__


