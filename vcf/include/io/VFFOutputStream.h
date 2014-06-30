/*
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world.
*/
//VFFOutputStream.h


#ifndef _VFFOUTPUTSTREAM_H__
#define _VFFOUTPUTSTREAM_H__


namespace VCF {



class APPKIT_API VFFOutputStream : public Object, public OutputStream {
public:
	VFFOutputStream( OutputStream* stream ) {
		m_stream = stream;
		m_tabLevel = 0;
	}

	virtual ~VFFOutputStream(){};

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom) {
		m_stream->seek( offset, offsetFrom );
	}

	virtual unsigned long getSize() {
		return m_stream->getSize();
	}

	virtual char* getBuffer() {
		return m_stream->getBuffer();
	}

	virtual void write( char* bytesToWrite, unsigned long sizeOfBytes ) {
		m_stream->write( bytesToWrite, sizeOfBytes );
	}

	void writeComponent( Component* component );

	String getTabString();
protected:
	OutputStream* m_stream;
	long m_tabLevel;

	String binToHex( Persistable* persistableObject );

	void writeObject( Object* object, const String& objectPropertyName );	
};

};

#endif //_VFFOUTPUTSTREAM_H__