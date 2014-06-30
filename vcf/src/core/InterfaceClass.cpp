//InterfaceClass.h
#include "FoundationKit.h"


using namespace VCF;
InterfaceClass::InterfaceClass( const String& interfaceName, const String& interfaceID, const String& superInterfaceName )
{
	m_interfaceName = interfaceName;
	m_interfaceID = interfaceID;
	
	m_superInterfaceClassName = superInterfaceName;

	m_methodContainer.initContainer( m_methods );
}

InterfaceClass::~InterfaceClass()
{
	std::map<String,Method*>::iterator it = m_methods.begin();
	while ( it != m_methods.end() ) {
		Method* m = it->second;
		delete m;
		m = NULL;
		it ++;
	}
	m_methods.clear();
}

void InterfaceClass::addMethod( Method* method )
{
	m_methods[ method->getName() ] = method;
}

bool InterfaceClass::hasMethod( const String& methodName )
{
	std::map<String,Method*>::iterator found = m_methods.find( methodName );
	return found != m_methods.end();
}

unsigned long InterfaceClass::getMethodCount()
{
	return m_methods.size();
}

Method* InterfaceClass::getMethod( const String& methodName )
{
	Method* result = NULL;
	std::map<String,Method*>::iterator found = m_methods.find( methodName );
	if ( found != m_methods.end() ) {
		result = found->second;
	}
	return result;
}

bool InterfaceClass::isEqual( Object* object ) const
{
	return compareObject( object );
}

void InterfaceClass::setSource( Object* source )
{
	std::map<String,Method*>::iterator it = m_methods.begin();
	while ( it != m_methods.end() ) {
		Method* m = it->second;
		m->setSource( source );
		it ++;
	}
}

InterfaceClass* InterfaceClass::getSuperInterfaceClass()
{
	InterfaceClass* result = NULL;

	return result;
}


/**
*ImplementedInterfaceClass
*/

ImplementedInterfaceClass::ImplementedInterfaceClass( const String& interfaceName, const String& interfaceID, const String& superInterfaceName ):
	InterfaceClass( interfaceName, interfaceID, superInterfaceName )
{

}

ImplementedInterfaceClass::~ImplementedInterfaceClass()
{

}