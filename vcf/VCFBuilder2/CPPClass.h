//CPPClass.h

#ifndef _CPPCLASS_H__
#define _CPPCLASS_H__



#include "GenericClass.h"


using namespace VCF;


#define CPPCLASS_CLASSID		"9b8a041d-b610-4730-bc45-dcb2e07d6a22"


namespace VCFBuilder  {


class Form;

/**
*Class CPPClass documentation
*/
class CPPClass : public GenericClass { 
public:
	CPPClass( ProjectObject* parent );

	virtual ~CPPClass();

	virtual String getImplFileName() {
		return m_implFileName;
	}

	virtual String getDeclFileName() {
		return m_declFileName;
	}

	virtual void setImplFileName( const String& implFileName );

	virtual void setDeclFileName( const String& declFileName );

	virtual void setName( const String& name );

	virtual String getNamespace() {
		return m_namespace;
	}

	virtual void setNamespace( const String& classNamespace ) {
		m_namespace = classNamespace;
	}

	//void updateCodeFromForm( Form* form );
protected:
	String m_implFileName;
	String m_declFileName;
	String m_namespace;
	String m_projectHeaderDir;
	String m_projectCPPDir;

	//void updateComponentMemberVariables( Form* form, String& classDecl );
private:
};


}; //end of namespace VCFBuilder

#endif //_CPPCLASS_H__


