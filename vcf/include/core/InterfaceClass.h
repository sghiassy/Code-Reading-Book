//InterfaceClass.h

#ifndef _INTERFACECLASS_H__
#define _INTERFACECLASS_H__



namespace VCF  {

/**
*The InterfaceClass does much the same for interface classes that the 
*VCF::Class does for classes. It is a base class for RTTI for interfaces
*/
class FRAMEWORK_API InterfaceClass : public VCF::Object { 
public:
	InterfaceClass( const String& interfaceName, const String& interfaceID, const String& superInterfaceName );

	virtual ~InterfaceClass();

	/**
	*returns the interface id for the interface class. InterfaceClass's may have the same name
	*so to prevent this, an ID is provided. This is ID MUST be generated using 
	*some algorithm that guarantees a valid UUID
	*/
	String getID() {
		return m_interfaceID;
	}

	/**
	*returns the interface name of the InterfaceClass
	*/
	String getInterfaceName() {
		return m_interfaceName;
	}

	/**
	*returns an enumerator containing the Methods of the Class
	*the enumerator does not reflect the order in which the properties were added.
	*/
	Enumerator<Method*>* getMethods(){
		return m_methodContainer.getEnumerator();	
	}

	/**
	*adds a new method to the InterfaceClass's method map.
	*/
	void addMethod( Method* method );
	
	/**
	*does the InterfaceClass have a have a particular method ? 
	*@param String the name of the method to find
	*@return bool true if the interface has the specified method, otherwise false
	*/
	bool hasMethod( const String& methodName );
	
	/**
	*the number of methods the Class has
	*/
	unsigned long getMethodCount();

	/**
	*gets the method specified by methodName, if the class 
	*has that method.
	*@param String the name of the method to try and retrieve
	*@return Method a pointer to a method of the interface.
	*/
	Method* getMethod( const String& methodName ); 

	virtual bool isEqual( Object* object ) const;

	virtual bool compareObject( Object* object )const = 0;
	

	/**
	*sets the source for all methods in the InterfaceClass to source.
	*@param Object the source to which the methods are set
	*/
	void setSource( Object* source );

	/**
	*returns the InterfaceClass that this interface is derived
	*from, if any.
	*@return InterfaceClass the super interface this interface is derived 
	*from. May return NULL if the interface is not derived from any thing
	*/
	InterfaceClass* getSuperInterfaceClass();
protected:
	String m_superInterfaceClassName;
	String m_interfaceName;
	String m_interfaceID;
	std::map<String,Method*> m_methods;	
	EnumeratorMapContainer<std::map<String,Method*>,Method*> m_methodContainer;
private:
};



/**
*ImplementedInterfaceClass documentation
*/
class FRAMEWORK_API ImplementedInterfaceClass : public InterfaceClass { 
public:
	ImplementedInterfaceClass( const String& interfaceName, const String& interfaceID, const String& superInterfaceName );

	virtual ~ImplementedInterfaceClass();

	/**
	*creates a new instance of an Object that implements the interface 
	*represented by this InterfaceClass. Since multiple interfaces may 
	*be implemented by a given Object, more than one InterfaceClass may
	*end up creating the same Object instance.
	*/
	virtual void createImplementingInstance( Object** implementingObjectInstance ) = 0;

	/**
	*creates an instance of the interface. Sadly this requires the use of a void* 
	*since we can't possibly know the correct interface base type.
	*Perhaps this may be replaced the use of the Interface base class, though it
	*will still require a type cast by the caller into the appropriate type	
	*/
	virtual void createInterfaceInstance( void** interfaceInstance ) = 0;
protected:
};



/**
*TypedInterfaceClass documentation
*/
template<class INTERFACE_TYPE> class FRAMEWORK_API TypedInterfaceClass : public InterfaceClass {
public:
	TypedInterfaceClass( const String& interfaceName, const String& interfaceID, const String& superInterfaceName ) :
		InterfaceClass( interfaceName, interfaceID, superInterfaceName ) {
	}

	virtual ~TypedInterfaceClass(){};

	virtual bool compareObject( Object* object )const{
		bool result = false;
		try {
			if ( typeid(*object) == typeid(INTERFACE_TYPE) ){
				result = true;
			}
		}
		catch (...){
			throw ClassNotFound();
		}
		return result;
	};	

};


/**
*TypedImplementedInterfaceClass documentation
*/
template<class INTERFACE_TYPE, class IMPLEMENTER_TYPE> class FRAMEWORK_API TypedImplementedInterfaceClass : public ImplementedInterfaceClass {
public:
	TypedImplementedInterfaceClass( const String& interfaceName, const String& interfaceID, const String& superInterfaceName ) :
		ImplementedInterfaceClass( interfaceName, interfaceID, superInterfaceName ) {
		
	}

	virtual ~TypedImplementedInterfaceClass(){};

	virtual void createImplementingInstance( Object** implementingObjectInstance ) {
		*implementingObjectInstance = NULL;
		*implementingObjectInstance = new IMPLEMENTER_TYPE();
	}

	virtual void createInterfaceInstance( void** voidInterfaceInstance ) {
		*voidInterfaceInstance = NULL;

		IMPLEMENTER_TYPE* implementerInstance = new IMPLEMENTER_TYPE();
		INTERFACE_TYPE* interfaceInstance = dynamic_cast<INTERFACE_TYPE*>( implementerInstance );
		
		*voidInterfaceInstance = (void*)interfaceInstance;
	}


	virtual bool compareObject( Object* object )const{
		bool result = false;
		try {
			if ( typeid(*object) == typeid(INTERFACE_TYPE) ){
				result = true;
			}
		}
		catch (...){
			throw ClassNotFound();
		}
		return result;
	};	
};



}; //end of namespace VCF

#endif //_INTERFACECLASS_H__


