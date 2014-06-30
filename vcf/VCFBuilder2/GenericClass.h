//GenericClass.h

#ifndef _GENERICCLASS_H__
#define _GENERICCLASS_H__

#include "ProjectObject.h"

namespace VCFBuilder  {

/**
*Class GenericClass documentation
*/
class GenericClass : public ProjectObject { 
public:
	GenericClass( ProjectObject* parent );

	virtual ~GenericClass();

	virtual String getImplFileName() {
		return "";
	}

	virtual String getDeclFileName() {
		return "";
	}

	void setSuperClassName( const String& superClassName ) {
		m_superClass = superClassName;
	}

	String getSuperClassName() {
		return m_superClass;
	}

	virtual void setImplFileName( const String& implFileName ){}

	virtual void setDeclFileName( const String& declFileName ) {}

	virtual void setName( const String& name ){};

	virtual String getNamespace() {
		return "";
	}

	virtual void setNamespace( const String& classNamespace ) {}

protected:
	String m_superClass;
};


}; //end of namespace VCFBuilder

#endif //_GENERICCLASS_H__


