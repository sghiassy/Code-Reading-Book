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

// ComponentOutputStream.cpp
#include "ApplicationKit.h"
#include "Control.h"
#include "ComponentOutputStream.h"

using namespace VCF;


ComponentOutputStream::ComponentOutputStream( OutputStream* outStream )
{
	this->m_outStream = outStream;
	m_needsDTD = true;
}

ComponentOutputStream::~ComponentOutputStream()
{
	
}

void ComponentOutputStream::seek(const unsigned long& offset, const SeekType& offsetFrom)
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->seek( offset, offsetFrom );
	}
}

unsigned long ComponentOutputStream::getSize()
{
	if ( NULL != this->m_outStream ){
		return this->m_outStream->getSize();
	}

	return 0;
}

char* ComponentOutputStream::getBuffer()
{
	if ( NULL != this->m_outStream ){
		return this->m_outStream->getBuffer();
	}
	else{
		return NULL;
	}	
}

void ComponentOutputStream::write( char* bytesToWrite, unsigned long sizeOfBytes )
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( bytesToWrite, sizeOfBytes );
	}
}	
	
void ComponentOutputStream::write( const short& val )
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( val );
	}
}
	
void ComponentOutputStream::write( const long& val )
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( val );
	}
}

void ComponentOutputStream::write( const int& val )
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( val );
	}
}

void ComponentOutputStream::write( const bool& val )
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( val );
	}
}

void ComponentOutputStream::write( const char& val )
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( val );
	}
}

void ComponentOutputStream::write( const float& val )
	{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( val );
	}
}

void ComponentOutputStream::write( const double& val )
{
	if ( NULL != this->m_outStream ){
		this->m_outStream->write( val );
	}
}	

void ComponentOutputStream::write( const String& val )
{
	if ( NULL != this->m_outStream ){
		String tmp = val + "\n";
		this->m_outStream->write( tmp );
	}
}

void ComponentOutputStream::writeComponent( Component* component )
{
	if ( NULL != component ){
		Class* clazz = component->getClass();
		Control* parent = NULL;
				
		if ( true == m_needsDTD ){
			//dump the DTD
			String hdr = "<?xml version=\"1.0\" standalone=\"no\"?>";
			this->write( hdr );
			ComponentDTD dtd;
			this->write( dtd.toString() );
		}
		
		m_needsDTD = false;

		if ( NULL != clazz ){
			String tmp = "";
			String strVal = "";
			String propName = "";
			Enumerator<Property*>* props = clazz->getProperties();
			tmp = "<object class=\"" + component->getClassName() + "\" classID=\"" + clazz->getID() + "\">";
			this->write( tmp );		
			
			if ( NULL != props ){
				while ( props->hasMoreElements() ){
					Property* prop = props->nextElement();
					writeProperty( prop, component );					
				}
			}//NULL != props
			//now check for children
			Container* container = dynamic_cast<Container*>( component );
			if ( NULL != container ){
				
				Enumerator<Control*>* children = container->getChildren();
				if ( NULL != children ){
					tmp = "<children>";
					this->write( tmp );

					while ( children->hasMoreElements() ){
						Control* child = children->nextElement();
						if ( NULL != child ){
							this->writeComponent( child );
						}
					}

					tmp = "</children>";
					this->write( tmp );
				}				
			}

			//now check for Components 
			Enumerator<Component*>* components = component->getComponents();
			if ( NULL != components ) {
				tmp = "<components>";
				this->write( tmp );	
				while ( true == components->hasMoreElements() ) {
					Component* comp = components->nextElement();
					if ( NULL != comp ) {
						writeComponent( comp );
					}
				}
				tmp = "</components>";
				this->write( tmp );	
			}
			tmp = "</object>";
			this->write( tmp );
		}//NULL != clazz
	}
}

void ComponentOutputStream::writePropertyValue( const String& propName, VariantData* value )
{
	String tmp = "";
	if ( NULL != value ){
		tmp = "\t<property name=\"" + propName + "\" ";
		String strVal = "null";
		if ( PROP_OBJECT != value->type ){
			strVal = value->toString();
			tmp += "value=\"" + strVal + "\"/>";
			this->write( tmp );
		}
		else {
			Object* object = *value;
			if ( NULL != object ){
				strVal = object->getClassName();
				tmp += "value=\"" + strVal + "\">";	
				this->write( tmp );
				Class* objectClass = object->getClass();
				if ( NULL != objectClass ){
					Enumerator<Property*>* objectProps = objectClass->getProperties();
					tmp = "\t<objectProperty>";
					this->write( tmp );
					if ( NULL != objectProps ){						
						while ( objectProps->hasMoreElements() ){
							Property* objectProp = objectProps->nextElement();
							writeProperty( objectProp, object );
						}						
					}
					tmp = "\t</objectProperty>";
					this->write( tmp );
				}
				else{
					tmp = "\t<objectProperty></objectProperty>";
					this->write( tmp );
				}
				tmp = "</property>";
				this->write( tmp );
			}
			else {
				tmp += "value=\"null\"/>";	
				this->write( tmp );
			}
		}
	}
	else{
		tmp = "\t<property name=\"" + propName + " \"null\"";
		this->write( tmp );
	}	
}

void ComponentOutputStream::writeProperty( Property* property, Object* source )
{
	if ( NULL != property ){
		String tmp = "";
		String propName = property->getName();
		if ( true == property->isCollection() ){
			property->startCollection( source );
			while ( property->hasMoreElements( source ) ){
				VariantData* collectionVal = property->nextElement( source );
				writePropertyValue( propName, collectionVal );
			}
		}
		else {
			VariantData* val = property->get();
			this->writePropertyValue( propName, val );
		}
	}
}
