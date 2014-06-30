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

//MemStreamUtils.h

/**
* @author  Jim Crafton
* @date 3/7/2000
* @version 1.0
*/

#ifndef _MEMSTREAMUTILS_H__
#define _MEMSTREAMUTILS_H__

namespace VCF {
/**
*GenericMemoryBuffer
*/

template <class DATA_TYPE> class FRAMEWORK_API GenericMemoryBuffer {
public:
	GenericMemoryBuffer( const ulong32 allocationSize = 65536) {
		m_allocationSize = allocationSize;
		m_currentAllocationSize = 0;
		m_currentBuffer = NULL;
		m_bytesWritten = 0;
		m_bytesRead = 0;		
	}

	virtual ~GenericMemoryBuffer(){
		delete [] m_currentBuffer;
		m_currentBuffer = NULL;
	}

	inline DATA_TYPE* getBuffer()	{
		return m_currentBuffer;
	}

	inline ulong32 getSize()  {
		return m_currentAllocationSize;
	}

	inline void write( const char* buffer, const ulong32& sizeOfBuffer ) {
		if ( m_currentAllocationSize < (m_bytesWritten + sizeOfBuffer) ) {
			allocate();
		}

		char* tmp = (char*)m_currentBuffer;
		tmp += m_bytesWritten;

		memcpy( tmp, buffer, sizeOfBuffer );

		m_bytesWritten += sizeOfBuffer;
	}

	inline void read( char* buffer, const ulong32& sizeOfBuffer ) {
		if ( m_currentAllocationSize < (m_bytesRead + sizeOfBuffer) ) {
			throw NoFreeMemException("No longer have enough memory in this buffer to read from.");
		}

		char* tmp = (char*)m_currentBuffer;
		tmp += m_bytesRead;
		
		memcpy( buffer, tmp, sizeOfBuffer );

		m_bytesRead += sizeOfBuffer;
	}

	inline void clear() {
		m_bytesWritten = 0;
		m_currentAllocationSize = 0;
		delete [] m_currentBuffer;
		m_currentBuffer = NULL;
		allocate();
	}
protected:
	inline void allocate() {
		ulong32 oldSize = m_currentAllocationSize;

		m_currentAllocationSize += m_allocationSize;
		
		DATA_TYPE* tmp = new DATA_TYPE[m_currentAllocationSize];
		
		if ( NULL == tmp ) {
			throw NoFreeMemException();
		}

		if ( NULL != m_currentBuffer ) {
			//copy over the old bits
			memcpy( tmp, m_currentBuffer, oldSize );
			delete [] m_currentBuffer;
		}

		m_currentBuffer = tmp;
	}
private:
	ulong32 m_allocationSize;
	DATA_TYPE* m_currentBuffer;
	ulong32 m_currentAllocationSize;
	ulong32 m_bytesWritten;
	ulong32 m_bytesRead;
	
};




/**
*CharMemStream
*/
class FRAMEWORK_API CharMemStream : public GenericMemoryBuffer<char> {
public:
	CharMemStream( const ulong32& allocationSize = 65536 ) :
		GenericMemoryBuffer<char> ( allocationSize ) {
	}

	virtual ~CharMemStream(){};

};

}; //end of namespace VCF

#endif // __MemStream_h__
