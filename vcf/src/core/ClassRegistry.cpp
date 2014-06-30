/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

// ClassRegistry.cpp
#include "FoundationKit.h"

#include "CantCreateObjectException.h"

using namespace VCF;

ClassRegistry* ClassRegistry::create()
{
	if ( NULL == ClassRegistry::m_registryInstance ) {
		ClassRegistry::m_registryInstance = new ClassRegistry();
	}

	return ClassRegistry::m_registryInstance;
}

ClassRegistry::ClassRegistry()
{
	m_classContainer.initContainer( m_classMap );

	m_interfaceContainer.initContainer( m_interfaceMap );

#ifdef VCF_RTTI
	//register "Object" as default class	
	TypedClass<Object>* objClass = TypedClass<Object>::create("VCF::Object", OBJECT_CLASSID, "");

		//new TypedClass<Object>( "VCF::Object", OBJECT_CLASSID, "" );

	m_classMap["VCF::Object"] = objClass;
	m_classIDMap[OBJECT_CLASSID] = objClass;
#endif

	//objClass->init();	
}

ClassRegistry::~ClassRegistry()
{
	std::map<String,Class*>::iterator it = m_classMap.begin();
	while ( it != m_classMap.end() ){
		delete it->second;
		it ++;
	}

	std::map<String,InterfaceClass*>::iterator it2 = m_interfaceMap.begin();
	while ( it2 != m_interfaceMap.end() ){
		delete it2->second;
		it2 ++;
	}

	m_classMap.clear();

	m_classIDMap.clear();

	m_interfaceMap.clear();

	m_interfaceIDMap.clear();

	m_implementedInterfacesIDMap.clear();
}

ClassRegistry* ClassRegistry::getClassRegistry()
{
	return ClassRegistry::m_registryInstance;
}

Class* ClassRegistry::getClass( const String& className )
{
	Class* result = NULL;

	std::map<String,Class*>::iterator found = 
		m_classMap.find( className );

	if ( found != m_classMap.end() ){
		result = found->second;
	}

	return result;
}

Class* ClassRegistry::getClassFromClassID( const String& classID )
{
	Class* result = NULL;

	std::map<String,Class*>::iterator found = 
		m_classIDMap.find( classID );

	if ( found != m_classIDMap.end() ){
		result = found->second;
	}

	return result;
}

Class* ClassRegistry::getClass( Object* object )
{
	Class* result = NULL;
	std::map<String,Class*>::iterator found = 
		m_classMap.begin();

	Class* clazz = NULL;
	while ( found != m_classMap.end() ){
		clazz = found->second;
		if ( clazz->isEqual( object ) ){
			result = clazz;
			break;
		}
		found++;
	}

	return result;
}

void ClassRegistry::createNewInstance( const String& className, Object** newInstance )
{
	Class* clazz = NULL;
	*newInstance = NULL;
	std::map<String,Class*>::iterator found = 
		m_classMap.find( className );

	if ( found != m_classMap.end() ){
		clazz = found->second;
		clazz->createInstance( newInstance );
	}
	if ( NULL == *newInstance ) {
		throw CantCreateObjectException( "Unable to create an instance of class\"" + className + "\"" );
	}
}

void ClassRegistry::createNewInstanceFromClassID( const String& classID, Object** newInstance )
{
	Class* clazz = NULL;
	*newInstance = NULL;
	std::map<String,Class*>::iterator found = 
		m_classIDMap.find( classID );

	if ( found != m_classIDMap.end() ){
		clazz = found->second;
		clazz->createInstance( newInstance );
	}
}

void ClassRegistry::addClass( const String& className, Class* classToRegister )
{	
	if ( NULL == classToRegister )
		return; //throw exception ?

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	
	String classID = classToRegister->getID();

	std::map<String,Class*>::iterator found;
	bool classNotFound = true;
	if  ( classID != "" ) {
		found = classRegistry->m_classIDMap.find( classID );
		classNotFound = !( found != m_classIDMap.end() );
	}
	else  {
		found = classRegistry->m_classMap.find( className );
		classNotFound = !( found != m_classMap.end() );
	}	

	if ( true == classNotFound ){	
		classRegistry->m_classMap[className] = classToRegister;	
		
		if  ( classID != "" ) {	
			classRegistry->m_classIDMap[classID] = classToRegister;
		}

		Class* superClass = classToRegister->getSuperClass();
		Class* tmp = NULL;
		while ( NULL != superClass ){
			//copy over properties
			Enumerator<Property*>* props = superClass->getProperties();
			if ( NULL != props ){
				while ( props->hasMoreElements() ){
					Property* prop = props->nextElement();
					if ( NULL != prop ){						
						if ( false == classToRegister->hasProperty( prop->getName() ) ) {
							Property* newProp = NULL;
							prop->copy( &newProp );
							classToRegister->addProperty( newProp );		
						}
					}
				}
			}

			//copy over events
			Enumerator<EventProperty*>* events = superClass->getEvents();
			if ( NULL != events ){
				while ( events->hasMoreElements() ){
					EventProperty* event = events->nextElement();
					if ( NULL != event ){
						if ( false == classToRegister->hasEventHandler( event->getEventMethodName() ) ) {
							classToRegister->addEvent( new EventProperty( event->getEventClassName(),
																			event->getHandlerClassName(),
																			event->getEventMethodName() ) );
						}						
					}
				}
			}
			tmp = superClass;
			superClass = tmp->getSuperClass();
		}
	}
}

Enumerator<Class*>* ClassRegistry::getClasses()
{
	return m_classContainer.getEnumerator();
}

void ClassRegistry::dump()
{
	/*
	std::map<String,Class*>::iterator it = m_classMap.begin();

	while ( it != m_classMap.end() ){	
		std::cout << typeid(*(it->second)).name() << std::endl;
		it++;
	}
	*/
}

ImplementedInterfaceClass* ClassRegistry::getImplementedInterface( const String& implementingClassName, const String& interfaceID )
{
	ImplementedInterfaceClass* result = NULL;
	Class* clazz = this->getClass( implementingClassName );
	if ( NULL != clazz ) {
		result = (ImplementedInterfaceClass*)clazz->getInterfaceByID( interfaceID );
	}
	return result;
}


void ClassRegistry::addInterface( const String& interfaceName, InterfaceClass* interfaceToRegister )
{
	if ( NULL == interfaceToRegister )
		return; //throw exception ?

	//ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	
	String interfaceID = interfaceToRegister->getID();

	std::map<String,InterfaceClass*>::iterator found;
	bool interfaceNotFound = true;

	if  ( interfaceID != "" ) {
		found = m_interfaceIDMap.find( interfaceID );
		interfaceNotFound = !( found != m_interfaceIDMap.end() );
	}
	else  {
		found = m_interfaceMap.find( interfaceName );
		interfaceNotFound = !( found != m_interfaceMap.end() );
	}	

	if ( true == interfaceNotFound ){	
		m_interfaceMap[interfaceName] = interfaceToRegister;	
		
		if  ( interfaceID != "" ) {	
			m_interfaceIDMap[interfaceID] = interfaceToRegister;
		}

		InterfaceClass* superInterfaceClass = interfaceToRegister->getSuperInterfaceClass();
		InterfaceClass* tmp = NULL;
		while ( NULL != superInterfaceClass ){
			//copy over the methods
			Enumerator<Method*>* methods = superInterfaceClass->getMethods();
			while ( true == methods->hasMoreElements() ) {
				Method* method = methods->nextElement();
				Method* inheritedMethod = NULL;
				method->copy( &inheritedMethod );
				interfaceToRegister->addMethod( method );
			}						

			
			tmp = superInterfaceClass;
			superInterfaceClass = tmp->getSuperInterfaceClass();
		}
	}
}

Enumerator<InterfaceClass*>* ClassRegistry::getInterfaces()
{
	return m_interfaceContainer.getEnumerator();
}

InterfaceClass* ClassRegistry::getInterface( const String& interfaceName )
{
	InterfaceClass* result = NULL;	

	std::map<String,InterfaceClass*>::iterator found = 
		m_interfaceMap.find( interfaceName );

	if ( found != m_interfaceMap.end() ){
		result = found->second;
	}

	return result;
}

InterfaceClass* ClassRegistry::getInterfaceFromInterfaceID( const String& interfaceID )
{
	InterfaceClass* result = NULL;

	std::map<String,InterfaceClass*>::iterator found = 
		m_interfaceIDMap.find( interfaceID );

	if ( found != m_interfaceIDMap.end() ){
		result = found->second;
	}

	return result;
}

bool ClassRegistry::registerMethod( Method* method, const String& className, const bool& isInterfaceMethod )
{
	bool result = false;

	if ( true == isInterfaceMethod ) {
		InterfaceClass* interfaceClass = this->getInterface( className );
		if ( NULL != interfaceClass ) {
			if ( false == interfaceClass->hasMethod( method->getName() ) ){				
				interfaceClass->addMethod( method );
				result = true;
			}
		}
	}
	else {
		Class* clazz = this->getClass( className );
		if ( NULL != clazz ) {
			if ( false == clazz->hasMethod( method->getName() ) ){				
				clazz->addMethod( method );
				result = true;
			}
		}
	}
	return result;
}

bool ClassRegistry::registerMethodByUUID( Method* method, const String& UUID, const bool& isInterfaceMethod )
{
	bool result = false;

	if ( true == isInterfaceMethod ) {
		InterfaceClass* interfaceClass = this->getInterfaceFromInterfaceID( UUID );
		if ( NULL != interfaceClass ) {
			if ( false == interfaceClass->hasMethod( method->getName() ) ){				
				interfaceClass->addMethod( method );
				result = true;
			}
		}
	}
	else {
		Class* clazz = this->getClassFromClassID( UUID );
		if ( NULL != clazz ) {
			if ( false == clazz->hasMethod( method->getName() ) ){				
				clazz->addMethod( method );
				result = true;
			}
		}
	}

	return result;
}


void ClassRegistry::createNewInterfaceInstanceFromInterfaceName( const String& interfaceName, void** newInterfaceInstance )
{
	std::map<String,ImplementedInterfaceClass*>::iterator it = m_implementedInterfacesIDMap.begin();
	ImplementedInterfaceClass* foundInterface = NULL;
	while ( (it != m_implementedInterfacesIDMap.end()) && (NULL == foundInterface) ) {
		ImplementedInterfaceClass* implInterface = it->second;
		if ( implInterface->getInterfaceName() == interfaceName ) {
			foundInterface = implInterface;
		}
		it++;
	}
	if ( NULL != foundInterface ) {
		foundInterface->createInterfaceInstance( newInterfaceInstance );
	}
}

void ClassRegistry::createNewInterfaceInstanceFromInterfaceID( const String& interfaceID, void** newInterfaceInstance )
{
	std::map<String,ImplementedInterfaceClass*>::iterator found = m_implementedInterfacesIDMap.find( interfaceID );
	if ( found != m_implementedInterfacesIDMap.end() ) {
		ImplementedInterfaceClass* implInterface = found->second;
		implInterface->createInterfaceInstance( newInterfaceInstance );
	}
}

void ClassRegistry::addImplementedInterface( ImplementedInterfaceClass* interfaceToRegister )
{	
	m_implementedInterfacesIDMap[interfaceToRegister->getID()] = interfaceToRegister;
}