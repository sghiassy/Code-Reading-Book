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

#ifndef _TABMODELEVENT_H__
#define _TABMODELEVENT_H__



namespace VCF {


#define TABMODELEVENT_CONST						232

#define TAB_MODEL_EVENT_ITEM_ADDED				CUSTOM_EVENT_TYPES + TABMODELEVENT_CONST + 1
#define TAB_MODEL_EVENT_ITEM_REMOVED			CUSTOM_EVENT_TYPES + TABMODELEVENT_CONST + 2
#define TAB_MODEL_EVENT_ITEM_SELECTED			CUSTOM_EVENT_TYPES + TABMODELEVENT_CONST + 3

class TabPage;

class TabModelEvent : public Event{
public:
	TabModelEvent( Object * source, TabPage* page );

	TabModelEvent( Object* source, const unsigned long& eventType, TabPage* page );

	virtual ~TabModelEvent();

	TabPage* getTabPage();
private:
	TabPage* m_page;

};

}; //end of namespace VCF

#endif //_TABMODELEVENT_H__