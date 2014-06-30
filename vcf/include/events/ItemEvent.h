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

// ItemEvent.h

#ifndef _ITEMEVENT_H__
#define _ITEMEVENT_H__



namespace VCF
{

class GraphicsContext;

#define ITEM_CONST						300

#define ITEM_EVENT_PAINT				CUSTOM_EVENT_TYPES + ITEM_CONST + 1
#define ITEM_EVENT_CHANGED				CUSTOM_EVENT_TYPES + ITEM_CONST + 2
#define ITEM_EVENT_SELECTED				CUSTOM_EVENT_TYPES + ITEM_CONST + 3
#define ITEM_EVENT_ADDED				CUSTOM_EVENT_TYPES + ITEM_CONST + 4
#define ITEM_EVENT_DELETED				CUSTOM_EVENT_TYPES + ITEM_CONST + 5
#define ITEM_EVENT_TEXT_CHANGED			CUSTOM_EVENT_TYPES + ITEM_CONST + 6

class APPKIT_API ItemEvent : public Event  
{
public:
	ItemEvent( Object* source );

	ItemEvent( Object* source, const unsigned long& eventType );
	
	ItemEvent( Object* source, GraphicsContext* context );

	virtual ~ItemEvent();

	GraphicsContext* getContext();

	/**
	*returns the point for this mouse event
	*/
	Point* getPoint();

	void setPoint( Point* point );

private:
	GraphicsContext* m_paintContext;
	Point m_point;
};

};

#endif 
