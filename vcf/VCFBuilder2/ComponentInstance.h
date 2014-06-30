//ComponentInstance.h

#ifndef _COMPONENTINSTANCE_H__
#define _COMPONENTINSTANCE_H__



#include "ProjectObject.h"


using namespace VCF;


#define COMPONENTINSTANCE_CLASSID		"cffe6459-f8ce-4312-ba5d-216b23ffbd42"


namespace VCFBuilder  {

/**
*Class ComponentInstance documentation
*/
class ComponentInstance : public ProjectObject { 
public:
	ComponentInstance( ProjectObject* parent );

	virtual ~ComponentInstance();

	Component* getComponent() {
		return m_component;
	}

	void setComponent( Component* component );

protected:
	Component* m_component;	
private:
};


}; //end of namespace VCFBuilder

#endif //_COMPONENTINSTANCE_H__


