//CPPClassInstance.h

#ifndef _CPPCLASSINSTANCE_H__
#define _CPPCLASSINSTANCE_H__



#include "CPPClass.h"


using namespace VCF;


#define CPPCLASSINSTANCE_CLASSID		"a461898d-7df3-4ca5-af86-bb78f93d66dc"


namespace VCFBuilder  {

/**
*Class CPPClassInstance documentation
*/
class CPPClassInstance : public CPPClass { 
public:	
	CPPClassInstance( ProjectObject* parent );

	virtual ~CPPClassInstance();

protected:

private:
};


}; //end of namespace VCFBuilder

#endif //_CPPCLASSINSTANCE_H__


