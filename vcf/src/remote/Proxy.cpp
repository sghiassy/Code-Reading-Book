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

#include "FoundationKit.h"

#include "Proxy.h"
#include "ClassServerInstance.h"
#include "MemoryStream.h"

using namespace VCFRemote;
using namespace VCF;

Proxy::Proxy( Object* localObjectInstance )
{
	this->m_className = "";
	this->m_classID = "";
	this->m_localObjectInstance = localObjectInstance;
	this->m_isRemote = (m_localObjectInstance == NULL);
	if ( NULL != m_localObjectInstance ) {
		Class* clazz = m_localObjectInstance->getClass();
		this->m_className = clazz->getClassName();
		this->m_classID = clazz->getID();
		this->m_instanceID.setMemAddress( (int)m_localObjectInstance );
	}
}

Proxy::~Proxy()
{
	if ( NULL != m_localObjectInstance ) {
		delete m_localObjectInstance;
		m_localObjectInstance = NULL;
	}
}

void Proxy::marshallArguments( const VCF::uint32& argCount, 
								VCF::VariantData** arguments,
								VCF::OutputStream* marshallingStream )
{
	/**marshall data as follows
	*number of arguments being sent - int
	*data signature - string
	*data 
	*at the moment this does not handle arrays of data
	*/
	marshallingStream->write( (int)argCount );
	
	for ( int i = 0;i<argCount;i++ ) {
		VariantData* arg = arguments[i];
		switch ( arg->type ) {
			case PROP_INT : {
				marshallingStream->write( String("i") );
				marshallingStream->write( (int)(*arg) );
			}
			break;

			case PROP_LONG : {
				marshallingStream->write( String("l") );
				marshallingStream->write( (long)(*arg) );
			}
			break;

			case PROP_ULONG : {
				marshallingStream->write( String("+l") );
				marshallingStream->write( (long)(*arg) );
			}
			break;

			case PROP_SHORT : {
				marshallingStream->write( String("h") );
				marshallingStream->write( (short)(*arg) );
			}
			break;

			case PROP_CHAR : {
				marshallingStream->write( String("c") );
				marshallingStream->write( (char)(*arg) );
			}
			break;

			case PROP_DOUBLE : {
				marshallingStream->write( String("d") );
				marshallingStream->write( (double)(*arg) );
			}
			break;

			case PROP_FLOAT : {
				marshallingStream->write( String("f") );
				marshallingStream->write( (float)(*arg) );
			}
			break;

			case PROP_BOOL : {
				marshallingStream->write( String("b") );
				marshallingStream->write( (bool)(*arg) );
			}
			break;

			case PROP_STRING : {
				marshallingStream->write( String("s") );
				marshallingStream->write( (String)(*arg) );
			}
			break;

			case PROP_OBJECT : {
				Object* obj = *arg;
				Persistable* persistentObj = dynamic_cast<Persistable*>(obj);
				if ( NULL != persistentObj ) { 
					marshallingStream->write( String("o") );
					marshallingStream->write( obj->getClassName() );	
					marshallingStream->write( persistentObj );
				}
				else {
					//throw exception
				}
			}
			break;
		}
	}
}

VariantData* Proxy::invoke( const VCF::String& methodName, const uint32& argCount, VariantData** arguments )
{
 	VariantData* result = NULL;	
	
	if ( NULL != m_localObjectInstance ) {
		Class* clazz = m_localObjectInstance->getClass();
		Method* methodToInvoke = clazz->getMethod( methodName );
		if ( NULL != methodToInvoke ) {
			result = methodToInvoke->invoke( arguments );
		}
	}
	else  {
		BasicOutputStream memStream;
		
		OutputStream* marshallingStream = &memStream;
		Proxy* thisPtr = this;
		marshallingStream->write( thisPtr );
		marshallingStream->write( methodName );
		//marshall data to the stream
		marshallArguments( argCount, arguments, marshallingStream );
		//data marshalled to marshallingStream
		
		ClassServerInstance classSvrInst;
		result = classSvrInst.invoke( marshallingStream );
	}	
	return result;
}


VCF::String Proxy::getClassName()
{
	return m_className;
}

VCF::String Proxy::getClassID()
{
	return m_classID;	
}

Proxy* Proxy::createInstance( const VCF::String& className )
{
	Proxy* result = NULL;

	ClassServerInstance classSvrInst;

	result = classSvrInst.createInstance( className );

	return result;
}

Proxy* Proxy::createInstanceByClassID( const VCF::String& classID )
{
	Proxy* result = NULL;

	ClassServerInstance classSvrInst;

	result = classSvrInst.createInstanceByClassID( classID );

	return result;
}

void Proxy::saveToStream( VCF::OutputStream * stream )
{
	stream->write( m_classID );
	stream->write( m_className );
	stream->write( m_isRemote );
	stream->write( &m_instanceID );
}

void Proxy::loadFromStream( VCF::InputStream * stream )
{
	stream->read( m_classID );
	stream->read( m_className );
	stream->read( m_isRemote );
	stream->read( &m_instanceID );
}