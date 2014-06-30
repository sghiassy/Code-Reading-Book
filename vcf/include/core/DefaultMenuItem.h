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

#ifndef _DEFAULTMENUITEM_H__
#define _DEFAULTMENUITEM_H__


namespace VCF {

class MenuItemPeer;


class APPKIT_API DefaultMenuItem : public MenuItem {

public:
	DefaultMenuItem();	

	DefaultMenuItem( const String& caption, MenuItem* parent=NULL, Menu* menuOwner=NULL );

	virtual ~DefaultMenuItem();

	void init();

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


	EVENT_HANDLER_LIST(MenuItemClicked);
	EVENT_HANDLER_LIST(MenuItemUpdate);

	virtual void addMenuItemClickedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(MenuItemClicked) 
	}

	virtual void removeMenuItemClickedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(MenuItemClicked) 
	}

	virtual void addMenuItemUpdateHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(MenuItemUpdate) 
	}

	virtual void removeMenuItemUpdateHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(MenuItemUpdate) 
	}

	FIRE_EVENT(MenuItemClicked,MenuItemEvent)
	FIRE_EVENT(MenuItemUpdate,MenuItemEvent)

	virtual bool containsPoint( Point * pt );

	virtual Image* getImage();

    virtual unsigned long getIndex();

	virtual void setIndex( const unsigned long& index );

    virtual void* getData();

	virtual void setData( void* data );	

	virtual Model* getModel();

	virtual void setModel( Model* model );

	virtual void paint( GraphicsContext* context, Rect* paintRect );
	
	virtual bool isSelected();

	virtual void setSelected( const bool& selected );

	virtual Enumerator<MenuItem*>* getChildren();	

	virtual void addChild( MenuItem* child );

	virtual void insertChild( const unsigned long& index, MenuItem* child );
	
	virtual void deleteChild( MenuItem* child );

	virtual void deleteChild( const unsigned long& index );

	virtual void clearChildren();

	virtual bool isChecked();

	virtual void setChecked( const bool& checked );

	virtual bool hasParent();

	virtual bool hasChildren();

	virtual Menu* getMenuOwner();

	virtual void setMenuOwner( Menu* menuOwner );

	virtual MenuItem* getParent();

	virtual void setParent( MenuItem* parent );

	virtual MenuItem* getChildAt( const unsigned long& index );

	virtual bool isEnabled();

	virtual void setEnabled( const bool& enabled );

	virtual bool isVisible();

	virtual void setVisible( const bool& visible );

	virtual bool getRadioItem();

	virtual void setRadioItem( const bool& value );

	virtual void setCaption( const String& caption );
	
	virtual String getCaption();

	virtual MenuItemPeer* getPeer();

	virtual bool isSeparator();

	virtual void setSeparator( const bool& separator );

	virtual void click();

	virtual void update();

	virtual Rect* getBounds() {
		return NULL;
	}

	virtual Control* getControl() {
		return NULL;
	}


	virtual void setControl( Control* control ) {}

	
	virtual ulong32 getImageIndex() {
		return m_imageIndex;
	}

	virtual void setImageIndex( const ulong32& imageIndex );

	virtual bool canPaint() {
		return false;	
	}
private:
	MenuItemPeer* m_Peer;
	std::vector<MenuItem*> m_menuItems;
	EnumeratorContainer<std::vector<MenuItem*>, MenuItem*> m_container;
	String m_caption;
	bool m_visible;
	ulong32 m_index;
	bool m_selected;
	void* m_data;
	Menu* m_menuOwner;
	bool m_separator;
	MenuItem* m_parent;
	bool m_radioItem;
	ulong32 m_imageIndex;
	bool m_isEnabled;
};

}; //end of namespace VCF

#endif //_DEFAULTMENUITEM_H__