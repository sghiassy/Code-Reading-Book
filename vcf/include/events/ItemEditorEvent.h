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

#ifndef _ITEMEDITOREVENT_H__
#define _ITEMEDITOREVENT_H__



namespace VCF {

#define ITEMEDITOR_EVENT_CONST				3000

#define ITEMEDITOR_VALIDATION				CUSTOM_EVENT_TYPES + ITEMEDITOR_EVENT_CONST + 1
#define ITEMEDITOR_CHANGED					CUSTOM_EVENT_TYPES + ITEMEDITOR_EVENT_CONST + 2

class TableCellItem;


class ItemEditorEvent : public Event {
public:
	ItemEditorEvent( Object* source ) : 
		Event( source ){
	}

	ItemEditorEvent( Object* source, void* dataToValidate ) : 
		Event( source, ITEMEDITOR_VALIDATION ){
		setUserData( dataToValidate );
	}
	ItemEditorEvent( Object* source, const unsigned long& eventType  ) : 
		Event( source, eventType ){
	}

	virtual ~ItemEditorEvent(){
	 
	}

	void fireInvalidStateException(){
		throw InvalidStateException(MAKE_ERROR_MSG(BAD_ITEMSTATE_EXCEPTION), __LINE__);
	}

	TableCellItem* getItemBeingEdited(){
		return m_itemBeingEdited;
	}

	void setItemBeingEdited( TableCellItem* itemBeingEdited ){
		m_itemBeingEdited = itemBeingEdited;
	}

private:
	TableCellItem* m_itemBeingEdited;
};

}; //end of namespace VCF

#endif //_ITEMEDITOREVENT_H__