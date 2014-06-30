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

// ComponentOutputStream.h

#ifndef _COMPONENTOUTPUTSTREAM_H__
#define _COMPONENTOUTPUTSTREAM_H__

#include "Stream.h"
#include "Persistable.h"

namespace VCF
{

class Control;

class Persistable;

class Property;

class VariantData;

class APPKIT_API ComponentOutputStream : public OutputStream  
{
public:
	ComponentOutputStream( OutputStream* outStream );

	virtual ~ComponentOutputStream();

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom);

	virtual unsigned long getSize();

	virtual char* getBuffer();

	virtual void write( char* bytesToWrite, unsigned long sizeOfBytes );	
	
	virtual void write( Persistable* persistableObject ){
		OutputStream::write( persistableObject );
	};

	virtual void write( const short& val );
	
	virtual void write( const long& val );

	virtual void write( const int& val );

	virtual void write( const bool& val );

	virtual void write( const char& val );

	virtual void write( const float& val );

	virtual void write( const double& val );

	virtual void write( const String& val );

	/**
	*Writing a component out to a ComponentOutputStream takes 
	*the following steps:
	*	1.	Open a <object> tag and write out the class name (<object class="component::ClassName" )
	*	2.	Get the Class for the component and then write out the properties
	*	3.	For each property write out a <property> tag with the name of the 
	*		property (<property name="propertyName" ). If the value of the property
	*		is a basic type (i.e. int, double, etc ), then write out the string value
	*		of the property. If the property value is a Object* then the value should be the 
	*		class name of the Object* ( <property name="listModel" value="DefaultListModel"> ).
	*	4.	If the value of the property is a Object* then open a new <objectProperty> tag. Follow
	*		steps 2 & 3. Close out the tag with a </objectProperty> tag.
	*	5.	Close the <property> tag with a </property> tag.
	*	6.	Close the <object> tag with </object>.
	*/
	void writeComponent( Component * component );
private:
	OutputStream* m_outStream;
	bool m_needsDTD;
	void writeProperty( Property* property, Object* source );
	void writePropertyValue( const String& propName, VariantData* value );
};

};

#endif 
