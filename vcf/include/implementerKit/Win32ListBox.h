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

// Win32ListBox.h

#ifndef _WIN32LISTBOX_H__
#define _WIN32LISTBOX_H__



namespace VCF
{

class APPKIT_API Win32ListBox : public AbstractWin32Component, public ListBoxPeer {

public:
	Win32ListBox( ListBoxControl* listboxControl );
	
	virtual ~Win32ListBox();	

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

	/**
     * on Win32 see LB_SETCARETINDEX 
     */
    virtual void setFocusedItem(ListItem * item);

    /**
     * on Win32 see LB_RESETCONTENT 
     */
    virtual void clear();

    /**
     * on Win32 see LB_SELITEMRANGEEX 
     */
    virtual void rangeSelect(const bool & isSelected, ListItem * first, ListItem * last);

    /**
     * on Win32 see LB_SETCURSEL 
     */
    virtual void selectItem(ListItem * item);

    /**
     * on Win32 see LB_GETITEMRECT 
     */
    virtual Rect* getItemRect();

    /**
     * on Win32 see LB_ADDSTRING 
     */
    virtual void addItem(ListItem * item);

    virtual void insertItem( const unsigned long& index, ListItem * item);

	virtual void removeItem( ListItem* item );

    /**
     * on Win32 see LB_GETCURSEL 
     */
    virtual bool isItemSelected(ListItem* item);

    /**
     * returns NULL if the point passed in is not over a list item 
     */
    virtual ListItem* isPtOverItem(Point* point);    

    /**
     * returns a the item that has the focus in a multiple selection range.
     * On Win32 see LB_GETCARETINDEX 
     */
    virtual ListItem* getFocusedItem();    

    /**
     * on  Win32 see LB_SETITEMHEIGHT 
     */
    virtual void setItemHeight( const unsigned long& itemHeight );

    /**
     * on Win32 see LB_GETCURSEL 
     */
    virtual ListItem* getSelectedItem();

    /**
     * on Win32 see LB_GETITEMHEIGHT 
     */
    virtual unsigned long getItemHeight();

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
	ListBoxControl* m_listboxControl;
	WNDPROC m_oldListboxWndProc;
	Rect m_itemRect;
	HBRUSH m_backgroundBrush;
};

};

#endif // !defined(AFX_WIN32LISTBOX_H__66DBACB3_0C30_11D4_8E4E_00207811CFAB__INCLUDED_)
