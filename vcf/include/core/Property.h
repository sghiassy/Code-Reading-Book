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

//Property.h

#ifndef _PROPERTY_H__
#define _PROPERTY_H__



namespace VCF
{

#define DEFAULT_PROP_VAL_STR		50


static int prop_count;

/**
*VariantData represents an unknown type of variable - similiar to 
*Visual Basic's Variant object. 
*VariantData objects can store int, long, double, bool String, or Object*.
*VariantData overrides conversion operators and provides operator equals overloads 
*to handle the conversion from one type to another. Setting the value of a VariantData
*automatically sets the type. For example:
*<pre>
*	VariantData val;
*	<b>int</b> i = 123;
*	val = i; <i>//automatically sets the VariantData::type to equal PROP_INT</i>
*</pre>
*To convert from one type to another is equally simple:
*<pre>
*	VariantData val;
*	<b>int</b> i;
*	<b>double</b> d;
*	val = 234.8546; <i>//val is 234.8546 and is set to the correct type</i>
*	d = val; <i>//val is converted to a double and d is assigned the value 234.8546</i>
*	val = 123; <i>//val is now assigned an int (or whatever the compiler assumes 123 is )</i>
*	i = val;	<i>//i is now equal to 123</i>
*</pre>
*Strings are a special case, since the compiler will not allow a union to be made with them, 
*thus the reason for the StringVal member outside of the union.
*VariantData objects can also have their data render as a string and can be assigned strings
*and convert then to the correct data value.
*
*@version 1.0
*@author Jim Crafton
*/
class FRAMEWORK_API VariantData : public Object {		
public:
	/**
	*creates an empty, undefined object
	*/
	VariantData(){
		ObjVal = NULL;
		this->type = PROP_UNDEFINED;
	};

	/**
	*creates a VariantData initialized by an int value
	*/
	VariantData( const int& val ) {
		IntVal = val;
		type = PROP_INT;
	};

	/**
	*creates a VariantData initialized by a long value
	*/
	VariantData( const long& val ) {
		LongVal = val;
		type = PROP_LONG;
	};

	/**
	*creates a VariantData initialized by an unsigned long value
	*/
	VariantData( const unsigned long& val ) {
		ULongVal = val;
		type = PROP_ULONG;
	};

	/**
	*creates a VariantData initialized by a float value
	*/
	VariantData( const float& val ) {
		FloatVal = val;
		type = PROP_FLOAT;
	};

	/**
	*creates a VariantData initialized by a double value
	*/
	VariantData( const double& val ) {
		DblVal = val;
		type = PROP_DOUBLE;
	};

	/**
	*creates a VariantData initialized by a char value
	*/
	VariantData( const char& val ) {
		CharVal = val;
		type = PROP_CHAR;
	};

	/**
	*creates a VariantData initialized by a bool value
	*/
	VariantData( const bool& val ) {
		BoolVal = val;
		type = PROP_BOOL;
	};

	/**
	*creates a VariantData initialized by a short value
	*/
	VariantData( const short& val ) {
		ShortVal = val;
		type = PROP_SHORT;
	};

	/**
	*creates a VariantData initialized by a String value
	*/
	VariantData( const String& val ) {
		StringVal = val;
		type = PROP_STRING;
	};

	/**
	*creates a VariantData initialized by a Enum value
	*/
	VariantData( const Enum& val ) {
		EnumVal.set( const_cast<Enum*>(&val) );
		type = PROP_ENUM;
	};

	/**
	*creates a VariantData initialized by a Enum value
	*/
	VariantData( Enum& val ) {
		EnumVal.set( &val );
		type = PROP_ENUM;
	};

	/**
	*creates a VariantData initialized by a Enum* value
	*/
	VariantData( Enum* val ) {
		EnumVal.set( val );
		type = PROP_ENUM;
	};

	/**
	*creates a VariantData initialized by a Object* value
	*/
	VariantData( Object* val ) {
		ObjVal = val;
		type = PROP_OBJECT;
	}

	/**
	*creates a VariantData initialized by a Object& value
	*/
	VariantData( Object& val ) {
		ObjVal = &val;
		type = PROP_OBJECT;
	}

	/**
	*creates a VariantData initialized by a const Object& value
	*/
	VariantData( const Object& val ){
		ObjVal = const_cast<Object*>(&val);
		type = PROP_OBJECT;
	}

	/**
	*creates a VariantData initialized by a Interface* value
	*/
	VariantData( Interface* val ){
		InterfaceVal = val;
		type = PROP_INTERFACE;
	}

	/**
	*copy constructor
	*/
	VariantData( const VariantData& value ){
		this->type = value.type;
		this->setValue( value );
	};

	virtual ~VariantData(){};

	/**
	*defines the data type of the VariantData, where type can represent
	*an int, long, unsigned long, short, char, double, float, bool, string,
	*Enum pointer, or Object pointer.
	*/
	PropertyDescriptorType type;

	union{
			int IntVal;
			long LongVal;
			short ShortVal;
			unsigned long ULongVal;
			float FloatVal;
			char CharVal;
			double DblVal;
			bool BoolVal;
			Object* ObjVal;
			EnumValue EnumVal;
			Interface* InterfaceVal;
	};

	/**
	*string are a special case, not allowed in unions
	*/
	String StringVal;

	/**
	*converts the VariantData to an int
	*/
	operator int (){
		return IntVal;
	};	
	
	/**
	*converts the VariantData to an long
	*/
	operator long (){
		return LongVal;
	};
	
	/**
	*converts the VariantData to an short
	*/
	operator short (){
		return ShortVal;
	};

	/**
	*converts the VariantData to an unsigned long
	*/
	operator unsigned long (){
		return ULongVal;
	};

	/**
	*converts the VariantData to an float
	*/
	operator float (){
		return FloatVal;
	};

	/**
	*converts the VariantData to an char
	*/
	operator char (){
		return CharVal;
	};

	/**
	*converts the VariantData to an double
	*/
	operator double (){
		return DblVal;
	};
	
	/**
	*converts the VariantData to an Interface pointer
	*/
	operator Interface* (){
		return InterfaceVal;
	}

	/**
	*converts the VariantData to an Object pointer
	*/
	operator Object* (){
		return ObjVal;
	};

	/**
	*converts the VariantData to an Object reference
	*/
	operator Object& (){
		return *ObjVal;
	};

	/**
	*converts the VariantData to an String
	*/
	operator String (){
		return StringVal;
	};

	/**
	*converts the VariantData to a bool
	*/
	operator bool (){
		return BoolVal;
	};

	/**
	*converts the VariantData to an Enum pointer
	*/
	operator Enum* (){
		return EnumVal.getEnum();
	};

	/**
	*converts the VariantData to an Enum reference
	*/
	operator Enum& (){
		return *EnumVal.getEnum();
	};

	/**
	*Assigns an int value to the VariantData
	*/
	VariantData& operator=( const int& newValue ){
		IntVal = newValue;
		type = PROP_INT;
		return *this;
	};
	
	/**
	*Assigns an long value to the VariantData
	*/
	VariantData& operator=( const long& newValue ){
		LongVal = newValue;
		type = PROP_LONG;
		return *this;
	};
	
	/**
	*Assigns an short value to the VariantData
	*/
	VariantData& operator=( const short& newValue ){
		ShortVal = newValue;
		type = PROP_SHORT;
		return *this;
	};

	/**
	*Assigns an unsigned long value to the VariantData
	*/
	VariantData& operator=( const unsigned long& newValue ){
		ULongVal = newValue;
		type = PROP_ULONG;
		return *this;
	};

	/**
	*Assigns an float value to the VariantData
	*/
	VariantData& operator=( const float& newValue ){
		FloatVal = newValue;
		type = PROP_FLOAT;
		return *this;
	};

	/**
	*Assigns an char value to the VariantData
	*/
	VariantData& operator=( const char& newValue ){
		CharVal = newValue;
		type = PROP_CHAR;
		return *this;
	};

	/**
	*Assigns an double value to the VariantData
	*/
	VariantData& operator=( const double& newValue ){
		DblVal = newValue;
		type = PROP_DOUBLE;
		return *this;
	};

	/**
	*Assigns an bool value to the VariantData
	*/
	VariantData& operator=( const bool& newValue ){
		BoolVal = newValue;
		type = PROP_BOOL;
		return *this;
	};

	/**
	*Assigns an string (as an array of char's) to the VariantData
	*/
	VariantData& operator=( const char* newValue ){
		StringVal = newValue;
		type = PROP_STRING;
		return *this;
	};

	/**
	*Assigns an Interface pointer to the VariantData
	*/
	VariantData& operator=( Interface* newValue ){
		//ObjVal->copy( newValue );
		InterfaceVal = newValue;
		type = PROP_INTERFACE;
		return *this;
	};

	/**
	*Assigns an Object pointer to the VariantData
	*/
	VariantData& operator=( Object* newValue ){
		//ObjVal->copy( newValue );
		ObjVal = newValue;
		type = PROP_OBJECT;
		return *this;
	};

	/**
	*Assigns an Object reference to the VariantData
	*/
	VariantData& operator=( const Object& newValue ){
		//ObjVal->copy( newValue );
		ObjVal = const_cast<Object*>(&newValue);
		type = PROP_OBJECT;
		return *this;
	};

	/**
	*Assigns an string (as an String) to the VariantData
	*/
	VariantData& operator=( const String& newValue ){
		StringVal = newValue;
		type = PROP_STRING;
		return *this;
	};

	/**
	*Assigns an Enum pointer to the VariantData
	*/
	VariantData& operator=( Enum* newValue ){
		EnumVal.set( newValue );
		type = PROP_ENUM;
		return *this;
	};

	/**
	*Assigns an Enum reference to the VariantData
	*/
	VariantData& operator=( const Enum& newValue ){
		EnumVal.set( const_cast<Enum*>(&newValue) );
		type = PROP_ENUM;
		return *this;
	};	

	VariantData& operator= ( const VariantData& newValue ) {
		setValue( newValue );
		return *this;
	}

	VariantData& operator= ( VariantData* newValue ) {
		setValue( *newValue );
		return *this;
	}

	/**
	*converts the VariantData to a string, no matter 
	*what the type. Object* are persisted to a TextOutputStream
	*if they support Persistable, otherwise the Object's toString() 
	*method is invoked.
	*/
	virtual String toString(){
		String result = "";
		switch ( type ) {
			case PROP_INT:{
				int i = *this;
				char tmp[DEFAULT_PROP_VAL_STR];
				memset(tmp, 0, DEFAULT_PROP_VAL_STR);
				sprintf( tmp, "%d", i );
				result += tmp;
			}
			break;

			case PROP_LONG:{
				long i = *this;
				char tmp[DEFAULT_PROP_VAL_STR];
				memset(tmp, 0, DEFAULT_PROP_VAL_STR);
				sprintf( tmp, "%d", i );
				result += tmp;
			}
			break;

			case PROP_SHORT:{
				short i = *this;
				char tmp[DEFAULT_PROP_VAL_STR];
				memset(tmp, 0, DEFAULT_PROP_VAL_STR);
				sprintf( tmp, "%d", i );
				result += tmp;
			}
			break;

			case PROP_ULONG:{
				unsigned long i = *this;
				char tmp[DEFAULT_PROP_VAL_STR];
				memset(tmp, 0, DEFAULT_PROP_VAL_STR);
				sprintf( tmp, "%d", i );
				result += tmp;
			}
			break;

			case PROP_FLOAT:{
				float i = *this;
				char tmp[DEFAULT_PROP_VAL_STR];
				memset(tmp, 0, DEFAULT_PROP_VAL_STR);
				sprintf( tmp, "%.5f", i );
				result += tmp;
			}
			break;

			case PROP_CHAR:{
				char i = *this;
				char tmp[DEFAULT_PROP_VAL_STR];
				memset(tmp, 0, DEFAULT_PROP_VAL_STR);
				sprintf( tmp, "%c", i );
				result += tmp;
			}
			break;

			case PROP_DOUBLE:{
				double i = *this;
				char tmp[DEFAULT_PROP_VAL_STR];
				memset(tmp, 0, DEFAULT_PROP_VAL_STR);
				sprintf( tmp, "%.5f", i );
				result += tmp;
			}
			break;

			case PROP_BOOL:{
				bool i = *this;								
				result += i ? "true":"false";
			}
			break;

			case PROP_OBJECT:{				
				Object* object = *this;
				if ( NULL != object ){
					//result += object->toString();
					Persistable* persist = dynamic_cast<Persistable*>(object);
					if ( NULL != persist ){
						TextOutputStream tos;

						persist->saveToStream( &tos );

						result += tos.getTextBuffer();
					}
					else {
						result += object->toString();
					}
				}
				else{
					result += "null";
				}
			}
			break;

			case PROP_INTERFACE:{				
				Interface* intrFace = *this;
				if ( NULL != intrFace ){
					Persistable* persist = dynamic_cast<Persistable*>(intrFace);
					if ( NULL != persist ){
						TextOutputStream tos;

						persist->saveToStream( &tos );

						result += tos.getTextBuffer();
					}
					else {
						result += "null";
					}
				}
				else{
					result += "null";
				}
			}
			break;

			case PROP_STRING:{
				result = StringVal;
			}
			break;

			case PROP_ENUM:{
				Enum* enumObj = *this;
				if ( NULL != enumObj ){
					result += enumObj->toString();
				}
				else{
					result += "null";
				}
			}
			break;
		}
		return result;
	};

	/**
	*Assigns the VariantData's data from a string. The conversion
	*process is dependent on the type of the VariantData.
	*/
	void setFromString( const String& value ){
		switch( type ){
			case PROP_INT:{
				int i = 0;
				sscanf( value.c_str(), STR_INT_CONVERSION, &i );
				IntVal = i;
			}
			break;

			case PROP_LONG:{
				int i = 0;
				sscanf( value.c_str(), STR_LONG_CONVERSION, &i );
				LongVal = i;
			}
			break;

			case PROP_SHORT:{
				int i = 0;
				sscanf( value.c_str(), STR_SHORT_CONVERSION, &i );
				ShortVal = i;
			}
			break;

			case PROP_ULONG:{
				int i = 0;
				sscanf( value.c_str(), STR_ULONG_CONVERSION, &i );
				ULongVal = i;
			}
			break;

			case PROP_FLOAT:{
				float i = 0.0f;
				sscanf( value.c_str(), STR_FLOAT_CONVERSION, &i );
				FloatVal = i;
			}
			break;

			case PROP_CHAR:{
				char i = '\0';
				sscanf( value.c_str(), STR_CHAR_CONVERSION, &i );
				CharVal = i;
			}
			break;

			case PROP_DOUBLE:{
				float f = 0;
				sscanf( value.c_str(), STR_DOUBLE_CONVERSION, &f );
				DblVal = f;
			}
			break;

			case PROP_BOOL:{
				if ( STR_BOOL_CONVERSION_TRUE == value ){
					BoolVal = true;
				}
				else if ( STR_BOOL_CONVERSION_FALSE == value ){
					BoolVal = false;
				}
			}
			break;

			case PROP_OBJECT:{				
				if ( NULL != ObjVal ){
					Persistable* persist = dynamic_cast<Persistable*>(ObjVal);
					if ( NULL != persist ){
						
						BasicInputStream bis( value );
						TextInputStream tis( &bis );
						persist->loadFromStream( &tis );
					}
				}				
			}
			break;

			case PROP_INTERFACE:{				
				if ( NULL != InterfaceVal ){
					Persistable* persist = dynamic_cast<Persistable*>(InterfaceVal);
					if ( NULL != persist ){
						
						BasicInputStream bis( value );
						TextInputStream tis( &bis );
						persist->loadFromStream( &tis );
					}
				}				
			}
			break;

			case PROP_STRING:{
				StringVal = value;
			}
			break;

			case PROP_ENUM:{
				if ( NULL != EnumVal.getEnum() ){
					EnumVal.set( value );
				}
			}
			break;
		}
	};

protected:
	void setValue( const VariantData& value ){
		type = value.type;
		switch( type ){
			case PROP_INT:{
				IntVal = value.IntVal;
			}
			break;

			case PROP_LONG:{
				LongVal = value.LongVal;
			}
			break;

			case PROP_DOUBLE:{
				DblVal = value.DblVal;
			}
			break;

			case PROP_BOOL:{
				BoolVal = value.BoolVal;
			}
			break;

			case PROP_OBJECT:{
				ObjVal = value.ObjVal;
			}
			break;

			case PROP_INTERFACE:{
				InterfaceVal = value.InterfaceVal;
			}
			break;
			
			case PROP_STRING:{
				StringVal = value.StringVal;
			}
			break;

			case PROP_ENUM:{
				EnumVal = value.EnumVal;
			}
			break;
		}
	}
};

/**
*The Property class represents a member attribute of a class. Properties allow a 
*programmer to dynamically query a class at runtime for all of it attributes
*with out knowing the exact type of class the object instance is. Property is 
*an abstract base class, and only provides an interface to work with. 
*
*@author Jim Crafton
*@version 1.0
*/
class FRAMEWORK_API Property : public Object {
public:
	Property(){
		m_source = NULL;
		m_isCollection = false;
		m_isReadOnly = false;
		m_bound = false;
		//prop_count ++;
		//StringUtils::trace( "Created Property\n\tProperty Count = " + StringUtils::toString(prop_count) + "\n" );
	};
	
	Property( const Property& prop ):
		m_value( prop.m_value )	{

		m_name = prop.m_name;
		m_displayName = prop.m_displayName;
		m_description = prop.m_description;
		m_type = prop.m_type;
		m_source = prop.m_source;
		m_isReadOnly = prop.m_isReadOnly;
		m_bound = prop.m_bound;
		
		//prop_count ++;
		//StringUtils::trace( "Created duplicate Property\n\tProperty Count = " + StringUtils::toString(prop_count) + "\n" );
	};

	virtual ~Property(){
		//prop_count --;
		//StringUtils::trace( "Deleted Property\n\tProperty Count = " + StringUtils::toString(prop_count) + "\n" );
	};
	
	/**
	*makes a copy of the property. Implemented in the templated
	*derived classes.
	*/
	virtual void copy( Property** copiedProperty ) = 0;

	/**
	*gets the name of the property.
	*/
	String getName() const{
		return m_name;
	};

	/**
	*returns the display name, usually something longer and more descriptive 
	*than the name alone.
	*/
	String getDisplayName() const{
		if ( m_displayName.size() > 0 ) {
			return m_displayName;
		}else
			return m_name;
	};
	
	/**
	*gets a description of the property,
	*telling what the property does or represents.
	*/
	String getDescription() const{
		return m_description;
	};
	
	/**
	*sets the description of the property
	*/
	void setDescription( const String& description ){
		m_description = description;
	};

	/**
	*sets the display name of the property
	*/
	void setDisplayName( const String& displayName ){
		m_displayName = displayName;
	}

	/**
	*sets the name of the property
	*/
	void setName( const String& name ){
		m_name = name;
	};

	/**
	*This function returns the class name of the property's type.
	*For primitive types it will return the primitive type name, i.e. for
	*a property of type long it will return "long", while a property
	*of type Color will return "VCF::Color".
	*@return String the name of the class that the property represents
	*/
	virtual String getTypeClassName() = 0;
	/**
	*returns the type of the Property. These are the same types as VariantData::type.
	*They can be int, long, unsigned long, short, float, double, char, bool,
	*string, enums or Object*.
	*/
	PropertyDescriptorType getType() const{
		return m_type;
	};

	/**
	*sets the type of the Property
	*/
	void setType( const PropertyDescriptorType& type ){
		m_type = type;
		m_value.type = m_type;
	};	

	/**
	*gets the value of the property for the given instance of source.
	*@param Object the object to invoke the get method for that 
	*particular property.
	*@return VariantData the value of the given property for that object (source)
	*/
	virtual VariantData* get( Object* source ) = 0;

	/**
	*sets a value on an object.
	*/
	virtual void set( Object* source, VariantData* value ) = 0;
	
	/**
	*sets the source for the Property instance.
	*@param Object the source object the property instance is 
	*associated with. get() and set() calls will be performed
	*on this source object. 
	*/
	void setSource( Object* source ){
		m_source = source;
		get();
	};

	/**
	*returns the object source that is associated with this property
	*instance.
	*/
	Object* getSource(){
		return m_source;
	}
	
	/**
	*returns a VariandData object that represents the value of this 
	*property on the source object that was specified in the setSource()
	*method.
	*/
	VariantData* get(){
		if ( NULL != m_source ){
			return get( m_source );
		}
		else {
			return NULL;
		}
	};

	/**
	*sets the value specified in the VariantData object passed in 
	*on the source obejct that was specified in the setSource()
	*method.
	*/
	void set( VariantData* value ){
		if ( NULL != m_source ){
			set( m_source, value );
		}
	};

	/**
	*calls the m_value's setFromString method which converts the 
	*string into the right type and then calls set()
	*/
	void set( Object* source, const String& value ){
		m_value.setFromString( value );
		set( source, &m_value );
	};

	/**
	*convenience method for calling set on propertys that have their 
	*source assigned to a valid pointer.
	*/
	void set( const String& value ){
		if ( NULL != m_source ){
			set( m_source, value );
		}
	};

	virtual String toString(){
		String result = "<property name=\"" + this->getName() + "\" value=\"";		
		VariantData* val = this->get();
		if ( NULL != val ){
			result += val->toString() + "\" />";
		}
		else{
			result += "null\" />";
		}		
		return result;
	};

	/**
	*returns whether or not the property represents a collection
	*if the result is true, then it is safe to call the hasMoreElements() and
	*nextElement() methods.
	*/
	bool isCollection(){
		return m_isCollection;
	};

	/**
	*returns whether or not the property collection has more elements
	*/
	virtual bool hasMoreElements( Object* source ){
		return false;	
	};

	/**
	*returns the next element in the collection, currently only forward 
	*iterating. Also no support for getting an item from an index
	*/
	virtual VariantData* nextElement( Object* source ){
		return NULL;	
	}; 

	/**
	*We can't call get() to grab the collection directly, since 
	*an Enumerator is a templated class, and this point we 
	*don't know the template type. So instead, we call startCollection()
	*to invoke the some getXXX function that returns an enumerator.
	*This will mean that derived classes will now have the enumerator
	*and can start calling methods on it. The base class implementation 
	*does nothing.
	*/
	virtual void startCollection( Object* source ){
		//no-op	
	};

	void startCollection(){
		if ( NULL != m_source ){
			startCollection( m_source );
		}
	};

	void add( VariantData* value ){
		if ( NULL != m_source ){
			add( m_source, value );
		}
	};

	virtual void add( Object* source, VariantData* value ) {
		//no-op	
	};

	virtual void insert( Object* source, const unsigned long& index, VariantData* value ){
		//no-op	
	};

	void insert( const unsigned long& index, VariantData* value ){
		if ( NULL != m_source ){
			insert( m_source, index, value );
		}
	};

	virtual void remove( Object* source, VariantData* value ){
		//no-op	
	};

	void remove( VariantData* value ){
		if ( NULL != m_source ){
			remove( m_source, value );
		}
	};

	virtual void remove( Object* source, const unsigned long& index ){
		//no-op	
	}

	void remove( const unsigned long& index ){
		if ( NULL != m_source ){
			remove( m_source, index );
		}
	};

	virtual bool collectionSupportsEditing(){
		return false;
	}

	bool isReadOnly(){
		return m_isReadOnly;
	}

	/**
	*This macro declares a std::vector<> of PropertyChangeHandler handlers
	*/
	EVENT_HANDLER_LIST(PropertyChanged);

	/**
	*This macro creates a method for adding a handler to the Property's Change events
	*/
	ADD_EVENT(PropertyChanged);

	/**
	*This macro creates a method for removing a handler to the Property's Change events
	*/
	REMOVE_EVENT(PropertyChanged);

	FIRE_EVENT(PropertyChanged, PropertyChangeEvent ); 
	
	/**
	*returns whether or not the property is "bound" to an object.
	*"Bound" properties cause a onPropertyChanged() method to 
	*be fired in any listeners whenever the property's set() method
	*is called
	*/
	bool isBound() {
		return m_bound;
	};

	void setBound( const bool& bound ){
		m_bound = bound;
	};

protected:
	VariantData m_value;	
	bool m_isCollection;
	bool m_isReadOnly;
private:
	bool m_bound;
	String m_name;
	String m_displayName;
	String m_description;
	PropertyDescriptorType m_type;
	Object* m_source;
};


#ifdef VCF_RTTI

/**
*Typed properties are designed to only work with Object derived classes.
*Many thanks go to Mario Motta (author VDK, mmotta@guest.net) for inspiring this from his
*VDKReadWriteValueProp class.
*
*@version 1.0
*@author Jim Crafton
*/
template <class PROPERTY> class FRAMEWORK_API TypedProperty : public Property {
public:
	typedef PROPERTY (Object::*GetFunction)(void);
	typedef void (Object::*SetFunction)(const PROPERTY& );

	TypedProperty( GetFunction propGetFunction, const PropertyDescriptorType& propertyType ){
		this->init();
		
		m_getFunction = propGetFunction;
		setType( propertyType );
		m_isReadOnly = true;
	};

	TypedProperty( GetFunction propGetFunction, SetFunction propSetFunction,
		               const PropertyDescriptorType& propertyType ){
		this->init();
		
		m_getFunction = propGetFunction;
		m_setFunction = propSetFunction;
		setType( propertyType );
	};

	TypedProperty( const TypedProperty<PROPERTY>& prop ):
		Property( prop ){

		this->init();
		m_getFunction = prop.m_getFunction;
		m_setFunction = prop.m_setFunction;	
	};

	virtual ~TypedProperty(){};

	virtual String getTypeClassName() {
		return StringUtils::getClassNameFromTypeInfo( typeid(PROPERTY) );
	}

	virtual void copy( Property** copiedProperty ){
		*copiedProperty = NULL;
		*copiedProperty = new TypedProperty<PROPERTY>(*this);
	};

	void init(){
		m_getFunction = NULL;		
		m_setFunction = NULL;		
	};

	virtual VariantData* get( Object* source ){		
		if ( (NULL != m_getFunction) && (NULL != source) ){
			m_value.type = this->getType();
			m_value = (source->*m_getFunction)();		 
			return &m_value;
		}
		else {
			return NULL;
		}
	};

	virtual void set( Object* source, VariantData* value ){
		if ( (NULL != m_setFunction) && (NULL != source) ){
			if ( true == this->isBound() ){
				VariantData* originalValue = this->get( source );
				PropertyChangeEvent changeEvent( source, originalValue, value );
				try {
					fireOnPropertyChanged( &changeEvent );
					(source->*m_setFunction)( *value );
				}
				catch ( PropertyChangeException ){
					//do not call the set method
					//re-throw the exception ?
					
				}				
			}
			else {
				(source->*m_setFunction)( *value );
			}
		}
	};
	
protected:	
	GetFunction m_getFunction;
	SetFunction m_setFunction;
};

template <class PROPERTY> class FRAMEWORK_API TypeDefProperty : public TypedProperty<PROPERTY> {
public:
	TypeDefProperty( GetFunction propGetFunction, const PropertyDescriptorType& propertyType, const String& typeDefClassName ):
		TypedProperty<PROPERTY>( propGetFunction, propertyType ) {
		m_typeDefClassName = typeDefClassName;
	};

	TypeDefProperty( GetFunction propGetFunction, SetFunction propSetFunction,
						const PropertyDescriptorType& propertyType, const String& typeDefClassName ):
							TypedProperty<PROPERTY>( propGetFunction, propSetFunction, propertyType )	{
		
		m_typeDefClassName = typeDefClassName;
	};

	TypeDefProperty( const TypeDefProperty<PROPERTY>& prop ):
		TypedProperty<PROPERTY>( prop ){
		m_typeDefClassName = prop.m_typeDefClassName;
	};

	virtual ~TypeDefProperty(){};

	virtual String getTypeClassName() {
		return m_typeDefClassName;
	}

	virtual void copy( Property** copiedProperty ){
		*copiedProperty = NULL;
		*copiedProperty = new TypeDefProperty<PROPERTY>(*this);
	};

protected:
	String m_typeDefClassName;
};

template <class PROPERTY> class FRAMEWORK_API TypedObjectProperty : public Property {
public:
	typedef PROPERTY* (Object::*GetFunction)(void);
	typedef void (Object::*SetFunction)( PROPERTY* );

	TypedObjectProperty( GetFunction propGetFunction ){
		this->init();
		m_getFunction = propGetFunction;
		setType( PROP_OBJECT );
		m_isReadOnly = true;
	};

	TypedObjectProperty( GetFunction propGetFunction, SetFunction propSetFunction ){
		this->init();
		
		m_getFunction = propGetFunction;
		m_setFunction = propSetFunction;
		setType( PROP_OBJECT );
	};

	TypedObjectProperty( const TypedObjectProperty<PROPERTY>& prop ):
		Property( prop ){

		this->init();
		m_getFunction = prop.m_getFunction;
		m_setFunction = prop.m_setFunction;	
	};

	virtual ~TypedObjectProperty(){};

	virtual String getTypeClassName() {
		return StringUtils::getClassNameFromTypeInfo( typeid(PROPERTY) );
	}

	virtual void copy( Property** copiedProperty ){
		*copiedProperty = NULL;
		*copiedProperty = new TypedObjectProperty<PROPERTY>(*this);
	};

	void init(){
		m_getFunction = NULL;		
		m_setFunction = NULL;		
		setType( PROP_OBJECT );
	};

	virtual VariantData* get( Object* source ){		
		if ( (NULL != m_getFunction) && (NULL != source) ){
			m_value.type = this->getType();
			m_value = (Object*)(source->*m_getFunction)();		 
			return &m_value;
		}
		else {
			return NULL;
		}
	};

	void set( Object* source, VariantData* value ){
		if ( (NULL != m_setFunction) && (NULL != source) ){
			Object* object = *value;
			if ( true == this->isBound() ){
				VariantData* originalValue = this->get( source );
				PropertyChangeEvent changeEvent( source, originalValue, value );
				try {
					fireOnPropertyChanged( &changeEvent );
					(source->*m_setFunction)( (PROPERTY*)(object) );
				}
				catch ( PropertyChangeException ){
					//do not call the set method
					//re-throw the exception ?
				}				
			}
			else {				
				(source->*m_setFunction)( (PROPERTY*)(object) );
			}			
		}
	};

protected:	
	GetFunction m_getFunction;
	SetFunction m_setFunction;
};

template <class PROPERTY> class FRAMEWORK_API TypedObjectRefProperty : public Property {
public:
	typedef PROPERTY& (Object::*GetFunction)(void);
	typedef void (Object::*SetFunction)( const PROPERTY& );

	TypedObjectRefProperty( GetFunction propGetFunction ){
		this->init();
		m_getFunction = propGetFunction;
		setType( PROP_OBJECT );
		m_isReadOnly = true;
	};

	TypedObjectRefProperty( GetFunction propGetFunction, SetFunction propSetFunction ){
		this->init();
		
		m_getFunction = propGetFunction;
		m_setFunction = propSetFunction;
		setType( PROP_OBJECT );
	};

	TypedObjectRefProperty( const TypedObjectProperty<PROPERTY>& prop ):
		Property( prop ){

		this->init();
		m_getFunction = prop.m_getFunction;
		m_setFunction = prop.m_setFunction;	
	};

	virtual ~TypedObjectRefProperty(){};

	virtual String getTypeClassName() {
		return StringUtils::getClassNameFromTypeInfo( typeid(PROPERTY) );
	}

	virtual void copy( Property** copiedProperty ){
		*copiedProperty = NULL;
		*copiedProperty = new TypedObjectRefProperty<PROPERTY>(*this);
	};

	void init(){
		m_getFunction = NULL;		
		m_setFunction = NULL;		
		setType( PROP_OBJECT );
	};

	virtual VariantData* get( Object* source ){		
		if ( (NULL != m_getFunction) && (NULL != source) ){
			m_value.type = this->getType();
			//m_value = (Object&)(source->*m_getFunction)();		 
			m_value = (source->*m_getFunction)();		 
			return &m_value;
		}
		else {
			return NULL;
		}
	};

	void set( Object* source, VariantData* value ){
		if ( (NULL != m_setFunction)  ){
			Object* object = *value;
			if ( true == this->isBound() ){
				VariantData* originalValue = this->get( source );
				PropertyChangeEvent changeEvent( source, originalValue, value );
				try {
					fireOnPropertyChanged( &changeEvent );
					(source->*m_setFunction)( (PROPERTY&)*object );
				}
				catch ( PropertyChangeException ){
					//do not call the set method
					//re-throw the exception ?
				}				
			}
			else {				
				(source->*m_setFunction)( (PROPERTY&)*object );
			}			
		}
	};

protected:	
	GetFunction m_getFunction;
	SetFunction m_setFunction;
};

template <class ENUM_PROPERTY> class FRAMEWORK_API TypedEnumProperty : public Property {
public:
	typedef ENUM_PROPERTY (Object::*GetFunction)(void);
	typedef void (Object::*SetFunction)( const ENUM_PROPERTY& );

	TypedEnumProperty( GetFunction propGetFunction, const ENUM_PROPERTY& lower, const ENUM_PROPERTY& upper ){
		this->init();
		m_getFunction = propGetFunction;
		m_enum = new TypedEnum<ENUM_PROPERTY>(lower,upper);
		m_lower = lower;
		m_upper = upper;
		setType( PROP_ENUM );
		m_isReadOnly = true;
	};

	TypedEnumProperty( GetFunction propGetFunction, SetFunction propSetFunction, const ENUM_PROPERTY& lower, const ENUM_PROPERTY& upper ){
		this->init();
		m_lower = lower;
		m_upper = upper;
		m_getFunction = propGetFunction;
		m_setFunction = propSetFunction;		
		m_enum = new TypedEnum<ENUM_PROPERTY>(lower,upper);		
		setType( PROP_ENUM );
	};

	TypedEnumProperty( GetFunction propGetFunction, SetFunction propSetFunction, 
		               const ENUM_PROPERTY& lower, const ENUM_PROPERTY& upper,
					   const unsigned long& enumNameCount, 
					   String* enumNames ){
		this->init();
		m_lower = lower;
		m_upper = upper;
		m_getFunction = propGetFunction;
		m_setFunction = propSetFunction;
		m_enumNameCount = enumNameCount;
		m_enumNames = enumNames;
		m_enum = new TypedEnum<ENUM_PROPERTY>(lower,upper, enumNameCount, enumNames );		
		setType( PROP_ENUM );
	};

	TypedEnumProperty( const TypedEnumProperty<ENUM_PROPERTY>& prop ){
		//Property( prop ){

		this->init();
		m_getFunction = prop.m_getFunction;
		m_setFunction = prop.m_setFunction;	
		m_enumNameCount = prop.m_enumNameCount;
		m_enumNames = prop.m_enumNames;
		m_lower = prop.m_lower;
		m_upper = prop.m_upper;
		if ( 0 < m_enumNameCount ){
			m_enum = new TypedEnum<ENUM_PROPERTY>( m_lower,m_upper, m_enumNameCount, m_enumNames );
		}
		else{
			m_enum = new TypedEnum<ENUM_PROPERTY>( m_lower,m_upper );
		}		
		setType( PROP_ENUM );
		setName( prop.getName() );
		setDisplayName( prop.getDisplayName() );		
		setDescription( prop.getDescription() );
	};

	virtual ~TypedEnumProperty(){
		delete m_enum;
	};

	virtual String getTypeClassName() {
		return StringUtils::getClassNameFromTypeInfo( typeid(ENUM_PROPERTY) );
	}

	virtual void copy( Property** copiedProperty ){
		*copiedProperty = NULL;
		*copiedProperty = new TypedEnumProperty<ENUM_PROPERTY>(*this);
	};

	void init(){		
		m_getFunction = NULL;		
		m_setFunction = NULL;		
		setType( PROP_ENUM );
		m_enumNameCount = 0;
	};

	virtual VariantData* get( Object* source ){
		if ( (NULL != m_getFunction) && (NULL != source) ){
			m_value.type = this->getType();
			m_enum->set( (source->*m_getFunction)() );
			m_value = m_enum;
			return &m_value;
		}
		else {
			return NULL;
		}
	};

	virtual void set( Object* source, VariantData* value ){
		if ( (NULL != m_setFunction) && (NULL != source) ){
			Enum* e = *value;
			ENUM_PROPERTY enumVal = (ENUM_PROPERTY)e->get();

			if ( true == this->isBound() ){
				VariantData* originalValue = this->get( source );
				PropertyChangeEvent changeEvent( source, originalValue, value );
				try {
					fireOnPropertyChanged( &changeEvent );
					(source->*m_setFunction)( enumVal );
				}
				catch ( PropertyChangeException ){
					//do not call the set method
					//re-throw the exception ?
				}				
			}
			else {				
				(source->*m_setFunction)( enumVal );
			}			
		}
	};

protected:	
	GetFunction m_getFunction;
	SetFunction m_setFunction;
	TypedEnum<ENUM_PROPERTY>* m_enum;
	ENUM_PROPERTY m_lower;
	ENUM_PROPERTY m_upper;
	unsigned long m_enumNameCount;
	String* m_enumNames;	
};

/**
*TypedCollectionProperty represents a type safe wrapper around properties that
*are enumerations of items. 
*/
template <class ITEM_TYPE> class FRAMEWORK_API TypedCollectionProperty : public Property {
public:
	typedef Enumerator<ITEM_TYPE>* (Object::*GetFunction)(void);
	typedef void (Object::*AddFunction)( ITEM_TYPE );
	typedef void (Object::*InsertFunction)( const unsigned long&, ITEM_TYPE );
	typedef void (Object::*DeleteFunction1)( ITEM_TYPE );
	typedef void (Object::*DeleteFunction2)( const unsigned long& );

	TypedCollectionProperty( GetFunction getFunc, AddFunction addFunc, InsertFunction insertFunc,
		                     DeleteFunction1 deleteFunc1, DeleteFunction2 deleteFunc2,
							 const PropertyDescriptorType& propertyType ){

		init();
		m_getFunction = getFunc;
		m_addFunc = addFunc;
		m_insertFunc = insertFunc;
		m_deleteFunc1 = deleteFunc1;
		m_deleteFunc2 = deleteFunc2;		
		setType( propertyType );		
	};


	TypedCollectionProperty( const TypedCollectionProperty<ITEM_TYPE>& prop ):
		Property( prop ){

		init();
		m_getFunction = prop.m_getFunction;		
		m_enumeration = prop.m_enumeration;
		m_addFunc = prop.m_addFunc;
		m_insertFunc = prop.m_insertFunc;
		m_deleteFunc1 = prop.m_deleteFunc1;
		m_deleteFunc2 = prop.m_deleteFunc2;
	};

	void init(){
		m_getFunction = NULL;
		m_isCollection = true;
		m_enumeration = NULL;		
		m_isReadOnly = true;
		m_addFunc = NULL;
		m_insertFunc = NULL;
		m_deleteFunc1 = NULL;
		m_deleteFunc2 = NULL;
	};

	virtual ~TypedCollectionProperty(){};

	virtual String getTypeClassName() {
		return StringUtils::getClassNameFromTypeInfo( typeid(ITEM_TYPE) );
	}

	virtual VariantData* get( Object* source ){		
		return NULL;
	};

	virtual void set( Object* source, VariantData* value ){
		//no-op
	};

	virtual bool hasMoreElements( Object* source ){
		if ( NULL != m_enumeration ){
			return m_enumeration->hasMoreElements();
		}
		else {
			return false;	
		}
	};

	virtual VariantData* nextElement( Object* source ){
		VariantData* element = NULL;
		if ( NULL != m_enumeration ){
			if ( true == m_enumeration->hasMoreElements() ){
				m_value = m_enumeration->nextElement();
				element = &m_value;
			}
		}
		return element;
	}; 

	virtual void startCollection( Object* source ){
		m_enumeration = NULL;
		if ( NULL != source ){
			m_enumeration = (source->*m_getFunction)();
		}
	};

	virtual void copy( Property** copiedProperty ){
		*copiedProperty = NULL;
		*copiedProperty = new TypedCollectionProperty<ITEM_TYPE>(*this);
	};

	virtual void add( Object* source, VariantData* value ){
		if ( (NULL != value) && (NULL != m_addFunc) ){
			ITEM_TYPE valToAdd = (ITEM_TYPE)(*value);
			(source->*m_addFunc)( valToAdd ); 			
		}
	};

	virtual void insert( Object* source, const unsigned long& index, VariantData* value ){
		if ( (NULL != value) && (NULL != source) && (NULL != m_insertFunc) ){
			ITEM_TYPE valToInsert = (ITEM_TYPE)(*value);
			(source->*m_insertFunc)( index, valToInsert );
		}
	};

	virtual void remove( Object* source, VariantData* value ){
		if ( (NULL != value) && (NULL != source) && (NULL != m_deleteFunc1) ){
			ITEM_TYPE valToRemove = (ITEM_TYPE)(*value);
			(source->*m_deleteFunc1)( valToRemove );
		}
	};

	virtual void remove( Object* source, const unsigned long& index ){
		if ( (NULL != source) && (NULL != m_deleteFunc2) ){
			(source->*m_deleteFunc2)( index );
		}
	};

	virtual bool collectionSupportsEditing(){
		if ( NULL != m_enumeration ){
			return m_enumeration->supportsEditing();
		}
		else{
			return false;
		}
	};
private:
	GetFunction m_getFunction;
	Enumerator<ITEM_TYPE>* m_enumeration;
	AddFunction m_addFunc;
	InsertFunction m_insertFunc;
	DeleteFunction1 m_deleteFunc1;
	DeleteFunction2 m_deleteFunc2;
};

/**
*TypedCollectionProperty represents a type safe wrapper around properties that
*are enumerations of Object* derived items. 
*/
template <class ITEM_TYPE> class FRAMEWORK_API TypedObjectCollectionProperty : public Property {
public:
	typedef Enumerator<ITEM_TYPE>* (Object::*GetFunction)(void);
	typedef void (Object::*AddFunction)( ITEM_TYPE );
	typedef void (Object::*InsertFunction)( const unsigned long&, ITEM_TYPE );
	typedef void (Object::*DeleteFunction1)( ITEM_TYPE );
	typedef void (Object::*DeleteFunction2)( const unsigned long& );

	TypedObjectCollectionProperty( GetFunction getFunc, AddFunction addFunc, InsertFunction insertFunc,
		                           DeleteFunction1 deleteFunc1, DeleteFunction2 deleteFunc2 ){
		init();
		m_getFunction = getFunc;
		m_addFunc = addFunc;
		m_insertFunc = insertFunc;
		m_deleteFunc1 = deleteFunc1;
		m_deleteFunc2 = deleteFunc2;
	};


	TypedObjectCollectionProperty( const TypedObjectCollectionProperty& prop ):
		Property( prop ){

		init();
		m_getFunction = prop.m_getFunction;		
		m_enumeration = prop.m_enumeration;
		m_addFunc = prop.m_addFunc;
		m_insertFunc = prop.m_insertFunc;
		m_deleteFunc1 = prop.m_deleteFunc1;
		m_deleteFunc2 = prop.m_deleteFunc2;
	};	

	virtual ~TypedObjectCollectionProperty(){};

	void init(){
		m_getFunction = NULL;
		m_isCollection = true;
		m_enumeration = NULL;		
		m_isReadOnly = true;
		m_addFunc = NULL;
		m_insertFunc = NULL;
		m_deleteFunc1 = NULL;
		m_deleteFunc2 = NULL;
		setType( PROP_OBJECT );
	};

	virtual String getTypeClassName() {
		return StringUtils::getClassNameFromTypeInfo( typeid(ITEM_TYPE) );
	}

	virtual VariantData* get( Object* source ){		
		return NULL;
	};

	virtual void set( Object* source, VariantData* value ){
		//no-op
	};

	virtual bool hasMoreElements( Object* source ){
		std::map<Object*,Enumerator<ITEM_TYPE>*>::iterator it = m_enumerationMap.find( source );
		if ( it != m_enumerationMap.end() ){
			Enumerator<ITEM_TYPE>* enumeration = it->second;
			bool result = false;
			if ( NULL != enumeration ){
				result = enumeration->hasMoreElements();
				if ( false == result ){
					m_enumerationMap.erase( source );
				}				
			}
			return result;
		}
		else {
			return false;	
		}
	};

	virtual VariantData* nextElement( Object* source ){
		VariantData* element = NULL;		
		std::map<Object*,Enumerator<ITEM_TYPE>*>::iterator it = m_enumerationMap.find( source );
		if ( it != m_enumerationMap.end() ){
			Enumerator<ITEM_TYPE>* enumeration = it->second;
			if ( NULL != enumeration ){
				if ( true == enumeration->hasMoreElements() ){
					m_value = enumeration->nextElement();
					element = &m_value;
				}
			}
		}
		return element;
	}; 

	virtual void startCollection( Object* source ){		
		m_enumeration = NULL;
		if ( NULL != source ) {			
			m_enumeration = (source->*m_getFunction)();
			m_enumerationMap[ source ] = m_enumeration;
		}
	};

	virtual void copy( Property** copiedProperty ){
		*copiedProperty = NULL;
		*copiedProperty = new TypedObjectCollectionProperty<ITEM_TYPE>(*this);
	};

	virtual void add( Object* source, VariantData* value ){
		if ( (NULL != value) && (NULL != m_addFunc) ){			
			if ( PROP_OBJECT == value->type ){
				ITEM_TYPE valToAdd = (ITEM_TYPE)(Object*)(*value);
				(source->*m_addFunc)( valToAdd ); 
			}
		}
	};

	virtual void insert( Object* source, const unsigned long& index, VariantData* value ){
		if ( (NULL != value) && (NULL != m_insertFunc) ){			
			if ( PROP_OBJECT == value->type ){
				ITEM_TYPE valToInsert = (ITEM_TYPE)(Object*)(*value);
				(source->*m_insertFunc)( index, valToInsert ); 
			}
		}
	};

	virtual void remove( Object* source, VariantData* value ){
		if ( (NULL != value) && (NULL != m_deleteFunc1) ){
			ITEM_TYPE valToRemove = (ITEM_TYPE)(Object*)(*value);
			(source->*m_deleteFunc1)( valToRemove ); 
		}
	};

	virtual void remove( Object* source, const unsigned long& index ){
		if ( NULL != m_deleteFunc2 ){			
			(source->*m_deleteFunc2)( index ); 
		}
	};

	virtual bool collectionSupportsEditing(){
		if ( NULL != m_enumeration ){
			return m_enumeration->supportsEditing();
		}
		else{
			return false;
		}
	};
private:
	GetFunction m_getFunction;
	AddFunction m_addFunc;
	InsertFunction m_insertFunc;
	DeleteFunction1 m_deleteFunc1;
	DeleteFunction2 m_deleteFunc2;

	Enumerator<ITEM_TYPE>* m_enumeration;
	std::map<Object*, Enumerator<ITEM_TYPE>*> m_enumerationMap;
};


#endif //#ifdef VCF_RTTI



class EventProperty : public Object {
public :
	EventProperty( const String& eventClassName, const String& handlerClassName,
					const String& eventMethodName ){
		m_eventClassName = eventClassName;
		m_handlerClassName = handlerClassName;	
		m_eventMethodName = eventMethodName;
	};

	virtual ~EventProperty(){};	
	
	String getEventClassName(){
		return m_eventClassName;	
	};
	
	String getHandlerClassName(){
		return m_handlerClassName;
	};
	
	String getEventMethodName() {
		return m_eventMethodName;
	}
	
protected:
	String m_eventClassName;
	String m_handlerClassName;	
	String m_eventMethodName;
};



}; //end of namespace VCF


#endif //_PROPERTY_H__