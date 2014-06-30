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

// ClassRegistry.h

#ifndef _CLASSREGISTRY_H__
#define _CLASSREGISTRY_H__




namespace VCF
{


/**
*ClassRegistry contains all Class's in the Framework Runtime.
*The ClassRegistry is used by the Framework Runtime to register Classes 
*and properties for Classes. The ClassRegistry is also used to dynamically 
*create new instances of a class from a class name.
*
*@author Jim Crafton
*@version 1.0
*/
class FRAMEWORK_API ClassRegistry : public Object  
{
public:
	static ClassRegistry* create();

	ClassRegistry();

	virtual ~ClassRegistry();	

	/**
	*a static function that returns the global instance of the ClassRegistry.
	*/
	static ClassRegistry* getClassRegistry();	

	/**
	*returns a Class object from a string. If no matching 
	*Class exists the function returns NULL.
	*In terms of efficiency, this function is faster than 
	*getClass( Object*)
	*@param String the name of the class to find
	*@return Class the class associated with the className
	*/
	Class* getClass( const String& className );	

	/**
	*returns a Class object from a string representing a ClassID/UUID. 
	*If no matching Class exists the function returns NULL.
	*In terms of efficiency, this function is faster than 
	*getClass( Object*)
	*@param String the UUID or ClassID of the class to find
	*@return Class the class associated with the ClassID
	*/
	Class* getClassFromClassID( const String& classID );	

	/**
	*returns a Class object for the object passed in.
	*@param Object the object instance to get a class for
	*@return Class the class associated with the object
	*/
	Class* getClass( Object* object );

	/**
	*creates a new instance of an object based on the class name.
	*@param String the class name of the object to create
	*@param Object a pointer to the newly created object
	*/
	void createNewInstance( const String& className, Object** newInstance );	

	/**
	*creates a new instance of an object based on the classID.
	*@param String the classID of the object to create
	*@param Object a pointer to the newly created object
	*/
	void createNewInstanceFromClassID( const String& classID, Object** newInstance );	

	/**
	*see ImplementedInterfaceClass::createInterfaceInstance()
	*/
	void createNewInterfaceInstanceFromInterfaceName( const String& interfaceName, void** newInterfaceInstance );

	/**
	*see ImplementedInterfaceClass::createInterfaceInstance()
	*/
	void createNewInterfaceInstanceFromInterfaceID( const String& interfaceID, void** newInterfaceInstance );

	/**
	*adds a new Class object to the ClassRegistry
	*@param String the name of the class
	*@param Class the Class object to register
	*/
	void addClass( const String& className, Class* classToRegister );

	/**
	*adds a new InterfaceClass object to the ClassRegistry that represents 
	*a particular interface
	*@param String the name of the interface
	*@param InterfaceClass the InterfaceClass object to register
	*/
	void addInterface( const String& interfaceName, InterfaceClass* interfaceToRegister );

	/**
	*returns an enumeration of all the classes in the system at the time
	*of the call
	*/
	Enumerator<Class*>* getClasses();

	Enumerator<InterfaceClass*>* getInterfaces();

	/**
	*returns a ImplementedInterfaceClass object from a string. If no matching 
	*ImplementedInterfaceClass exists the function returns NULL.	
	*@param String the name of the implementing class to find
	*@paramString the interface UUID to find
	*@return ImplementedInterfaceClass the implemented interface associated with the implementingClassName
	*/
	ImplementedInterfaceClass* getImplementedInterface( const String& implementingClassName, const String& interfaceID );

	/**
	*returns a InterfaceClass object from a string. If no matching 
	*InterfaceClass exists the function returns NULL.	
	*@param String the name of the interface to find
	*@return InterfaceClass the interface associated with the interfaceName
	*/
	InterfaceClass* getInterface( const String& interfaceName );	

	/**
	*returns a InterfaceClass object from a string ID. If no matching 
	*InterfaceClass exists the function returns NULL.	
	*@param String the UUID of the interface to find
	*@return InterfaceClass the interface associated with the interfaceName
	*/
	InterfaceClass* getInterfaceFromInterfaceID( const String& interfaceID );	

	/**
	*Adds an implemented interface class to the registry
	*/
	void addImplementedInterface( ImplementedInterfaceClass* interfaceToRegister );

	/**
	*attempts to register the method with the class or interface
	*@return bool represents whether or not the registration attempt 
	*was successful. If the method returns false then the caller must
	*free the Method object that was passed in
	*/
	bool registerMethod( Method* method, const String& className, const bool& isInterfaceMethod );


	/**
	*attempts to register the method with the class or interface
	*@param Method the Method object that represents the method to registered
	*@param String the UUID of the class or interface to register the Method 
	*with
	*@param bool whether or not the Method is associated with an Interface
	*or a Class.
	*@return bool represents whether or not the registration attempt 
	*was successful. If the method returns false then the caller must
	*free the Method object that was passed in
	*/
	bool registerMethodByUUID( Method* method, const String& UUID, const bool& isInterfaceMethod );

	/**
	*Dumps out the classes in registery to stdout
	*/
	void dump();

	/**
	*the global ClassRegistry instance.
	*/
	static ClassRegistry* m_registryInstance;

private:
	EnumeratorMapContainer<std::map<String,Class*>,Class*> m_classContainer;
	EnumeratorMapContainer<std::map<String,InterfaceClass*>,InterfaceClass*> m_interfaceContainer;

	/**
	*a map of all the classes in the ClassRegistry. keyed by the class name 
	*/
	std::map<String,Class*> m_classMap;

	/**
	*a map of all the interfaces in the ClassRegistry. keyed by the interface name 
	*/
	std::map<String,InterfaceClass*> m_interfaceMap;

	/**
	*a map of all the classes in the ClassRegistry. keyed by the classID 
	*/
	std::map<String,InterfaceClass*> m_interfaceIDMap;
	
	/**
	*a map of all the classes in the ClassRegistry. keyed by the classID 
	*/
	std::map<String,ImplementedInterfaceClass*> m_implementedInterfacesIDMap;

	/**
	*a map of all the classes in the ClassRegistry. keyed by the classID 
	*/
	std::map<String,Class*> m_classIDMap;

};



#ifdef VCF_RTTI

/**
*this function registers a property with a class, where the PROPERTY_TYPE is defined as some 
*type like int, double, bool, Object*, etc.
*
*@param PROPERTY_TYPE initialValue is used partly for an initial value for the 
*property, but mainly to make sure thr template function is created properly.
*@param String className - the name of the class to associatent the property with
*@param String propertyName - the name of the property
*@param TypedProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the property
*@param TypedProperty<PROPERTY_TYPE>::SetFunction - the property's set function, allows for 
*setting values on the property
*@param PropertyDescriptorType the property type
*/
template <class PROPERTY_TYPE>  void registerProperty( const String& className,
                                                 const String& propertyName,
                                                 TypedProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
                                                 TypedProperty<PROPERTY_TYPE>::SetFunction propertySetFunction,
                                                 const PropertyDescriptorType& propertyFieldDescriptor ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedProperty<PROPERTY_TYPE>* newProperty = 
							new TypedProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                             propertySetFunction, 
														 propertyFieldDescriptor );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
}

template <class PROPERTY_TYPE>  void registerTypeDefProperty( const String& className,
                                                 const String& propertyName,
                                                 TypedProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
                                                 TypedProperty<PROPERTY_TYPE>::SetFunction propertySetFunction,
                                                 const PropertyDescriptorType& propertyFieldDescriptor,
												 const String& typeDefName ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypeDefProperty<PROPERTY_TYPE>* newProperty = 
							new TypeDefProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                             propertySetFunction, 
														 propertyFieldDescriptor, typeDefName  );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
}

/**
*this function registers a READ-ONLY property with a class, where the PROPERTY_TYPE is defined as some 
*basic type like int, double, bool, etc.
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the property
*@param PropertyDescriptorType the property type
*/
template <class PROPERTY_TYPE>  void registerReadOnlyProperty( 
												 const String& className, 
												 const String& propertyName,
												 TypedProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,												 
												 const PropertyDescriptorType& propertyFieldDescriptor ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedProperty<PROPERTY_TYPE>* newProperty = 
							new TypedProperty<PROPERTY_TYPE>( propertyGetFunction, 
														 propertyFieldDescriptor );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
}

template <class PROPERTY_TYPE>  void registerTypeDefReadOnlyProperty( const String& className,
                                                 const String& propertyName,
                                                 TypedProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,                                                 
                                                 const PropertyDescriptorType& propertyFieldDescriptor,
												 const String& typeDefName ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypeDefProperty<PROPERTY_TYPE>* newProperty = 
							new TypeDefProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                             propertySetFunction, typeDefName  );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
}

/**
*this function registers a READ-ONLY property with a class, where the PROPERTY_TYPE is defined as some 
*Oject* derived type
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedObjectProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the property
*/
template <class PROPERTY_TYPE>  void registerReadOnlyProperty( const String& className, 
												         const String& propertyName,
												         TypedObjectProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedObjectProperty<PROPERTY_TYPE>* newProperty = 
							new TypedObjectProperty<PROPERTY_TYPE>( propertyGetFunction );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
}

template <class PROPERTY_TYPE>  void registerReadOnlyPropertyRef( const String& className, const String& propertyName,
												TypedObjectRefProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedObjectRefProperty<PROPERTY_TYPE>* newProperty = 
							new TypedObjectRefProperty<PROPERTY_TYPE>( propertyGetFunction );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
}

/**
*this function registers a READ-ONLY property with a class, where the PROPERTY_TYPE is defined as some 
*enum type
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the enum property
*@param PROPERTY_TYPE the lower bound of enum
*@param PROPERTY_TYPE the upper bound of enum
*/
template <class PROPERTY_TYPE>  void registerReadOnlyProperty( const String& className, const String& propertyName,
												         TypedEnumProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
												         PROPERTY_TYPE lower, PROPERTY_TYPE upper ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedEnumProperty<PROPERTY_TYPE>* newProperty = 
							new TypedEnumProperty<PROPERTY_TYPE>( propertyGetFunction, lower, upper );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};

/**
*this function registers a READ-ONLY property with a class, where the PROPERTY_TYPE is defined as some 
*enum type. In addition, this also associates a human readable string with each enum value. These
*value's can then be represented more descriptively in an IDE than just their integer equivalent. 
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the enum property
*@param PROPERTY_TYPE the lower bound of enum
*@param PROPERTY_TYPE the upper bound of enum
*@param unsigned long the number of string names enumNames points to
*@param String a pointer to an array of Strings that holds a human 
*readable value for each enum type.
*/
template <class PROPERTY_TYPE>  void registerReadOnlyProperty( const String& className, const String& propertyName,
												         TypedEnumProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
												         PROPERTY_TYPE lower, PROPERTY_TYPE upper,
												         const unsigned long& enumNameCount, 
												         String* enumNames ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedEnumProperty<PROPERTY_TYPE>* newProperty = 
							new TypedEnumProperty<PROPERTY_TYPE>( propertyGetFunction, 
															 NULL,			
															 lower, upper,
															 enumNameCount,
															 enumNames );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};

/**
*this function registers a property with a class, where the PROPERTY_TYPE is defined as some 
*Object* derived type
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedObjectProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the property
*@param TypedObjectProperty<PROPERTY_TYPE>::SetFunction - the property's set function, allows for 
*setting values on the property
*/
template <class PROPERTY_TYPE>  void registerProperty( const String& className, const String& propertyName,
												TypedObjectProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
												TypedObjectProperty<PROPERTY_TYPE>::SetFunction propertySetFunction ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedObjectProperty<PROPERTY_TYPE>* newProperty = 
							new TypedObjectProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                             propertySetFunction );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};

template <class PROPERTY_TYPE>  void registerPropertyRef( const String& className, const String& propertyName,
												TypedObjectRefProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
												TypedObjectRefProperty<PROPERTY_TYPE>::SetFunction propertySetFunction ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedObjectRefProperty<PROPERTY_TYPE>* newProperty = 
							new TypedObjectRefProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                             propertySetFunction );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};
/**
*this function registers a property with a class, where the PROPERTY_TYPE is defined as some 
*enum type
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedEnumProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the property
*@param TypedEnumProperty<PROPERTY_TYPE>::SetFunction - the property's set function, allows for 
*setting values on the property
*@param PROPERTY_TYPE the lower bound of enum
*@param PROPERTY_TYPE the upper bound of enum
*/
template <class PROPERTY_TYPE>  void registerProperty( const String& className, const String& propertyName,
												 TypedEnumProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
												 TypedEnumProperty<PROPERTY_TYPE>::SetFunction propertySetFunction,
												 PROPERTY_TYPE lower, PROPERTY_TYPE upper ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedEnumProperty<PROPERTY_TYPE>* newProperty = 
							new TypedEnumProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                                 propertySetFunction, lower, upper );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};

/**
*this function registers a property with a class, where the PROPERTY_TYPE is defined as some 
*enum type. In addition, this also associates a human readable string with each enum value. These
*value's can then be represented more descriptively in an IDE than just their integer equivalent. 
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the enum property
*@param TypedEnumProperty<PROPERTY_TYPE>::SetFunction - the property's set function, allows for 
*setting values on the property
*@param PROPERTY_TYPE the lower bound of enum
*@param PROPERTY_TYPE the upper bound of enum
*@param unsigned long the number of string names enumNames points to
*@param String a pointer to an array of Strings that holds a human 
*readable value for each enum type.
*/
template <class PROPERTY_TYPE>  void registerProperty( const String& className, const String& propertyName,
												 TypedEnumProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
												 TypedEnumProperty<PROPERTY_TYPE>::SetFunction propertySetFunction,
												 PROPERTY_TYPE lower, PROPERTY_TYPE upper,
												 const unsigned long& enumNameCount, 
												 String* enumNames ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedEnumProperty<PROPERTY_TYPE>* newProperty = 
							new TypedEnumProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                                 propertySetFunction, 
															 lower, upper,
															 enumNameCount,
															 enumNames );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};



template <class PROPERTY_TYPE>  void registerCollectionProperty( const String& className, const String& propertyName,
												           TypedCollectionProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
												           const PropertyDescriptorType& propertyFieldDescriptor ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedCollectionProperty<PROPERTY_TYPE>* newProperty = 
							new TypedCollectionProperty<PROPERTY_TYPE>( propertyGetFunction, 
							                                       propertyFieldDescriptor );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};

/**
*this function registers a property with a class, where the PROPERTY_TYPE is defined as some 
*Object* derived type in a collection. In registering the collection, you must provide ways 
*to enumerate, add, insert, and delete from the collection. 
*
*class PROPERTY_TYPE is the template type to base the function on
*@param String className - the name of the class to associate the property with
*@param String propertyName - the name of the property
*@param TypedCollectionProperty<PROPERTY_TYPE>::GetFunction - the property's get function, allows for 
*retreiving values from the enum property
*@param TypedCollectionProperty<PROPERTY_TYPE>::AddFunction the collection's add function. Takes an
*item of PROPERTY_TYPE type
*@param TypedCollectionProperty<PROPERTY_TYPE>::InsertFunction the collection's insert function. Takes an
*item of PROPERTY_TYPE type and an index
*@param TypedCollectionProperty<PROPERTY_TYPE>::DeleteFunction1 the collection's delete function. Takes an
*item of PROPERTY_TYPE type
*@param TypedCollectionProperty<PROPERTY_TYPE>::DeleteFunction2 the collection's delete function. Takes 
*an index
*/
template <class PROPERTY_TYPE>  void registerCollectionProperty( const String& className, const String& propertyName,
												           TypedObjectCollectionProperty<PROPERTY_TYPE>::GetFunction propertyGetFunction,
														   TypedObjectCollectionProperty<PROPERTY_TYPE>::AddFunction propertyAddFunction, 
														   TypedObjectCollectionProperty<PROPERTY_TYPE>::InsertFunction propertyInsertFunction,
														   TypedObjectCollectionProperty<PROPERTY_TYPE>::DeleteFunction1 propertyDeleteFunction1,
														   TypedObjectCollectionProperty<PROPERTY_TYPE>::DeleteFunction2 propertyDeleteFunction2 ){
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Class* clazz = classRegistry->getClass( className );
	if ( NULL != clazz ){
		if ( false == clazz->hasProperty( propertyName ) ){
			TypedObjectCollectionProperty<PROPERTY_TYPE>* newProperty = 
							new TypedObjectCollectionProperty<PROPERTY_TYPE>( propertyGetFunction,
							                                             propertyAddFunction,
																		 propertyInsertFunction, 
																		 propertyDeleteFunction1,
																		 propertyDeleteFunction2 );
			newProperty->setName( propertyName );
			clazz->addProperty( newProperty );
		}
	}
};

/**
*registers a class in the global ClassRegistry instance.
*@param T - a dummy param to make the &*$#$% C++ compiler happy and ensure the 
*template function is created correctly. It would be nice to remove this because 
*it not used at all in the function
*@param String the name of the class to register
*@param String the name of the class's super class
*@param String the classID of the class to register. This MUST be 
*a string that represents unique ID as returned by a UUID function/algorithm
*
*@return bool true if the class has been registered, otherwise false
*/
template <class T> bool registerClass( T* fakeParam, const String& className, 
									   const String& superClassName, const String& classID ){
	
	bool result = false;
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	
	//StringUtils::trace( "registerClass for " + String(typeid(T).name()) + "\n" );
	
	Class* clazz = classRegistry->getClass( className );

	if ( NULL == clazz ){		
		//StringUtils::trace( "registering new class " + className + " with ClassRegistry\n" );

		TypedClass<T>* objClass = new TypedClass<T>( className, classID, superClassName );

		classRegistry->addClass( className, objClass );

		result = true;
	}
	return result;
};

template <class T>  bool registerAbstractClass( T* fakeParam, const String& className, 
									           const String& superClassName, const String& classID ){
	
	bool result = false;
	//StringUtils::trace( "registerAbstractClass for " + String(typeid(T).name()) + "\n" );
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();

	Class* clazz = classRegistry->getClass( className );

	if ( NULL == clazz ){		
		//StringUtils::trace( "registering new abstract class " + className + " with ClassRegistry\n" );
		TypedAbstractClass<T>* objClass = new TypedAbstractClass<T>( className, classID, superClassName );

		classRegistry->addClass( className, objClass );

		result = true;
	}
	return result;
};


/**
*registers an implemented interface with a particular class type. 
*/
template <class INTERFACE_TYPE, class IMPLEMENTER_TYPE> 
	bool registerImplementedInterface( INTERFACE_TYPE* fakeParam, const String& implementationClassName, const String& interfaceName,
								const String& interfaceID, const String& superInterfaceClassName ) {

	bool result = false;
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();

	InterfaceClass* interfaceClass = classRegistry->getImplementedInterface( implementationClassName, interfaceID );
	if ( NULL == interfaceClass ) {

		Class* clazz = classRegistry->getClass( implementationClassName );
		result = clazz->hasInterfaceID( interfaceID );
		if ( false == result ) {
			InterfaceClass* interfaceClassInst = 
				new TypedImplementedInterfaceClass<INTERFACE_TYPE,IMPLEMENTER_TYPE>( interfaceName, interfaceID, superInterfaceClassName );
			
			clazz->addInterface( interfaceClassInst );
			result = true;
		}
	}
	return result;
}


/**
*registers an interface in the class registry
*/
template <class INTERFACE_TYPE> 
	bool registerInterface( INTERFACE_TYPE* fakeParam, const String& interfaceName,
								const String& interfaceID, const String& superInterfaceClassName ) {

	bool result = false;
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();

	InterfaceClass* interfaceClass = classRegistry->getInterfaceFromInterfaceID( interfaceID );
	if ( NULL == interfaceClass ) {

		interfaceClass = new TypedInterfaceClass<INTERFACE_TYPE>( interfaceName, interfaceID, superInterfaceClassName );

		classRegistry->addInterface( interfaceName, interfaceClass );
		
		result = true;		
	}
	return result;
}

/**
*registers a new event witha class. Calls the Class::addEvent()
*if a class is found for the given clasName. Internally the class will
*create a new EventSet class if the event set did not previously exist 
*and the add the new event.
*MSVC note:
*the inline keyword was added to make the linker happy and stop it bitching
*about linker warning 4006. 
*/
inline FRAMEWORK_API bool registerEvent( const String& className, const String& eventClassName, 
											const String& handlerClassName, const String& eventMethodName ){
	bool result = false;
	
	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();

	Class* clazz = classRegistry->getClass( className );	

	if ( NULL != clazz ){
		if ( ! clazz->hasEventHandler( eventMethodName ) ) {
			clazz->addEvent( new EventProperty( eventClassName, handlerClassName, eventMethodName ) );
			result = true;
		}
	}
	return result;
}


/**
*registers a new method with a Class
*@param T - a dummy param to make the &*$#$% C++ compiler happy and ensure the 
*template function is created correctly. It would be nice to remove this because 
*it not used at all in the function
*@param String the name of the class to register
*@param String the name of the class's method to register
*@param METHOD_TYPE the method pointer of the class
*@param String the arg types of the method
*@param bool doe the method have a return value
*@param ulong32 the number of arguments of the method
*
*@return bool true if the method has been registered, otherwise false
*/
template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerMethod0Return( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
													    METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();

	Method* newMethod = new TypedMethodArg0Return<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}	

};


template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerMethod1Return( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
													    METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg1Return<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}

};

template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerMethod2Return( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
													    METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg2Return<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};

template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerMethod3Return( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
													    METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg3Return<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};

template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerMethod4Return( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
													    METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg4Return<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}

};

template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerMethod5Return( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
													    METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg5Return<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};


template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerMethod6Return( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
													    METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg6Return<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};

/**
*registers a new method with a Class
*@param T - a dummy param to make the &*$#$% C++ compiler happy and ensure the 
*template function is created correctly. It would be nice to remove this because 
*it not used at all in the function
*@param String the name of the class to register
*@param String the name of the class's method to register
*@param METHOD_TYPE the method pointer of the class
*@param String the arg types of the method
*@param bool doe the method have a return value
*@param ulong32 the number of arguments of the method
*
*@return bool true if the method has been registered, otherwise false
*/
 template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerVoidMethodArg0( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
														 METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg0<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};


template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerVoidMethodArg1( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
														 METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg1<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};

template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerVoidMethodArg2( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
														 METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg2<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};

template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerVoidMethodArg3( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
														 METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg3<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};

template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerVoidMethodArg4( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
														 METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg4<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};


template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerVoidMethodArg5( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
														 METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg5<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};


template<class SOURCE_TYPE, 
		 class METHOD_TYPE>  void registerVoidMethodArg6( SOURCE_TYPE* fakeParam, const String& className, const String& methodName, 
														 METHOD_TYPE methodPtr, const String& argTypes, const bool& isInterfaceMethod=false  ){

	ClassRegistry* classRegistry = ClassRegistry::getClassRegistry();
	Method* newMethod = new TypedMethodArg6<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes );
	newMethod->setName( methodName );
	if ( false == classRegistry->registerMethod( newMethod, className, isInterfaceMethod ) ) {
		delete newMethod;
		newMethod = NULL;
	}
};


#endif //#ifdef VCF_RTTI


};




#endif 

