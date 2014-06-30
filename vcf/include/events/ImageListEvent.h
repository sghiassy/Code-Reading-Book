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

// ImageListEvent.h

#if !defined(_IMAGELISTEVENT_H__)
#define _IMAGELISTEVENT_H__



namespace VCF
{


#define IMAGELIST_CONST						1100

#define IMAGELIST_EVENT_WIDTH_CHANGED		CUSTOM_EVENT_TYPES + IMAGELIST_CONST + 1
#define IMAGELIST_EVENT_HEIGHT_CHANGED		CUSTOM_EVENT_TYPES + IMAGELIST_CONST + 2
#define IMAGELIST_EVENT_CREATED				CUSTOM_EVENT_TYPES + IMAGELIST_CONST + 3
#define IMAGELIST_EVENT_DESTROYED			CUSTOM_EVENT_TYPES + IMAGELIST_CONST + 4
#define IMAGELIST_EVENT_ITEM_ADDED			CUSTOM_EVENT_TYPES + IMAGELIST_CONST + 5
#define IMAGELIST_EVENT_ITEM_DELETED		CUSTOM_EVENT_TYPES + IMAGELIST_CONST + 6


class APPKIT_API ImageListEvent : public Event  
{
public:
	
	ImageListEvent( Object * source);

	ImageListEvent( Object* source, const unsigned long& eventType, Image* image=NULL );

	virtual ~ImageListEvent();
	
	Image* getImage() {
		return m_image;
	}

	ulong32 getIndexOfImage() {
		return m_imageIndex;
	}

	void setIndexOfImage( const ulong32& indexOfImage ) {
		m_imageIndex = indexOfImage;
	}
protected:
	Image* m_image;
	ulong32 m_imageIndex;
};

};

#endif 
