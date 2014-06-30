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

#ifndef _VARIANTDATASTREAM_H__
#define _VARIANTDATASTREAM_H__


namespace VCF {

/**
*The VariantDataInputStream is used to read 
*a VariantData object from an InputStream source. 
*/
class FRAMEWORK_API VariantDataInputStream : public InputStream {

public:
	VariantDataInputStream( InputStream* sourceInputStream ){
		m_inStream = sourceInputStream;
	};

	virtual ~VariantDataInputStream(){};

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom)	{
		if ( NULL == m_inStream ) {
			throw InvalidPointerException();
		}
		m_inStream->seek( offset, offsetFrom );
	}

	virtual unsigned long getSize() {
		if ( NULL == m_inStream ) {
			throw InvalidPointerException();
		}
		return m_inStream->getSize();
	}

	virtual char* getBuffer() {
		if ( NULL == m_inStream ) {
			throw InvalidPointerException();
		}
		return m_inStream->getBuffer();
	}

	virtual void read( char* bytesToRead, unsigned long sizeOfBytes ) {
		if ( NULL == m_inStream ) {
			throw InvalidPointerException();
		}
		m_inStream->read( bytesToRead, sizeOfBytes );
	}

	void readVariantData( VariantData* data ) {
		String type = "";
		m_inStream->read( type );
		if ( type == "i" ) {
			int val = 0;
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "l" ) {			
			long val = 0;
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "+l" ) {
			long val = 0;
			m_inStream->read( val );
			*data = (unsigned long)val;
		}
		else if ( type == "h" ) {
			short val = 0;
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "c" ) {
			char val = 0;
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "d" ) {
			double val = 0.0;
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "f" ) {
			float val = 0.0f;
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "b" ) {
			bool val = false;
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "s" ) {
			String val = "";
			m_inStream->read( val );
			*data = val;
		}
		else if ( type == "o" ) {
			Object* obj = *data;
			if ( NULL != obj ) {
				Persistable* persistentObj = dynamic_cast<Persistable*>(obj);
				if ( NULL != persistentObj ) {
					m_inStream->read( persistentObj );
				}
				else {
					throw TypeCastException("Error attempting to read from stream.\nObject doesn't support Persistable interface!"); 
				}
			}
			else {
				throw InvalidPointerException(); 
			}
		}		
	}
private:
	InputStream* m_inStream;
};

/**
*The VariantDataOutputStream is used to write a
*VariantData object to an OutputStream source. 
*/
class FRAMEWORK_API VariantDataOutputStream : public OutputStream{

public:
	VariantDataOutputStream( OutputStream* sourceOutputStream ){
		m_outStream = sourceOutputStream;
	};

	virtual ~VariantDataOutputStream(){};

	virtual void seek(const unsigned long& offset, const SeekType& offsetFrom)	{
		if ( NULL == m_outStream ) {
			throw InvalidPointerException();
		}
		m_outStream->seek( offset, offsetFrom );
	}

	virtual unsigned long getSize() {
		if ( NULL == m_outStream ) {
			throw InvalidPointerException();
		}
		return m_outStream->getSize();
	}

	virtual char* getBuffer() {
		if ( NULL == m_outStream ) {
			throw InvalidPointerException();
		}
		return m_outStream->getBuffer();
	}

	virtual void write( char* bytesToWrite, unsigned long sizeOfBytes ) {
		if ( NULL == m_outStream ) {
			throw InvalidPointerException();
		}
		m_outStream->write( bytesToWrite, sizeOfBytes );
	}

	void writeVariantData( VariantData* data ) {
		switch ( data->type ) { 
			case PROP_INT : {
				m_outStream->write( String("i") );
				m_outStream->write( (int)(*data) );
			}
			break;

			case PROP_LONG : {
				m_outStream->write( String("l") );
				m_outStream->write( (long)(*data) );
			}
			break;

			case PROP_ULONG : {
				m_outStream->write( String("+l") );
				m_outStream->write( (long)(*data) );
			}
			break;

			case PROP_SHORT : {
				m_outStream->write( String("h") );
				m_outStream->write( (short)(*data) );
			}
			break;

			case PROP_CHAR : {
				m_outStream->write( String("c") );
				m_outStream->write( (char)(*data) );
			}
			break;

			case PROP_DOUBLE : {
				m_outStream->write( String("d") );
				m_outStream->write( (double)(*data) );
			}
			break;

			case PROP_FLOAT : {
				m_outStream->write( String("f") );
				m_outStream->write( (float)(*data) );
			}
			break;

			case PROP_BOOL : {
				m_outStream->write( String("b") );
				m_outStream->write( (bool)(*data) );
			}
			break;

			case PROP_STRING : {
				m_outStream->write( String("s") );
				m_outStream->write( (String)(*data) );
			}
			break;

			case PROP_OBJECT : {
				Object* obj = *data;
				if ( NULL != obj ) {
					Persistable* persistentObj = dynamic_cast<Persistable*>(obj);
					if ( NULL != persistentObj ) { 
						m_outStream->write( String("o") );
						m_outStream->write( obj->getClassName() );	
						m_outStream->write( persistentObj );
					}				
					else {
						throw TypeCastException("Error attempting to write to stream.\nObject doesn't support Persistable interface!"); 
					}
				}
				else {
					throw InvalidPointerException();
				}
			}
			break;
		}
	}
private:
	OutputStream* m_outStream;
};

}; //end of namespace VCF

#endif //_VARIANTDATASTREAM_H__