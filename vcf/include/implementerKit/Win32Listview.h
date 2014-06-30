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

// Win32Listview.h

#ifndef _WIN32LISTVIEW_H__
#define _WIN32LISTVIEW_H__


namespace VCF
{

class APPKIT_API Win32Listview : public AbstractWin32Component, public ListviewPeer {
public:	
	Win32Listview( ListViewControl* listviewControl );

	virtual ~Win32Listview();

	/**
     * returns a ID that identifies this control. On Win32 this corresponds to an HWND. 
     */
    virtual long getHandleID(){
		return AbstractWin32Component::getHandleID();	
	};

    /**
     * returns a text associated with the component. This usually gets used in the Component::getCaption() method. 
     */
    virtual VCF::String getText(){
		return AbstractWin32Component::getText();	
	};

    /**
     * sets the text for the widget 
     */
    virtual void setText( const VCF::String& text ){
		AbstractWin32Component::setText( text );		
	};

    /**
     * sets the bounds for the component. Bounds are specified in the coordinate system of the componenents parent. 
     */
    virtual void setBounds( VCF::Rect* rect ){
		AbstractWin32Component::setBounds( rect );	
	};

    /**
     * returns the bounds of the component in the coordinate system of the parent. 
     */
    virtual VCF::Rect* getBounds(){
		return AbstractWin32Component::getBounds();	
	};

    /**
     * shows or hides the component. 
     * This does NOT close the component (if invoked on a frame based component ). 
     */
    virtual void setVisible( const bool& visible ){
		AbstractWin32Component::setVisible( visible );	
	};

    /**
     * returns wether or not the component is currently visible. 
     */
    virtual bool getVisible(){
		return AbstractWin32Component::getVisible();	
	};

    /**
     * returns a bit-masked unsigned long that contains style constants. 
     *  These style constants are defined in the VCF, and must 
     * be translated to the particular windowing system being used. 
     */
    virtual unsigned long getStyleMask(){
		return AbstractWin32Component::getStyleMask();	
	};

    /**
     * sets the current style mask.
     *  Should cause a repaint of the component, if neccessary. 
     */
    virtual void setStyleMask( const unsigned long& styleMask ){
		AbstractWin32Component::setStyleMask( styleMask );	
	};

    /**
     * returns the component that this Peer is attached to. 
     */
    virtual VCF::Control* getControl(){
		return AbstractWin32Component::getControl();	
	};

    /**
     * attahces the Peer to a particular component. This should only be done once. 
     */
    virtual void setControl( VCF::Control* component ){
		AbstractWin32Component::setControl( component );	
	};

    virtual void setParent( VCF::Control* parent ){
		AbstractWin32Component::setParent( parent );	
	};

	virtual VCF::Control* getParent(){
		return AbstractWin32Component::getParent();	
	};

	virtual bool isFocused(){
		return AbstractWin32Component::isFocused();	
	};

	virtual void setFocus( const bool& focused ){
		AbstractWin32Component::setFocus( focused );
	};

	virtual bool isEnabled(){
		return AbstractWin32Component::isEnabled();	
	};

	virtual void setEnabled( const bool& enabled ){
		AbstractWin32Component::setEnabled( enabled );	
	};

	virtual void createParams();

	virtual LRESULT handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc = NULL );

	virtual void addItem( ListItem * item );

    virtual void insertItem( const unsigned long& index, ListItem * item );
	
	virtual void clear();

	virtual void deleteItem( ListItem* item );

	virtual void setFocusedItem(ListItem * item);

	virtual void selectItem(ListItem * item);

	virtual Rect* getItemRect( ListItem* item );

	virtual bool isItemSelected(ListItem* item);

	virtual ListItem* isPtOverItem(Point* point);

	virtual ListItem* getFocusedItem();

	virtual ListItem* getSelectedItem();

	virtual Enumerator<ListItem*>* getSelectedItems();

	virtual void rangeSelect( Rect* selectionRect );

	virtual void addHeaderColumn( const String& columnName );

	virtual void insertHeaderColumn( const unsigned long& index, const String& columnName );

	virtual void deleteHeaderColumn( const unsigned long& index );

	virtual IconStyleType getIconStyle();

	virtual void setIconStyle( const IconStyleType& iconStyle );

	virtual bool getAllowsMultiSelect();

	virtual void setAllowsMultiSelect( const bool& allowsMultiSelect );

	virtual IconAlignType getIconAlignment();

	virtual void setIconAlignment( const IconAlignType& iconAlignType );

	virtual bool getAllowLabelEditing();

	virtual void setAllowLabelEditing( const bool& allowLabelEditing );

	void onItemPaint( ItemEvent* event );

    void onItemChanged( ItemEvent* event );

    void onItemSelected( ItemEvent* event );

	void onItemAdded( ItemEvent* event );
	
	void onItemDeleted( ItemEvent* event );

	virtual void setFont( Font* font ){
		AbstractWin32Component::setFont( font );	
	};

	virtual void repaint( Rect* repaintRect=NULL ){
		AbstractWin32Component::repaint(repaintRect);
	};
	
	virtual void keepMouseEvents(){
		AbstractWin32Component::keepMouseEvents();
	};
	
	virtual void releaseMouseEvents(){
		AbstractWin32Component::releaseMouseEvents();
	};

	virtual void setCursor( Cursor* cursor ) {
		AbstractWin32Component::setCursor( cursor );
	}
private:
	ListViewControl* m_listviewControl;
	ItemEventHandler<Win32Listview>* m_itemAddedHandler;
	ItemEventHandler<Win32Listview>* m_itemDeletedHandler;
	ItemEventHandler<Win32Listview>* m_itemChangedHandler;
	ItemEventHandler<Win32Listview>* m_itemSelectedHandler;
	ItemEventHandler<Win32Listview>* m_itemPaintedHandler;

	std::vector<ListItem*> m_selectedItems;
	EnumeratorContainer<std::vector<ListItem*>, ListItem*> m_enumContainer;
	Rect m_itemRect;
	WNDPROC m_oldListviewWndProc;

	IconStyleType translateStyleToIconStyle( const DWORD& wsStyle );
	DWORD translateIconStyleToStyleBit( const IconStyleType& iconStyle );

	IconAlignType translateStyleToIconAlignment( const DWORD& wsStyle );
	DWORD translateIconAlignmentToStyleBit( const IconAlignType& iconAlignment );
	Color m_backColor;
};


};

#endif 
