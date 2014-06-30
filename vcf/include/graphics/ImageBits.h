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

//ImageBits.h

#ifndef _IMAGE_BITS_H__
#define _IMAGE_BITS_H__


namespace VCF
{

struct RGBVals{
	char b;
	char g;
	char r;
};

struct RGBAVals{
	char b;
	char g;
	char r;
	char a;
};

struct RGBValsf{
	float b;
	float g;
	float r;
};

struct RGBAValsf{
	float b;
	float g;
	float r;
	float a;
};


/**
*Basic class for image bit twiddling 
*default behaviour is to allow for RGBA channels
*/

#define IMAGEBITS_CLASSID		"ED88C098-26AB-11d4-B539-00C04F0196DA"

class GRAPHICSKIT_API ImageBits : public Object {
public :

	BEGIN_CLASSINFO(ImageBits, "VCF::ImageBits", "VCF::Object", IMAGEBITS_CLASSID)		
	END_CLASSINFO(ImageBits)

	ImageBits(){
		
		m_width = 0;
		m_height = 0;
		m_bits = NULL;
		this->init();
		m_needsMemAlloc = true;
	};

	ImageBits( const unsigned long& width, const unsigned long& height, const bool& needsMemAlloc=true ){		
		m_width = width;
		m_height = height;
		m_bits = NULL;
		m_needsMemAlloc = needsMemAlloc;
		this->init();
	};

	virtual ~ImageBits(){
		if ( (NULL != m_bits) && (true == m_needsMemAlloc) ) {
			delete [] m_bits;	
		}
	};

	void init(){
		if ( true == m_needsMemAlloc ) {
			if ( NULL != m_bits ){
				delete [] m_bits;
			}
			unsigned long size = getSize();
			if ( size > 0 ){
				m_bits = new RGBAVals[size];
			}
			else{
				m_bits = NULL;
			}
		}
	};

	void setWidth( const unsigned long& width ){
		m_width = width;
		this->init();
	};

	void setHeight( const unsigned long& height ){
		m_height = height;
		this->init();
	};

	unsigned long m_width;

	unsigned long m_height;
	
	unsigned long getSize(){
		return m_height * m_width;
	};
	
	bool m_needsMemAlloc;

	RGBAVals* m_bits;
};

};
	
#endif