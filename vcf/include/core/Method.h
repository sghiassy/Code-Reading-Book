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

#ifndef _METHOD_H__
#define _METHOD_H__


namespace VCF {


/**
*Base class for dynamic method invocation
*Methods are dynamically invokable and can support
*querying for argument types
*querying is done throught the getArgumentType() method
*and will only work if the m_argTypes string has bee set appropriately.
*This string consists of a series of characters each of which
*are interpreted as a special code for the primitive type.
*The code table is as follows:
<p>
<pre>
	  code	|	primitive type
	----------------------------	
	  "i"   |		integer
	  "l"	|		long
	  "h"	|		short
	  "+l"	|		unsigned long
	  "f"	|		float
	  "c"	|		char
	  "d"	|		double
	  "o"	|		Object*
	  "b"	|		bool
	  "s"	|		String or String&
	  "e"	|		Enum* 

</pre>
<p>
*To create an instance of a Method object use one of the template 
*based classes described below.
*/
class FRAMEWORK_API Method : public Object{

public:
	Method(){
		m_argCount = 0;	
		m_hasReturnValue = false;
	};

	virtual ~Method(){};

	/**
	*makes a copy of the method. Implemented in the templated
	*derived classes.
	*/
	virtual void copy( Method** copiedMethod ) = 0;

	/**
	*invoke the method on the source object
	*We don't need the number of elements in the arguments[] because 
	*m_argCount should be set and we can use these. If this is not the 
	*case then the programmer has screwed up somewhere !
	*the source argument may be NULL, in this case the 
	*implementing code should Assumes setSource() has already been called
	*this would have gotten set when the class or interface 
	*the method belonged to was retreived from the Object in question
	*/
	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ) = 0;
	
	/**
	*returns the name of the method
	*/
	String getName(){
		return m_name;
	}
	
	/**
	*sets the name of the method
	*/
	void setName( const String& name ){
		m_name = name;
	}

	/**
	*returns the numbers of arguments required to invoke
	*the method.
	*/
	ulong32 getArgCount(){
		return m_argCount;
	}

	/**
	*specifies whether or not the method has a return 
	*value
	*@return bool true if the method has a return value
	*otherwise false
	*/
	bool hasReturnValue(){
		return m_hasReturnValue;
	}

	/**
	*returns the bound source of the method - may return NULL if
	*no source has been set
	*/
	virtual Object* getSource() = 0;

	/**
	*sets the source of the object
	*/
	virtual void setSource( Object* source ) = 0;

	/**
	*returns the argument type for the specified argument
	*@param ulong32 a zero based index representing a particular 
	*argument. For a method declared like this void someMethod( bool, int )
	*then an index of 0 would represent the frist argument (bool), and 
	*index 1 would represnt the second argument(int).
	*@return PropertyDescriptorType the argument type
	*/
	PropertyDescriptorType getArgumentType( const ulong32& argumentIndex ){
		PropertyDescriptorType result = PROP_UNDEFINED;
		VCFChar ch = m_argTypes[argumentIndex];

		switch ( ch ){
			case 'i' :{
				result = PROP_INT;
			}
			break;

			case 'l' :{
				result = PROP_LONG;
			}
			break;

			case 'h' :{
				result = PROP_SHORT;
			}
			break;

			case '+' :{
				VCFChar next = m_argTypes[argumentIndex+1];
				if ( 'l' == next ){
					result = PROP_ULONG;
				}
			}
			break;

			case 'f' :{
				result = PROP_FLOAT;
			}
			break;

			case 'c' :{
				result = PROP_CHAR;
			}
			break;

			case 'd' :{
				result = PROP_DOUBLE;
			}
			break;

			case 'o' :{
				result = PROP_OBJECT;
			}
			break;

			case 'b' :{
				result = PROP_BOOL;
			}
			break;

			case 'e' :{
				result = PROP_ENUM;
			}
			break;

			case 's' :{
				result = PROP_STRING;
			}
			break;
		}
		return result;
	}
protected:
	String m_argTypes;
	ulong32 m_argCount;
	bool m_hasReturnValue;
	String m_name;	
};



/**
*Base template class for methods that do NOT return values
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethod : public Method {
public:
	TypedMethod( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ){
		m_methodPtr = methodPtr;
		m_source = source;
		m_argTypes = argTypes;
		m_objSource = NULL;
	}


	TypedMethod( const TypedMethod& method ) {
		
	}

	virtual ~TypedMethod(){	}

	virtual Object* getSource(){
		return m_objSource;
	}

	virtual void setSource( Object* source ){
		//does the source actually support this interface/class ?
		m_source = dynamic_cast<SOURCE_TYPE*>( source );
		m_objSource = NULL;
		if ( NULL != m_source ){
			m_objSource = source;
		}
	}
	
protected:
	SOURCE_TYPE* m_source;
	Object* m_objSource;
	METHOD_TYPE m_methodPtr;
};


/**
*Base template class for methodsthat DO return values
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodReturn : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodReturn( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
		TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ) {
		m_hasReturnValue = true;
	}

	virtual ~TypedMethodReturn(){}	

protected:
	VariantData m_returnVal;
};


/**
*Method template class for methods have 0 arguments
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg0 : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg0( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 0;
	}

	TypedMethodArg0( const TypedMethodArg0& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual ~TypedMethodArg0(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}		
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 0 == m_argCount ) {
				(m_source->*m_methodPtr)();
			}
		}

		return NULL;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg0<SOURCE_TYPE,METHOD_TYPE>( *this );
	}
};

/**
*Method template class for methods have 1 argument
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg1 : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg1( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 1;
	}

	TypedMethodArg1( const TypedMethodArg1& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg1<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg1(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 1 == m_argCount ) {
				(m_source->*m_methodPtr)(*(arguments[0]));
			}
		}

		return NULL;
	}
};


/**
*Accepts methds with 2 arguments - no return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg2 : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg2( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 2;
	}

	TypedMethodArg2( const TypedMethodArg2& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg2<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg2(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 2 == m_argCount ) {
				(m_source->*m_methodPtr)(	*(arguments[0]),
											*(arguments[1])		);
			}
		}

		return NULL;
	}
};


/**
*Accepts methds with 3 arguments - no return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg3 : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg3( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 3;
	}

	TypedMethodArg3( const TypedMethodArg3& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg3<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg3(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 3 == m_argCount ) {
				(m_source->*m_methodPtr)(	*(arguments[0]),
											*(arguments[1]),
											*(arguments[2])		);
			}
		}

		return NULL;
	}
};



/**
*Accepts methds with 4 arguments - no return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg4 : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg4( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 4;
	}

	TypedMethodArg4( const TypedMethodArg4& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg4<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg4(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 4 == m_argCount ) {
				(m_source->*m_methodPtr)(	*(arguments[0]),
											*(arguments[1]),
											*(arguments[2]),
											*(arguments[3])		);
			}
		}

		return NULL;
	}
};


/**
*Accepts methds with 5 arguments - no return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg5 : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg5( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 5;
	}

	TypedMethodArg5( const TypedMethodArg5& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg5<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg5(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 5 == m_argCount ) {
				(m_source->*m_methodPtr)(	*(arguments[0]),
											*(arguments[1]),
											*(arguments[2]),
											*(arguments[3]),
											*(arguments[4])		);
			}
		}

		return NULL;
	}
};


/**
*Accepts methds with 6 arguments - no return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg6 : public TypedMethod<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg6( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethod<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 6;
	}

	TypedMethodArg6( const TypedMethodArg6& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg6<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg6(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 6 == m_argCount ) {
				(m_source->*m_methodPtr)(	*(arguments[0]),
											*(arguments[1]),
											*(arguments[2]),
											*(arguments[3]),
											*(arguments[4]),
											*(arguments[5])		);
			}
		}

		return NULL;
	}
};


/**
*Method template for methods with 1 argument and a return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg0Return : public TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg0Return( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 0;
	}

	TypedMethodArg0Return( const TypedMethodArg0Return& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg0Return<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg0Return(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 0 == m_argCount ) {
				m_returnVal = (m_source->*m_methodPtr)();
			}
		}

		return &m_returnVal;
	}
};


/**
*Method template for methods with 1 argument and a return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg1Return : public TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg1Return( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 1;
	}
	
	TypedMethodArg1Return( const TypedMethodArg1Return& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg1Return<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg1Return(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 1 == m_argCount ) {
				m_returnVal = (m_source->*m_methodPtr)(*(arguments[0]));
			}
		}

		return &m_returnVal;
	}
};


/**
*Method template for methods with 2 arguments and a return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg2Return : public TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg2Return( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 2;
	}

	TypedMethodArg2Return( const TypedMethodArg2Return& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg2Return<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg2Return(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 2 == m_argCount ) {
				m_returnVal = (m_source->*m_methodPtr)(	*(arguments[0]),
														*(arguments[1]) );
			}
		}

		return &m_returnVal;
	}
};


/**
*Method template for methods with 3 arguments and a return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg3Return : public TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg3Return( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 3;
	}

	TypedMethodArg3Return( const TypedMethodArg3Return& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg3Return<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg3Return(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 3 == m_argCount ) {
				m_returnVal = (m_source->*m_methodPtr)(	*(arguments[0]),
														*(arguments[1]),
														*(arguments[2]) );
			}
		}

		return &m_returnVal;
	}
};


/**
*Method template for methods with 4 arguments and a return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg4Return : public TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg4Return( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 4;
	}

	TypedMethodArg4Return( const TypedMethodArg4Return& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg4Return<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg4Return(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 4 == m_argCount ) {
				m_returnVal = (m_source->*m_methodPtr)(	*(arguments[0]),
														*(arguments[1]),
														*(arguments[2]),
														*(arguments[3]) );
			}
		}

		return &m_returnVal;
	}
};


/**
*Method template for methods with 5 arguments and a return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg5Return : public TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg5Return( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 5;
	}

	TypedMethodArg5Return( const TypedMethodArg5Return& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg5Return<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg5Return(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 5 == m_argCount ) {
				m_returnVal = (m_source->*m_methodPtr)(	*(arguments[0]),
														*(arguments[1]),
														*(arguments[2]),
														*(arguments[3]),
														*(arguments[4]) );
			}
		}

		return &m_returnVal;
	}
};



/**
*Method template for methods with 6 arguments and a return value
*/
template <class SOURCE_TYPE, class METHOD_TYPE> class TypedMethodArg6Return : public TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE> {
public:
	TypedMethodArg6Return( METHOD_TYPE methodPtr=NULL, const String& argTypes="", SOURCE_TYPE* source=NULL ):
	  TypedMethodReturn<SOURCE_TYPE,METHOD_TYPE>( methodPtr, argTypes, source ){
		m_argCount = 6;
	}

	TypedMethodArg6Return( const TypedMethodArg6Return& method )  {
		m_methodPtr = method.m_methodPtr;
		m_source = method.m_source;
		m_objSource = method.m_objSource;
		m_argTypes = method.m_argTypes;
		m_argCount = method.m_argCount;
		m_name = method.m_name;
	}

	virtual void copy( Method** copiedMethod ) {
		*copiedMethod = new TypedMethodArg6Return<SOURCE_TYPE,METHOD_TYPE>( *this );
	}

	virtual ~TypedMethodArg6Return(){	}

	virtual VariantData* invoke( VariantData** arguments, 
								 Object* source=NULL ){

		if ( NULL != source ) {
			m_source = dynamic_cast<SOURCE_TYPE*>( source );	
		}	
		
		if ( (NULL != m_source) && (NULL != m_methodPtr) ){
			if  ( 6 == m_argCount ) {
				m_returnVal = (m_source->*m_methodPtr)(	*(arguments[0]),
														*(arguments[1]),
														*(arguments[2]),
														*(arguments[3]),
														*(arguments[4]),
														*(arguments[5]) );
			}
		}

		return &m_returnVal;
	}
};


}; //end of namespace VCF

#endif //_METHOD_H__