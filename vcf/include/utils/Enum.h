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

// Enum.h: 

#ifndef _ENUM_H__
#define _ENUM_H__


namespace VCF
{

/**
*the Enum class is used to wrap C++ enum types,
*allowing the user to iterate forward and backward
*through the enum type. This works best with enums
*that have members whose value changes in intervals
*of 1.
*/
class FRAMEWORK_API Enum : public Object{
public:
	Enum(){};

	virtual ~Enum(){};

	/**
	*moves to the next value.
	*When the end of the enum is reached
	*it cycles back to the begining
	*@return int the int value of the enum
	*/
	virtual int next() = 0;

	/**
	*Moves to the last value of the enum
	*@return int the int value of the enum
	*/
	virtual int end() = 0;

	/**
	*gets the current value of the enum
	*@return int the int value of the enum
	*/
	virtual int get() = 0;
	
	/**
	*Moves to the first value of the enum
	*@return int the int value of the enum
	*/
	virtual int begin() = 0;

	/**
	*Sets the current value of the enum
	*@return int the int value of the enum
	*/
	virtual void set( const int& intVal ) = 0;

	operator int (){
		return get();
	};

	/**
	*sets the enum val from a string. The default behaviour 
	*is to assume the string represents an integer (i.e. "1" or "3")
	*if this is not the case then the enum wil be set to 0 value
	*/
	virtual void setFromString( const String& textVal ){
		int i = 0;		
		sscanf( textVal.c_str(), "%d", &i );
		set( i );
	};
};


/**
*This is specific template class used to wrap the 
*C++ enum type (ENUM_TYPE) to the Enum class.
*/
template <class ENUM_TYPE> class FRAMEWORK_API TypedEnum : public Enum {
public:

	TypedEnum( const ENUM_TYPE& lower, const ENUM_TYPE& upper ){
		m_enum = lower;
		m_lower = (int)lower;
		m_upper = (int)upper;
		m_enumNames.clear();
		m_namesAvailable = false;
	};
	
	TypedEnum( const ENUM_TYPE& lower, const ENUM_TYPE& upper, const unsigned long& enumNameCount, String* enumNames ){
		m_enum = lower;
		m_lower = (int)lower;
		m_upper = (int)upper;
		m_enumNames.clear();
		if ( enumNameCount > 0 ){
			for ( int i=0;i<enumNameCount;i++ ){
				m_enumNames.push_back( enumNames[i] );
			}
		}
		m_namesAvailable = m_enumNames.size() > 0 ? true : false;
	};

	virtual ~TypedEnum() {};

	virtual int next(){
		int i = (int)m_enum;
		i++;		
		if ( i > m_upper ){			
			i = m_lower;
		}
		m_enum = (ENUM_TYPE)i;
		return i;
	};

	virtual int begin(){
		m_enum = (ENUM_TYPE)m_lower;
		return (int)m_enum;
	};

	virtual int end(){
		m_enum = (ENUM_TYPE)m_upper;
		return (int)m_enum;
	};

	virtual int get(){
		return (int)m_enum;
	};
	
	void set( const ENUM_TYPE& val ){
		m_enum = val;
	};

	virtual void set( const int& intVal ){
		m_enum = (ENUM_TYPE)intVal;
	}

	ENUM_TYPE getValAsEnum(){
		return m_enum;
	};

	operator ENUM_TYPE (){
		return getValAsEnum();
	};	

	TypedEnum<ENUM_TYPE>& operator=( const ENUM_TYPE& val ){
		set( val );
		return *this;
	};	
	
	virtual void setFromString( const String& textVal ){
		if ( true == m_namesAvailable ){
			std::vector<String>::iterator it = m_enumNames.begin();
			int i = 0;
			bool found = false;
			while ( it != m_enumNames.end() ){
				if ( (*it) == textVal ){
					found = true;
					break;
				}
				i++;
				it++;
			}
			if ( true == found ){
				set( i );
			}
			else {
				Enum::setFromString( textVal );	
			}
		}
		else {
			Enum::setFromString( textVal );
		}
	};

	virtual String toString(){
		String result = "";
		
		if ( true == m_namesAvailable ){
			result = m_enumNames[get()];
		}
		else {
			char tmp[25];
			memset( tmp, 0, 25 );
			sprintf( tmp, "%d", get() );
			result += tmp;
		}
		return result;
	};

private:
	ENUM_TYPE m_enum;
	int m_lower;
	int m_upper;
	std::vector<String> m_enumNames;
	bool m_namesAvailable;
};



class FRAMEWORK_API EnumValue {
public:
	Enum* getEnum() {
		if ( NULL != m_enum ) {
			m_enum->set( m_enumValue );
		}
		return m_enum;
	}
	
	int getEnumVal() {
		return 	m_enumValue;
	}

	void set( const EnumValue& enumVal ) {
		m_enum = enumVal.m_enum;
		if ( NULL != m_enum ) {
			m_enumValue = m_enum->get();
		}
		else {
			m_enumValue = 0;
		}
	}

	void set( const String& stringVal ) {
		if ( NULL != m_enum ) {
			m_enum->setFromString( stringVal );
			m_enumValue = m_enum->get();
		}
		else {
			m_enumValue = 0;
		}
	}

	void set( Enum* enumVal ) {
		m_enum = enumVal;
		if ( NULL != m_enum ) {
			m_enumValue = m_enum->get();
		}
		else {
			m_enumValue = 0;
		}
	}
	
protected:
	int m_enumValue;
	Enum* m_enum;
};

};

#endif 
