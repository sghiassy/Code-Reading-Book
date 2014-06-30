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

//Class.cpp
#include "FoundationKit.h"


using namespace VCF;



Class::Class( const String& className, const String& classID, const String& superClass )
{
	m_className = className;
	m_classID = classID;
	m_superClass = NULL;
	m_superClassName = superClass;
	m_propertyContainer.initContainer( m_properties );
	m_eventContainer.initContainer( m_events );
	m_interfaceContainer.initContainer( m_interfaces );
	m_methodContainer.initContainer(m_methods);
}

Class::~Class()
{	
	std::map<String,Property*>::iterator props = m_properties.begin();

	while ( props != m_properties.end() ){
		Property* prop = props->second;		
		delete prop;		
		props++;
	}
	
	m_properties.clear();

	std::map<String,EventProperty*>::iterator events = m_events.begin();
	while ( events != m_events.end() ){
		delete events->second;
		events ++;
	}
	m_events.clear();

	std::map<String,Method*>::iterator methods = m_methods.begin();
	while ( methods != m_methods.end() ){
		Method* method = methods->second;
		delete method;
		method = NULL;
		methods++;
	}
	m_methods.clear();

	std::map<String,InterfaceClass*>::iterator interfaces = m_interfaces.begin();
	while ( interfaces != m_interfaces.end() ){
		InterfaceClass* ic = interfaces->second;
		delete ic;
		ic = NULL;
		interfaces++;
	}
	m_interfaces.clear();
}

String Class::getClassName()
{
	return this->m_className;
}

bool Class::hasProperty( const String& propertyName )
{
	bool result = false;
    std::map<String,Property*>::iterator found = m_properties.find( propertyName );
	result = (found != m_properties.end());
	return result;
}

Property* Class::getProperty( const String& propertyName )
{
	Property* result = NULL;
	
	std::map<String,Property*>::iterator found = m_properties.find( propertyName );
	if ( found != m_properties.end() ){
		result = found->second;
	}

	return result;
}

void Class::addEvent( EventProperty* event )
{
	if ( NULL != event ) {
		String methodName = event->getEventMethodName();
		std::map<String,EventProperty*>::iterator found = m_events.find( methodName );
		if ( found != m_events.end() ){
			
		}
		else{
			
			m_events[methodName] = event;
		}
	}
}

EventProperty* Class::getEvent( const String& eventMethodName )
{
	EventProperty* result = NULL;
	std::map<String,EventProperty*>::iterator found = m_events.find( eventMethodName );
	if ( found != m_events.end() ) {
		result = found->second;
	}
	return result;
}

bool Class::hasEventHandler( const String& eventMethodName )
{
	bool result = false;
	std::map<String,EventProperty*>::iterator found = m_events.find( eventMethodName );
	result = (found != m_events.end());
	return result;
}

void Class::addProperty( Property* property )
{	
	if ( NULL != property ){		
		
		String name = property->getName();
		
		m_properties[name] = property;		
	}	
}

void Class::addMethod( Method* method )
{
	if ( NULL != method ){
		String name = method->getName();
		if ( false == this->hasMethod( name ) ){
			m_methods[name] = method;
		}
	}
}

bool Class::hasMethod( const String& methodName ){
	bool result = false;
    std::map<String,Method*>::iterator found = m_methods.find( methodName );
	result = (found != m_methods.end());
	return result;
}

unsigned long Class::getMethodCount(){
	return m_methods.size();
}

Method* Class::getMethod( const String& methodName ){
	Method* result = NULL;
	
	std::map<String,Method*>::iterator found = m_methods.find( methodName );
	if ( found != m_methods.end() ){
		result = found->second;
	}

	return result;
}

void Class::setSource( Object* source )
{
	std::map<String,Property*>::iterator props = m_properties.begin();
	while ( props != m_properties.end() ){
		props->second->setSource( source );
		props++;
	}

	std::map<String,Method*>::iterator methods = m_methods.begin();
	while ( methods != m_methods.end() ){
		Method* method = methods->second;
		method->setSource( source );
		methods++;
	}

	std::map<String,InterfaceClass*>::iterator interfaces = m_interfaces.begin();
	while ( interfaces != m_interfaces.end() ){
		InterfaceClass* interfaceClass = interfaces->second;
		interfaceClass->setSource( source );
		interfaces++;
	}
}

unsigned long Class::getPropertyCount()
{
	return m_properties.size();
}

String Class::getID()
{
	return this->m_classID;
}

Class* Class::getSuperClass()
{
	if ( NULL == this->m_superClass ){
		ClassRegistry* registry = ClassRegistry::getClassRegistry();
		if ( NULL != registry ) {
			m_superClass = registry->getClass( this->m_superClassName );
		}
	}
	return this->m_superClass;
}

String Class::getClassNameForProperty( Property* property )
{
	String result = "";

	if ( NULL != property ){
		VariantData* val = property->get();
		if ( NULL != val ){
			switch ( val->type ){
				case PROP_INT: {
					result = CLASS_INTEGER;
				}
				break;

				case PROP_LONG: {
					result = CLASS_LONG;
				}
				break;

				case PROP_SHORT: {
					result = CLASS_SHORT;
				}
				break;

				case PROP_ULONG: {
					result = CLASS_ULONG;
				}
				break;

				case PROP_FLOAT: {
					result = CLASS_FLOAT;
				}
				break;

				case PROP_CHAR: {
					result = CLASS_CHAR;
				}
				break;

				case PROP_DOUBLE: {
					result = CLASS_DOUBLE;
				}
				break;

				case PROP_OBJECT: {
					result = property->getTypeClassName();				
				}
				break;

				case PROP_BOOL: {
					result = CLASS_BOOL;
				}
				break;

				case PROP_STRING: {
					result = CLASS_STRING;
				}
				break;
				
				case PROP_ENUM: {
					result = CLASS_ENUM;
				}
				break;
			}
		}
	}

	return result;
}

unsigned long Class::getInterfaceCount()
{
	return m_interfaces.size();
}

bool Class::hasInterface( const String& interfaceName )
{
	bool result = false;
	std::map<String, InterfaceClass*>::iterator it = m_interfaces.begin();	
	while ( (false == result) && (it != m_interfaces.end()) )  {
		InterfaceClass* ic = it->second;
		if ( interfaceName == ic->getInterfaceName() ) {
			result = true;
		}
		it ++;
	}

	return result;
}

bool Class::hasInterfaceID( const String& interfaceID )
{
	std::map<String, InterfaceClass*>::iterator found = m_interfaces.find( interfaceID );
	return found != m_interfaces.end();
}

InterfaceClass* Class::getInterfaceByName( const String& interfaceName )
{
	InterfaceClass* result = NULL;
	
	std::map<String, InterfaceClass*>::iterator it = m_interfaces.begin();	
	while ( (NULL == result) && (it != m_interfaces.end()) )  {
		InterfaceClass* ic = it->second;
		if ( interfaceName == ic->getInterfaceName() ) {
			result = ic;
		}
		it ++;
	}

	return result;
}

InterfaceClass* Class::getInterfaceByID( const String& interfaceID )
{
	InterfaceClass* result = NULL;

	std::map<String, InterfaceClass*>::iterator found = m_interfaces.find( interfaceID );
	if ( found != m_interfaces.end() ) {
		result = found->second;
	}

	return result;
}

void Class::addInterface( InterfaceClass* newInterface )
{
	m_interfaces[ newInterface->getID() ] = newInterface;
	//copy over method from existing interface in classregistry
	//if any exist
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	InterfaceClass* existingInterface = classRegistry->getInterfaceFromInterfaceID( newInterface->getID() );
	if ( NULL != existingInterface ) {
		Enumerator<Method*>* methods = existingInterface->getMethods();
		while ( true == methods->hasMoreElements() ) {
			Method* method = methods->nextElement();
			Method* newMethod = NULL;
			method->copy( &newMethod );
			if ( false == newInterface->hasMethod( method->getName() ) ) {
				newInterface->addMethod( newMethod );
			}
		}
	}
}
