/*
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world.
*/
//VFFInputStream.h

#ifndef _VFFINPUTSTREAM_H__
#define _VFFINPUTSTREAM_H__

namespace VCF {

class InputStream;
class Parser;

class APPKIT_API VFFInputStream : public Object, public InputStream {
public:
	VFFInputStream( InputStream* stream );

	virtual ~VFFInputStream();

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom) {
		m_stream->seek( offset, offsetFrom );
	}

	virtual unsigned long getSize() {
		return m_stream->getSize();
	}

	virtual char* getBuffer() {
		return m_stream->getBuffer();
	}

	virtual void read( char* bytesToRead, unsigned long sizeOfBytes ) {
		m_stream->read( bytesToRead, sizeOfBytes );
	}

	/**
	*retreives the outer most class/UUID that contains all other obejcts 
	*in this VFF stream
	*/
	void getOuterClassNameAndUUID( String& className, String& UUID );

	/**
	*reads in a new component, that is the caller passes in a pointer to 
	*a pointer of a Component, and the method creates a new instance for the caller.
	*@param Component**  the derefenced component ptr should be NULL when invoking this
	*method, usually this is a top level component like a Frame
	*/
	void readComponent( Component** component );

	/**
	*reads in a Component from the stream, and assigns it's value to the 
	*component instance passed in
	*@param Component* must NOT be null. Represents the already existing component 
	*that will be updated as a result of reading the contents of the stream.
	*/
	void readComponentInstance( Component* component );
protected:

	class DeferredPropertySetter {
	public:
		DeferredPropertySetter( const String& propertyVal, const String& propertyName, Object* source ) {
			m_propertyVal = propertyVal;
			m_propertyName = propertyName;
			m_source = source;
		}

		virtual ~DeferredPropertySetter(){};
		String m_propertyVal;
		String m_propertyName;
		Object* m_source;	
	};

	InputStream* m_stream;
	Parser* m_parser;
	void hexToBin( const String& hexString, Persistable* persistableObject );
	
	void readChildComponents( Component* component );

	void processAsignmentTokens( const VCFChar& token, const String& currentSymbol, Class* clazz );

	std::vector<DeferredPropertySetter*> m_deferredProperties;
	bool m_atTopLevel;
	long m_componentInputLevel;
};


};

#endif //_VFFINPUTSTREAM_H__