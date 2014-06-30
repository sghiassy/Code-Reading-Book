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

// Class.h

#ifndef _CLASS_H__
#define _CLASS_H__



namespace VCF
{

class Property;
class Method;
class EventProperty;
class InterfaceClass;
/**
*common class names for native types, like int, long, char, bool, etc.
*/
#define CLASS_INTEGER		"integer"
#define CLASS_LONG			"long"
#define CLASS_ULONG			"ulong"
#define CLASS_FLOAT			"float"
#define CLASS_DOUBLE		"double"
#define CLASS_CHAR			"char"
#define CLASS_BOOL			"bool"
#define CLASS_SHORT			"short"
#define CLASS_ENUM			"enum"
#define CLASS_STRING		"string"

/**
*Class is the base class for all RTTI in the Framework. Class was written 
*because C++ RTTI is shockingly primitive, and many of these features are
*found in other OO languages (i.e. ObjectPascal, ObjectiveC, SmallTalk, Java, et. al)
*and are immensely useful and powerful.
*Class is an abstract base class that template class's derive from. Classes provide
*the following information:
*<ul>
*	<li>
*	the name of the Class - this is stored in a member variable rather 
*	than relying on typeid().name() to retreive the class name. Not all
*	compiler's support the typeid().name() function (i.e. MSVC 6.0 does, Borland
*	probably does, but GCC does not)
*	</li>
*
*	<li>
*	the Class ID - this represents a UUID for the class. This will prove useful
*	when distributed features creep in.
*	</li>
*
*	<li>
*	the ability to discover all the properties of a Class at runtime. A property
*	is defined as some class attribute that is provided access to via getter and 
*	setter methods. Properties can be primitive types (int, long double, etc) or 
*	Object derived types, or enums. Properties can also be collections of other 
*	properties. 
*	</li>
*
*	<li>
*	retreiving the super class of the class.
*	</li>
*
*	<li>
*	the ability to create a new instance of the class the Class object represents.
*	This of course assumes a default constructor is available.
*	</li>
*</ul>
*
*In order for the RTTI to work in the Framework developers of derived classes must do 
*three things for their classes to participate in the Framework. Failure to implement these
*steps will mean their classes will not have correct RTTI. A series of macros (defined in 
*ClassInfo.h) have been written to make this easier.
*<p>
*The first step is (obviously) making sure that your class is derived from a Framework object.
*For example:
*<pre>
*	<b>class</b> Foo : <b>public</b> VCF::Object {  <i>//this is OK</i>
*	...
*	};
*	
*	<b>class</b> Foo {  <i>//this is bad - there is no way to hook the RTTI up without at</i>
*	...					<i>//least deriving from VCF::Object</i>
*	};
*</pre>
*</p>
*<p>
*Next you should define a class id (as a string) for your class. If you are on winblows use 
*guidgen.exe to create UUID's for you. The define should look something like this:
*<pre>
*<b>#define</b> FOO_CLASSID			"1E8CBE21-2915-11d4-8E88-00207811CFAB"
*</pre>
*The next step is to add to macros to your class defintion (.h/.hpp file). These are required
*for the basic RTTI info (class-super class relation ships) and to make sure you'll inherit 
*any properties of your super class. For example:
*<pre>
*	<b>class</b> Foo : <b>public</b> VCF::Object {  
*		BEGIN_CLASSINFO(Foo, "Foo", "VCF::Object", FOO_CLASSID)
*		END_CLASSINFO(Foo)
*	...
*	};
*</pre>
*The macros create a public nested class used to register your class that you're writing.
*The above macros generate the following inline code for the developer of the Foo class (
*shown in red ).
*<pre>
*	<b>class</b> Foo : <b>public</b> VCF::Object {  
*<font color="red">
*		<b>class</b> FooInfo : <b>public</b> ClassInfo&ltFoo&gt { 
*		<b>public:</b> 
*			FooInfo( Foo* source ): 
* 				ClassInfo&ltFoo&gt( source, "Foo", "VCF::Object", "1E8CBE21-2915-11d4-8E88-00207811CFAB" ){ 
*				<b>if</b> ( true == isClassRegistered()  ){ 
*
*				} 
*			}; 
*		 
*			<b>virtual</b> ~FooInfo(){}; 
*     
*		};<i>//end of FooInfo</i>
*</font>
*		...
*	}; 
*</pre>
*
*The next step is to add any properties to the class. This is optional and is only neccessary if 
*the you want to expose the properties of the class you're writing. Adding properties is also
*done through macros and looks like this:
*<pre>
*	<b>class</b> Foo : <b>public</b> VCF::Object {  
*		BEGIN_CLASSINFO(Foo, "Foo", "VCF::Object", FOO_CLASSID)
*		PROPERTY( double, "fooVal", Foo::getFooVal, Foo::setFooVal, PROP_DOUBLE )
*		END_CLASSINFO(Foo)
*	...
*	<b>double</b> getFooVal();
*	<b>void</b> setFooVal( <b>const double</b>& val );
*	...
*	};
*</pre>
*If the properties are Object derived properties then the following can be done:
*<pre>
*	<b>class</b> Foo : <b>public</b> VCF::Object {  
*		BEGIN_CLASSINFO(Foo, "Foo", "VCF::Object", FOO_CLASSID)
*		OBJECT_PROPERTY( Foo, "fooObj", Foo::getFoo, Foo::setFoo )
*		END_CLASSINFO(Foo)
*	...
*	Foo* getFoo();
*	<b>void</b> setFoo( Foo* val );
*	...
*	};
*</pre>
*@author Jim Crafton
*@version 1.0	
*/
class FRAMEWORK_API Class : public Object
{
public:
	/**
	*the constructor for a new Class.
	*@param String the class name of the class
	*@param String the class id for the class. This MUST be a string 
	*that represents unique ID as returned by a UUID function/algorithm
	*the format is this:
	*96B6A350-2873-11d4-8E88-00207811CFAB
	*
	*@param String the name of the class's super class
	*/
	Class( const String& className, const String& classID, const String& superClass );

	virtual ~Class();
	
	/**
	*returns the super class for the class. If the Rect::getClass() was called
	*and then getSuperClass() was called on the return value of Rect::getClass(),
	*the return would be a Class* for Object.
	*/
	Class* getSuperClass();

	/**
	*returns the class id for the class. Class's may have the same name
	*so to prevent this, an ID is provided. This is ID MUST be generated using 
	*some algorithm that guarantees a valid UUID
	*/
	String getID();

	/**
	*returns the class name for the Class
	*/
	String getClassName();
	
	/**
	*returns an enumerator containing the Property values
	*the enumerator does not reflect the order in which the properties were added.
	*/
	Enumerator<Property*>* getProperties(){
		return m_propertyContainer.getEnumerator();	
	};

	/**
	*returns an enumerator containing the Methods of the Class
	*the enumerator does not reflect the order in which the properties were added.
	*/
	Enumerator<Method*>* getMethods(){
		return m_methodContainer.getEnumerator();	
	};
	

	/**
	*adds a new property to the Class's property map.
	*/
	void addProperty( Property* property );
	
	/**
	*does the Class have a have a particular property ? 
	*@param String the name of the property to find
	*@return bool true if the class has the specified property, otherwise false
	*/
	bool hasProperty( const String& propertyName );
	
	/**
	*the number of properties the Class has
	*/
	unsigned long getPropertyCount();

	/**
	*gets the property specified by propertyName, if the class 
	*has that property.
	*@param String the name of the property to try and retrieve
	*@return Property a pointer to a property of the class.
	*/
	Property* getProperty( const String& propertyName ); 

	/**
	*adds a new property to the Class's property map.
	*/
	void addMethod( Method* method );
	
	/**
	*does the Class have a have a particular property ? 
	*@param String the name of the property to find
	*@return bool true if the class has the specified property, otherwise false
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
	*@return Method a pointer to a method of the class.
	*/
	Method* getMethod( const String& methodName ); 	
	
	/**
	*Adds an event to the Class
	*@param EventProperty* the event to add - see EventProperty for more info.
	*/
	void addEvent( EventProperty* event );

	/**
	*does the Class have this particular event handler ?
	*@return bool returns true if the Class has an event handler by this name
	*/
	bool hasEventHandler( const String& eventMethodName );

	/**
	*returns an enumerator containing the EventProperty values.
	*the enumerator does not reflect the order in which the events were added.
	*/
	Enumerator<EventProperty*>* getEvents(){
		return m_eventContainer.getEnumerator();	
	};

	/**
	*Returns an EventProperty by name
	*@param String the event method to return
	*/
	EventProperty* getEvent( const String& eventMethodName );

	/**
	*Returns an enumeration of interfaces implemented by this class
	*/
	Enumerator<InterfaceClass*>* getInterfaces() {
		return m_interfaceContainer.getEnumerator();
	}

	unsigned long getInterfaceCount();

	bool hasInterface( const String& interfaceName );

	bool hasInterfaceID( const String& interfaceID );

	InterfaceClass* getInterfaceByName( const String& interfaceName );
	
	InterfaceClass* getInterfaceByID( const String& interfaceID );

	void addInterface( InterfaceClass* newInterface );
	
	/**
	*compares an object to the stored object instance. This uses
	*typeid which is OK in GCC. The actual compare is made in compareObject()
	*which is implemented in the derived Template classes
	@param Object another Class instance to compare this Class instance to
	@return bool true if the object's typeid is equal to this' typeid
	*/
	virtual bool isEqual( Object* object )const{
		return compareObject( object );
	};

	/**
	*creates a new instance of an Object based on the Class Type.
	*Actual implementation is performed by the template instance of the 
	*class.
	*/
	virtual void createInstance( Object** objectInstance ) = 0;

	/**
	*Used to compare the object instance passed in with the class type.
	*Implemented in the template derived class so a type safe compare is
	*made.
	*/
	virtual bool compareObject( Object* object )const = 0;

	/**
	*sets the source of all properties in the Class to source.
	*@param Object the source to which the properties are set
	*/
	void setSource( Object* source );

	/**
	*returns the class name of a Property. This will also 
	*return the correct type for C++ basic pritives (i.e. int,
	*bool, etc)
	*/
	static String getClassNameForProperty( Property* property );
private:	
	String m_className;
	String m_classID;
	String m_superClassName;
	Class* m_superClass;
	std::map<String,Property*> m_properties;	
	EnumeratorMapContainer<std::map<String,Property*>,Property*> m_propertyContainer;

	std::map<String,Method*> m_methods;	
	EnumeratorMapContainer<std::map<String,Method*>,Method*> m_methodContainer;

	std::map<String, EventProperty*> m_events;
	EnumeratorMapContainer<std::map<String,EventProperty*>,EventProperty*> m_eventContainer;

	std::map<String, InterfaceClass*> m_interfaces;
	EnumeratorMapContainer<std::map<String,InterfaceClass*>,InterfaceClass*> m_interfaceContainer;

};


#ifdef VCF_RTTI

/**
*TypedClass represents a specific instance of a Class. The CLASS_TYPE
is used to specify the Object the Class represents. So TypedClass<Rect> is 
*used to represent the Class for a Rect instance. 
*
*@author Jim Crafton
*@version 1.0
*/
template <class CLASS_TYPE> class FRAMEWORK_API TypedClass : public Class  
{
public:
	TypedClass( const String& className, const String& classID, const String& superClass ):
		Class( className, classID, superClass ){		
		
	};

	virtual ~TypedClass(){
		
	};	

	static TypedClass<CLASS_TYPE>* create(const String& className, const String& classID, const String& superClass) {
		TypedClass<CLASS_TYPE>* result = new TypedClass<CLASS_TYPE>(className, classID, superClass);
		result->init();
		return result;
	}

	/**
	*creates a new instance of type CLASS_TYPE and assigns it to the 
	*pointer represented by the objectInstance argument.
	*@param Object the pointer to the newly created instance.
	*/
	virtual void createInstance( Object** objectInstance ){
		*objectInstance = NULL;
		*objectInstance = new CLASS_TYPE();
	};
	

	virtual bool compareObject( Object* object )const{
		bool result = false;
		try {
			if ( typeid(*object) == typeid(CLASS_TYPE) ){
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
*TypedAbstractClass is used to represent abstract classes that cannot be instantiated 
*due to virtual pure methods, but must be represented in the class hierarchy because 
*they are derived from by other child classes.
*
*@author Jim Crafton
*@version 1.0
*/
template <class CLASS_TYPE> class FRAMEWORK_API TypedAbstractClass : public Class  
{
public:
	TypedAbstractClass( const String& className, const String& classID, const String& superClass ):
		Class( className, classID, superClass ){		
		
	};

	virtual ~TypedAbstractClass(){
		
	};	

	virtual void createInstance( Object** objectInstance ){
		*objectInstance = NULL;
		//this should probably throw an exception !
	};
	
	virtual bool compareObject( Object* object )const{
		bool result = false;
		if ( typeid(*object) == typeid(CLASS_TYPE) ){
			result = true;
		}
		return result;
	};	

};

#endif //#ifdef VCF_RTTI


};


#endif 
