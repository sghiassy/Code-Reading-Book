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

#ifndef _DEFAULTTABPAGE_H__
#define _DEFAULTTABPAGE_H__



namespace VCF {

#define DEFAULTTABPAGE_CLASSID	"86F02173-3E7F-11d4-8EA7-00207811CFAB"

class APPKIT_API DefaultTabPage : public TabPage{
public:
	BEGIN_CLASSINFO(DefaultTabPage, "VCF::DefaultTabPage", "VCF::TabPage", DEFAULTTABPAGE_CLASSID)
	END_CLASSINFO(DefaultTabPage)

	DefaultTabPage( Control* component=NULL );

	virtual ~DefaultTabPage();

	EVENT_HANDLER_LIST(ItemPaint);
	EVENT_HANDLER_LIST(ItemChanged);
	EVENT_HANDLER_LIST(ItemSelected);
	EVENT_HANDLER_LIST(ItemAdded);
	EVENT_HANDLER_LIST(ItemDeleted);

	virtual void addItemPaintHandler( EventHandler* handler ){
		ADD_EVENT_IMPL(ItemPaint) 
	}

	virtual void addItemChangedHandler( EventHandler* handler ){
		ADD_EVENT_IMPL(ItemChanged) 
	}

	virtual void addItemSelectedHandler( EventHandler* handler ){
		ADD_EVENT_IMPL(ItemSelected) 
	}

	virtual void addItemAddedHandler( EventHandler* handler ){
		ADD_EVENT_IMPL(ItemAdded) 
	}

	virtual void addItemDeletedHandler( EventHandler* handler ){
		ADD_EVENT_IMPL(ItemDeleted) 
	}
    
	virtual void removeItemPaintHandler( EventHandler* handler ){
		REMOVE_EVENT_IMPL(ItemPaint) 
	}

	virtual void removeItemChangedHandler( EventHandler* handler ){
		REMOVE_EVENT_IMPL(ItemChanged) 
	}

	virtual void removeItemSelectedHandler( EventHandler* handler ){
		REMOVE_EVENT_IMPL(ItemSelected) 
	}

	virtual void removeItemAddedHandler( EventHandler* handler ){
		REMOVE_EVENT_IMPL(ItemAdded) 
	}

	virtual void removeItemDeletedHandler( EventHandler* handler ){
		REMOVE_EVENT_IMPL(ItemDeleted) 
	}

	FIRE_EVENT(ItemPaint,ItemEvent)
	FIRE_EVENT(ItemChanged,ItemEvent)
	FIRE_EVENT(ItemSelected,ItemEvent)
	FIRE_EVENT(ItemAdded,ItemEvent)
	FIRE_EVENT(ItemDeleted,ItemEvent)

	virtual bool containsPoint( Point * pt );

	virtual Image* getImage();

    virtual unsigned long getIndex();

	virtual void setIndex( const unsigned long& index );

    virtual void* getData();

	virtual void setData( void* data );	

	virtual Model* getModel();

	virtual void setModel( Model* model );

	virtual void setPageName( const String& name );

	virtual String getPageName();

	virtual Control* getPageComponent();

	virtual void setPageComponent( Control* component );

	virtual bool isSelected();

	virtual void setSelected( const bool& selected );

	virtual void paint( GraphicsContext* context, Rect* paintRect );

	virtual ulong32 getPreferredHeight();

	virtual Rect* getBounds() {
		return &m_bounds;
	}

	virtual Control* getControl() {
		return m_owningControl;
	}


	virtual void setControl( Control* control ) {
		m_owningControl = control;
	}

	virtual ulong32 getImageIndex() {
		return m_imageIndex;
	}

	virtual void setImageIndex( const ulong32& imageIndex );

	virtual bool canPaint() {
		return true;	
	}
private:
	Model* m_model;
	Control* m_owningControl;
	ulong32 m_index;
	String m_pageName;
	Control* m_component;
	bool m_selected;
	ulong32 m_preferredHeight;
	Rect m_bounds;	
	ulong32 m_imageIndex;
};

}; //end of namespace VCF

#endif //_DEFAULTTABPAGE_H__