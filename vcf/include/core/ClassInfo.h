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

//ClassInfo.h


#ifndef _CLASSINFO_H__ 
#define _CLASSINFO_H__

#include "ClassRegistry.h"

namespace VCF
{


/**
*RTTI helper classes for properly registering RTTI meta data with the 
*class registry
*/

/**
*The ClassInfo is used to simplify registering 
*classes with the FoundationKit's runtime ClassRegistry.
*The class is derived from by using the macros below. It is
*created on the stack, and the constructor will automatically
*register the class, as well as any of the classes properties,
*methods, and events that have been specified through the macros.
*/
template <class CLASS_TYPE> class FRAMEWORK_API ClassInfo : public Object {
public:
	ClassInfo( const String& className, 
		       const String& superClassName, const String& classID ){
		m_isClassRegistered = registerClass<CLASS_TYPE>( NULL, className, superClassName, classID );
	};
	
	virtual ~ClassInfo(){};
	
	bool isClassRegistered(){
		return m_isClassRegistered;
	}
	
private:
	bool m_isClassRegistered;
};	


/**
*The AbstractClassInfo is used to simplify registering 
*classes with the FoundationKit's runtime ClassRegistry.
*It is uses specifically with classes that are abstract,
*in other words are not supposed to be created, only derived from.
*The class is derived from by using the macros below. It is
*created on the stack, and the constructor will automatically
*register the class, as well as any of the classes properties,
*methods, and events that have been specified through the macros.
*/
template <class CLASS_TYPE> class FRAMEWORK_API AbstractClassInfo : public Object {
public:
	AbstractClassInfo( const String& className, 
		       const String& superClassName, const String& classID ){
		
		m_isClassRegistered = registerAbstractClass<CLASS_TYPE>( NULL, className, superClassName, classID );
		
	};
	
	virtual ~AbstractClassInfo(){};
	
	bool isClassRegistered(){
		return m_isClassRegistered;
	}
	
private:
	bool m_isClassRegistered;
};


/**
*The InterfaceInfo class serves bascially 
*the same purpose as the ClassInfo class.
*It is used to register interfaces and their 
*methods with the 
*/
template <class INTERFACE_TYPE> class FRAMEWORK_API InterfaceInfo : public Object
{
public:
	InterfaceInfo( const String& interfaceName, 
					const String& interfaceID, 
					const String& superInterfaceClassName ) {

		m_isInterfaceRegistered = registerInterface<INTERFACE_TYPE>( NULL, interfaceName, interfaceID, superInterfaceClassName );
		
	}

	virtual ~InterfaceInfo(){};

protected:
	bool m_isInterfaceRegistered;
};




/**
*Object creation macros
*/

/**
*makes create with the default constructor pattern, i.e.
*a create() method with no arguments
*/
#define CREATE( ClassType ) \
	static ClassType* create() {\
		ClassType* result = new ClassType();\
		result->init(); \
		return result;\
	};\
	\

/**
*makes a create() method with the copy constructor pattern.
*The create() method takes a pointer to a object instance
*of the same type.
*@param ClassType a pointer an object to copy it's 
*initial data from
*/
#define CREATE_COPY( ClassType ) \
	static ClassType* create( ClassType* ClassType##Val ) {\
		ClassType* result = new ClassType( *ClassType##Val );\
		result->init(); \
		return result;\
	};\
	\


/**
*makes a create() method with a single argument. Requires a 
*constructor to also be declared that accepts a single argument
*of the same type.
*@param ArgType the type of argument to use in the create() method
*/
#define CREATE_1( ClassType, ArgType ) \
	static ClassType* create( ArgType arg ) {\
		ClassType* result = new ClassType( arg );\
		result->init(); \
		return result;\
	};\
	\

#define CREATE_2( ClassType, ArgType1, ArgType2 ) \
	static ClassType* create( ArgType1 arg1,  ArgType2 arg2 ) {\
		ClassType* result = new ClassType( arg1, arg2 );\
		result->init(); \
		return result;\
	};\
	\

#define CREATE_3( ClassType, ArgType1, ArgType2, ArgType3 ) \
	static ClassType* create( ArgType1 arg1,  ArgType2 arg2, ArgType3 arg3 ) {\
		ClassType* result = new ClassType( arg1, arg2, arg3 );\
		result->init(); \
		return result;\
	};\
	\

#define CREATE_4( ClassType, ArgType1, ArgType2, ArgType3, ArgType4 ) \
	static ClassType* create( ArgType1 arg1,  ArgType2 arg2, ArgType3 arg3, ArgType4 arg4 ) {\
		ClassType* result = new ClassType( arg1, arg2, arg3, arg4 );\
		result->init(); \
		return result;\
	};\
	\

#define CREATE_5( ClassType, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5 ) \
	static ClassType* create( ArgType1 arg1,  ArgType2 arg2, ArgType3 arg3, ArgType4 arg4, ArgType5 arg5 ) {\
		ClassType* result = new ClassType( arg1, arg2, arg3, arg4, arg5 );\
		result->init(); \
		return result;\
	};\
	\

#define CREATE_6( ClassType, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5, ArgType6 ) \
	static ClassType* create( ArgType1 arg1,  ArgType2 arg2, ArgType3 arg3, ArgType4 arg4, ArgType5 arg5, ArgType6 arg6 ) {\
		ClassType* result = new ClassType( arg1, arg2, arg3, arg4, arg5, arg6 );\
		result->init(); \
		return result;\
	};\
	\


#ifdef VCF_RTTI


/**
*RTTI macros 
*/
/**
*When defining your classes RTTI info in the VCF you will always start 
*with either this macro or the BEGIN_ABSTRACT_CLASSINFO macro.
*This will begin the declaration of a nested class derived from
*ClassInfo.
*@param  ClassType the class type to use. If you class is called Foo, 
*then you will pass in Foo here.
*@param ClassName the fully qualified class name to identify your class by.
*Please note that fully qualified means that namespaces need to also be 
*in the name. So if your class is called Foo in the namespace Baz, then 
*you would use then name "Baz::Foo" here
*@param superClassName the name of the super class for this class. Once
*again this must be a fully qualified classname (see above). If your class 
*was called Foo and derived from VCF::Object, then you would pass 
*in "VCF::Object" here.
*@param classID a string that represents a unique id. Preferably a UUID 
*that has been converted into a string representation.
*This macro must be paired with the END_CLASSINFO macro once 
*you are finished defining your RTTI.
*/
#define BEGIN_CLASSINFO( ClassType, ClassName, superClassName, classID ) \
	class ClassType##Info : public ClassInfo<ClassType> { \
	public: \
		ClassType##Info(): \
			ClassInfo<ClassType>( ClassName, superClassName, classID ){ \
			String tmpClassName = ClassName; \
			if ( true == isClassRegistered()  ){ \
			\
			    		
/**
*When defining your classes RTTI info in the VCF you will always start 
*with either this macro or the BEGIN_ABSTRACT_CLASSINFO macro.
*This will begin the declaration of a nested class derived from
*AbstractClassInfo. 
*@param  ClassType the class type to use. If you class is called Foo, 
*then you will pass in Foo here.
*@param ClassName the fully qualified class name to identify your class by.
*Please note that fully qualified means that namespaces need to also be 
*in the name. So if your class is called Foo in the namespace Baz, then 
*you would use then name "Baz::Foo" here
*@param superClassName the name of the super class for this class. Once
*again this must be a fully qualified classname (see above). If your class 
*was called Foo and derived from VCF::Object, then you would pass 
*in "VCF::Object" here.
*@param classID a string that represents a unique id. Preferably a UUID 
*that has been converted into a string representation.
*This macro must be paired with the END_CLASSINFO macro once 
*you are finished defining your RTTI.
*/
#define BEGIN_ABSTRACT_CLASSINFO( ClassType, ClassName, superClassName, classID ) \
	class ClassType##Info : public AbstractClassInfo<ClassType> { \
	public: \
		ClassType##Info(): \
			AbstractClassInfo<ClassType>( ClassName, superClassName, classID ){ \
			String tmpClassName = ClassName; \
			if ( true == isClassRegistered()  ){ \
			\

/**
*When defining your interfaces RTTI info in the VCF you will always start 
*with this macro. This will begin the declaration of a nested class 
*derived from InterfaceInfo. 
*@param  InterfaceType the class type to use for your interface. If your
*interface is called FooInterface, then you will pass in FooInterface here.
*@param InterfaceName the fully qualified class name to identify your class by.
*Please note that fully qualified means that namespaces need to also be 
*in the name. So if your class is called FooInterface in the namespace Baz, then 
*you would use then name "Baz::FooInterface" here
*@param SuperInterfaceName the name of the interface that this interface derives
*from. Once again this must be a fully qualified classname (see above). If your class 
*was called FooInterface and derived from VCF::Interface, then you would pass 
*in "VCF::Interface" here (this will be the case most of the time).
*@param InterfaceID a string that represents a unique id. Preferably a UUID 
*that has been converted into a string representation.
*This macro must be paired with the END_INTERFACEINFO macro once 
*you are finished defining your RTTI.
*/
#define BEGIN_INTERFACEINFO(InterfaceType,InterfaceName,SuperInterfaceName,InterfaceID)\
	class InterfaceType##Info : public InterfaceInfo<InterfaceType> {\
	public:\
		InterfaceType##Info():\
		InterfaceInfo<InterfaceType>(InterfaceName,InterfaceID,SuperInterfaceName){\
			String tmpInterfaceName = InterfaceName;\
			if ( true == m_isInterfaceRegistered ) {\
			\


/**
*This macro is used within a BEGIN_CLASSINFO/END_CLASSINFO block
*to indicate that this class fully implements the specified 
*interface. 
*/
#define IMPLEMENTS_INTERFACE( ClassType, InterfaceType, implementationClassName, interfaceName, interfaceID, superInterfaceClassName ) \
	registerImplementedInterface<InterfaceType,ClassType>( NULL, implementationClassName, interfaceName, interfaceID, superInterfaceClassName ); \
		\


/**
*Registers that the class fires an event. By declaring this it is expected that
*there are methods for adding event handler of the specified type that fire
*events of the specified event type.
*@param handlerClassName the name of the EventHandler derived class. This class 
*will contain a method pointer to some source object that wants to get notified 
*for the event. 
*@param eventClassName the class name of the Event type that is fired 
*by this class. 
*@param eventMethod the method name that is used in the supporting 
*event methods. So you might pass in "StreamExhausted". You could then 
*expect to find methods as follows declared in the class:
*<pre>
*   void addStreamExhaustedHandler( EventHandler* handler ) ;
*   void removeStreamExhaustedHandler( EventHandler* handler ) ;
*   void fireOnStreamExhausted( eventClassName* event ) ;
*</pre>
*/
#define EVENT(handlerClassName,eventClassName,eventMethod) \
		registerEvent( tmpClassName, eventClassName, handlerClassName, eventMethod ); \
		\



#define PROPERTY( type, propName, getFunc, setFunc, propType ) \
	            registerProperty<type>( tmpClassName, propName, \
							           (TypedProperty<type>::GetFunction)&getFunc, \
									   (TypedProperty<type>::SetFunction)&setFunc, \
									    propType ); \
										\

#define PROPERTY_TYPEDEF( type, propName, getFunc, setFunc, propType, typeName ) \
	            registerTypeDefProperty<type>( tmpClassName, propName, \
							           (TypedProperty<type>::GetFunction)&getFunc, \
									   (TypedProperty<type>::SetFunction)&setFunc, \
									    propType, typeName ); \
										\

#define READONLY_PROPERTY( type, propName, getFunc, propType ) \
	            registerReadOnlyProperty<type>( tmpClassName, propName, \
							                    (TypedProperty<type>::GetFunction)&getFunc, \
									            propType ); \
										                    \


#define READONLY_PROPERTY_TYPEDEF( type, propName, getFunc, propType, typeName ) \
	            registerTypeDefReadOnlyProperty<type>( tmpClassName, propName, \
							                    (TypedProperty<type>::GetFunction)&getFunc, \
									            propType, typeName ); \
										                    \

#define OBJECT_PROPERTY( type, propName, getFunc, setFunc ) \
				registerProperty<type>( tmpClassName, propName,  \
                            (TypedObjectProperty<type>::GetFunction)&getFunc,  \
							(TypedObjectProperty<type>::SetFunction)&setFunc );  \
							                                                   \


#define READONLY_OBJECT_PROPERTY( type, propName, getFunc ) \
				registerReadOnlyProperty<type>( tmpClassName, propName, \
                            (TypedObjectProperty<type>::GetFunction)&getFunc ); \
																		       \


#define OBJECT_PROPERTY_REF( type, propName, getFunc, setFunc ) \
				registerPropertyRef<type>( tmpClassName, propName,  \
                            (TypedObjectRefProperty<type>::GetFunction)&getFunc,  \
							(TypedObjectRefProperty<type>::SetFunction)&setFunc );  \
							                                                   \

#define READONLY_OBJECT_PROPERTY_REF( type, propName, getFunc ) \
				registerReadOnlyPropertyRef<type>( tmpClassName, propName,  \
                            (TypedObjectRefProperty<type>::GetFunction)getFunc );  \
							                                                   \


#define ENUM_PROPERTY(type,propName,getFunc,setFunc, lower, upper) \
				registerProperty<type>( tmpClassName, propName, \
									    (TypedEnumProperty<type>::GetFunction)getFunc, \
										(TypedEnumProperty<type>::SetFunction)setFunc, \
										lower, \
										upper); \
										 \

#define LABELED_ENUM_PROPERTY(type,propName,getFunc,setFunc, lower, upper, count, enumNames) \
				registerProperty<type>( tmpClassName, propName, \
									    (TypedEnumProperty<type>::GetFunction)getFunc, \
										(TypedEnumProperty<type>::SetFunction)setFunc, \
										lower, \
										upper, \
										count, \
										enumNames ); \
													\


#define READONLY_ENUM_PROPERTY(type,propName,getFunc,lower, upper) \
				registerReadOnlyProperty<type>( tmpClassName, propName, \
												(TypedEnumProperty<type>::GetFunction)getFunc, \
												lower, \
												upper); \
												        \


#define READONLY_LABELED_ENUM_PROPERTY(type,propName,getFunc,lower, upper, count, enumNames) \
				registerReadOnlyProperty<type>( tmpClassName, propName, \
									            (TypedEnumProperty<type>::GetFunction)getFunc, \
												lower, \
												upper, \
												count, \
												enumNames ); \
												             \


#define COLLECTION_PROPERTY( type,propName,getFunc, propType ) \
				registerCollectionProperty<type>( tmpClassName, propName, \
				                                  (TypedCollectionProperty<type>::GetFunction)getFunc, \
                                                  propType ); \
										            \

#define OBJECT_COLLECTION_PROPERTY( type,propName,getFunc, addFunc, insertFunc, deleteFunc1, deleteFunc2 ) \
				registerCollectionProperty<type>( tmpClassName, propName, \
				                                  (TypedObjectCollectionProperty<type>::GetFunction)getFunc, \
												  (TypedObjectCollectionProperty<type>::AddFunction)addFunc, \
												  (TypedObjectCollectionProperty<type>::InsertFunction)insertFunc, \
												  (TypedObjectCollectionProperty<type>::DeleteFunction1)deleteFunc1, \
												  (TypedObjectCollectionProperty<type>::DeleteFunction2)deleteFunc2 ); \
										            \

#define METHOD_VOID( methodName, classType, methType, methodPtr ) \
	typedef  void ( classType::* _T_##classType##methType )(void); \
	registerVoidMethodArg0<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, "" ); \
	  \

#define METHOD_1VOID( methodName, classType, methType, argType, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType); \
	registerVoidMethodArg1<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_2VOID( methodName, classType, methType, argType1, argType2, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2); \
	registerVoidMethodArg2<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_3VOID( methodName, classType, methType, argType1, argType2, argType3, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3); \
	registerVoidMethodArg3<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_4VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4); \
	registerVoidMethodArg4<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_5VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5); \
	registerVoidMethodArg5<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_6VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5, argType6); \
	registerVoidMethodArg6<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_RETURN( methodName, classType, methType, returnType, methodPtr ) \
	typedef  returnType ( classType::* _T_##classType##methType )(void); \
	registerMethod0Return<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, "" ); \
	  \

#define METHOD_1RETURN( methodName, classType, methType, returnType, argType, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType); \
	registerMethod1Return<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_2RETURN( methodName, classType, methType, returnType, argType1, argType2, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2); \
	registerMethod2Return<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_3RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3); \
	registerMethod3Return<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_4RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4); \
	registerMethod4Return<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_5RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5); \
	registerMethod5Return<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define METHOD_6RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5, argType6); \
	registerMethod6Return<classType,_T_##classType##methType##>( NULL, tmpClassName, methodName, methodPtr, argTypeDesc ); \
	  \

#define INTERFACE_METHOD_VOID( methodName, classType, methType, methodPtr ) \
	typedef  void ( classType::* _T_##classType##methType )(void); \
	registerVoidMethodArg0<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, "", true ); \
	  \

#define INTERFACE_METHOD1_VOID( methodName, classType, methType, argType, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType); \
	registerVoidMethodArg1<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD2_VOID( methodName, classType, methType, argType1, argType2, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2); \
	registerVoidMethodArg2<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD3_VOID( methodName, classType, methType, argType1, argType2, argType3, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3); \
	registerVoidMethodArg3<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD4_VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4); \
	registerVoidMethodArg4<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD5_VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5); \
	registerVoidMethodArg5<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD6_VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc ) \
	typedef  void ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5, argType6); \
	registerVoidMethodArg6<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD_RETURN( methodName, classType, methType, returnType, methodPtr ) \
	typedef  returnType ( classType::* _T_##classType##methType )(void); \
	registerMethod0Return<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, "", true ); \
	  \

#define INTERFACE_METHOD1_RETURN( methodName, classType, methType, returnType, argType, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType); \
	registerMethod1Return<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD2_RETURN( methodName, classType, methType, returnType, argType1, argType2, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2); \
	registerMethod2Return<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD3_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3); \
	registerMethod3Return<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD4_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4); \
	registerMethod4Return<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD5_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5); \
	registerMethod5Return<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define INTERFACE_METHOD6_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc ) \
	typedef  returnType ( classType::* _T_##classType##methType )(argType1, argType2, argType3, argType4, argType5, argType6); \
	registerMethod6Return<classType,_T_##classType##methType##>( NULL, tmpInterfaceName, methodName, methodPtr, argTypeDesc, true ); \
	  \

#define END_CLASSINFO( ClassType ) \
  			} \
		}; \
		 \
		 \
		virtual ~ClassType##Info(){}; \
     \
	}; \
	\



#define END_INTERFACEINFO(InterfaceType)\
			}\
		};\
		virtual ~InterfaceType##Info(){};\
	};\
	\


#define REGISTER_CLASSINFO( ClassType ) \
	ClassType::ClassType##Info classInfoFor##ClassType; \
		                                           \

#define REGISTER_INTERFACEINFO(InterfaceType)\
	InterfaceType::InterfaceType##Info interfaceInfoFor##InterfaceType;\
	\


#else


#define BEGIN_CLASSINFO( ClassType, ClassName, superClassName, classID )

#define BEGIN_ABSTRACT_CLASSINFO( ClassType, ClassName, superClassName, classID )

#define BEGIN_INTERFACEINFO(InterfaceType,InterfaceName,SuperInterfaceName,InterfaceID)

#define IMPLEMENTS_INTERFACE( ClassType, InterfaceType, implementationClassName, interfaceName, interfaceID, superInterfaceClassName )

#define EVENT(handlerClassName,eventClassName,eventMethod)

#define PROPERTY( type, propName, getFunc, setFunc, propType )

#define READONLY_PROPERTY( type, propName, getFunc, propType ) 

#define OBJECT_PROPERTY( type, propName, getFunc, setFunc )

#define READONLY_OBJECT_PROPERTY( type, propName, getFunc ) 

#define OBJECT_PROPERTY_REF( type, propName, getFunc, setFunc ) 

#define READONLY_OBJECT_PROPERTY_REF( type, propName, getFunc ) 

#define ENUM_PROPERTY(type,propName,getFunc,setFunc, lower, upper)

#define LABELED_ENUM_PROPERTY(type,propName,getFunc,setFunc, lower, upper, count, enumNames)

#define READONLY_ENUM_PROPERTY(type,propName,getFunc,lower, upper)

#define READONLY_LABELED_ENUM_PROPERTY(type,propName,getFunc,lower, upper, count, enumNames)

#define COLLECTION_PROPERTY( type,propName,getFunc, propType )

#define OBJECT_COLLECTION_PROPERTY( type,propName,getFunc, addFunc, insertFunc, deleteFunc1, deleteFunc2 )

#define METHOD_VOID( methodName, classType, methType, methodPtr )

#define METHOD_1VOID( methodName, classType, methType, argType, methodPtr, argTypeDesc )

#define METHOD_2VOID( methodName, classType, methType, argType1, argType2, methodPtr, argTypeDesc )

#define METHOD_3VOID( methodName, classType, methType, argType1, argType2, argType3, methodPtr, argTypeDesc )

#define METHOD_4VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc )

#define METHOD_5VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc )

#define METHOD_6VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc )

#define METHOD_RETURN( methodName, classType, methType, returnType, methodPtr )

#define METHOD_1RETURN( methodName, classType, methType, returnType, argType, methodPtr, argTypeDesc )

#define METHOD_2RETURN( methodName, classType, methType, returnType, argType1, argType2, methodPtr, argTypeDesc )

#define METHOD_3RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, methodPtr, argTypeDesc )

#define METHOD_4RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc )

#define METHOD_5RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc )

#define METHOD_6RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc )

#define INTERFACE_METHOD_VOID( methodName, classType, methType, methodPtr )

#define INTERFACE_METHOD1_VOID( methodName, classType, methType, argType, methodPtr, argTypeDesc )

#define INTERFACE_METHOD2_VOID( methodName, classType, methType, argType1, argType2, methodPtr, argTypeDesc )

#define INTERFACE_METHOD3_VOID( methodName, classType, methType, argType1, argType2, argType3, methodPtr, argTypeDesc )

#define INTERFACE_METHOD4_VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc )

#define INTERFACE_METHOD5_VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc )

#define INTERFACE_METHOD6_VOID( methodName, classType, methType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc )

#define INTERFACE_METHOD_RETURN( methodName, classType, methType, returnType, methodPtr )

#define INTERFACE_METHOD1_RETURN( methodName, classType, methType, returnType, argType, methodPtr, argTypeDesc )

#define INTERFACE_METHOD2_RETURN( methodName, classType, methType, returnType, argType1, argType2, methodPtr, argTypeDesc )

#define INTERFACE_METHOD3_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, methodPtr, argTypeDesc )

#define INTERFACE_METHOD4_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, methodPtr, argTypeDesc )

#define INTERFACE_METHOD5_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, methodPtr, argTypeDesc )

#define INTERFACE_METHOD6_RETURN( methodName, classType, methType, returnType, argType1, argType2, argType3, argType4, argType5, argType6, methodPtr, argTypeDesc )


#define END_CLASSINFO( ClassType )

#define REGISTER_CLASSINFO( ClassType ) 

#define REGISTER_INTERFACEINFO(InterfaceType)

#define END_INTERFACEINFO(InterfaceType)

#endif //VCF_RTTI


};

#endif

