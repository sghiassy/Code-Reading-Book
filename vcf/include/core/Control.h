/**
*Visual Control Framework for C++
*
*Copyright © 2000 Jim Crafton
*
*This program is free software; you can redistribute it and/or
*modify it under the terms of the GNU General Public License
*as published by the Free Software Foundation; either version 2
*of the License, or (at your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*
*Contact Jim Crafton at ddiego@one.net for more information about the 
*Visual Control Framework
*/ 
// Control.h

#ifndef _CONTROL_H__
#define _CONTROL_H__


namespace VCF {


class Border;

class Font;

class ControlPeer;

class GraphicsContext;

class ControlGraphicsContext;

class PopupMenu;

class Scrollable;

class Cursor;

class Model;



#define CONTROL_CLASSID		"B91B1828-3639-4bcf-9882-342F16C90E21"


/**
 * The base class for all widgets in the Visual Component Framework.
 *In addition it implements the View interface. This means that the 
 *Control is itself a specialized form of a View, though it may not have 
 *have a model, and can hold an external view, separate from itself.
 */
class APPKIT_API Control : public Component, public AbstractView {
public:
	//Anchor delta enums
	enum {
		ANCHOR_DTOP = 0,
		ANCHOR_DLEFT,
		ANCHOR_DBOTTOM,
		ANCHOR_DRIGHT
	};

	BEGIN_ABSTRACT_CLASSINFO(Control, "VCF::Control", "VCF::Component", CONTROL_CLASSID);
	PROPERTY( double, "left", Control::getLeft, Control::setLeft, PROP_DOUBLE );
	PROPERTY( double, "top", Control::getTop, Control::setTop, PROP_DOUBLE );
	PROPERTY( bool, "visible", Control::getVisible, Control::setVisible, PROP_BOOL );	
	PROPERTY( double, "width", Control::getWidth, Control::setWidth, PROP_DOUBLE );
	PROPERTY( double, "height", Control::getHeight, Control::setHeight, PROP_DOUBLE );
	PROPERTY( bool, "enabled", Control::isEnabled, Control::setEnabled, PROP_BOOL );
	PROPERTY( bool, "doubleBuffered", Control::isDoubleBuffered, Control::setDoubleBuffered, PROP_BOOL );
	PROPERTY( bool, "useParentFont", Control::useParentFont, Control::setUseParentFont, PROP_BOOL );
	PROPERTY( bool, "focused", Control::isFocused, Control::setFocus, PROP_BOOL );	
	PROPERTY( bool, "autoStartDragDrop", Control::getAutoStartDragDrop, Control::setAutoStartDragDrop, PROP_BOOL );
	PROPERTY( String, "whatThisHelpString", Control::getWhatThisHelpString, Control::setWhatThisHelpString, PROP_STRING );
	PROPERTY( String, "toolTip", Control::getToolTip, Control::setToolTip, PROP_STRING );
	OBJECT_PROPERTY( Border, "border", Control::getBorder, Control::setBorder );	
	OBJECT_PROPERTY( Color, "color", Control::getColor, Control::setColor );
	OBJECT_PROPERTY( Font, "font", Control::getFont, Control::setFont );
	OBJECT_PROPERTY( PopupMenu, "popupMenu", Control::getPopupMenu, Control::setPopupMenu );
	LABELED_ENUM_PROPERTY( AlignmentType, "alignment", Control::getAlignment, Control::setAlignment, 
						   ALIGN_NONE, ALIGN_CLIENT, 6, AlignmentTypeNames);	

	PROPERTY_TYPEDEF( long, "anchor", Control::getAnchor, Control::setAnchor, PROP_LONG, "AnchorTypeMask"  );		

	EVENT("VCF::ControlEventHandler", "VCF::ControlEvent", "ControlSized" );
	EVENT("VCF::ControlEventHandler", "VCF::ControlEvent", "ControlPositioned" );
	EVENT("VCF::ControlEventHandler", "VCF::ControlEvent", "ControlParentChanged" );	

	EVENT("VCF::MouseEventHandler", "VCF::MouseEvent", "MouseDoubleClicked" );
	EVENT("VCF::MouseEventHandler", "VCF::MouseEvent", "MouseClicked" );
	EVENT("VCF::MouseEventHandler", "VCF::MouseEvent", "MouseMove" );
	EVENT("VCF::MouseEventHandler", "VCF::MouseEvent", "MouseUp" );
	EVENT("VCF::MouseEventHandler", "VCF::MouseEvent", "MouseDown" );

	EVENT("VCF::KeyboardEventHandler", "VCF::KeyboardEvent", "KeyPressed"  );
	EVENT("VCF::KeyboardEventHandler", "VCF::KeyboardEvent", "KeyDown"  );
	EVENT("VCF::KeyboardEventHandler", "VCF::KeyboardEvent", "KeyUp"  );	

	EVENT("VCF::WhatsThisHelpEventHandler", "VCF::WhatsThisHelpEvent", "ControlHelpRequested" );

	EVENT("VCF::HelpEventHandler", "VCF::HelpEvent", "HelpRequested" );

	EVENT("VCF::FocusEventHandler", "VCF::FocusEvent", "FocusLost"  );
	EVENT("VCF::FocusEventHandler", "VCF::FocusEvent", "FocusGained"  );

	EVENT("VCF::ToolTipEventHandler", "VCF::ToolTipEvent", "ToolTipRequested"  );
	EVENT("VCF::ToolTipEventHandler", "VCF::ToolTipEvent", "ToolTip"  );
	
	END_CLASSINFO(Control);

	Control();	

	virtual ~Control();

	//declare the event handler lists
	EVENT_HANDLER_LIST(ControlSized);	
	EVENT_HANDLER_LIST(ControlPositioned);	
	EVENT_HANDLER_LIST(ControlParentChanged);	
	EVENT_HANDLER_LIST(MouseDoubleClicked);	
	EVENT_HANDLER_LIST(MouseClicked);	
	EVENT_HANDLER_LIST(MouseMove);	
	EVENT_HANDLER_LIST(MouseUp);	
	EVENT_HANDLER_LIST(MouseDown);	
	EVENT_HANDLER_LIST(KeyPressed);	
	EVENT_HANDLER_LIST(KeyDown);	
	EVENT_HANDLER_LIST(KeyUp);	
	EVENT_HANDLER_LIST(ControlHelpRequested);	
	EVENT_HANDLER_LIST(HelpRequested);	
	EVENT_HANDLER_LIST(FocusLost);	
	EVENT_HANDLER_LIST(FocusGained);	
	EVENT_HANDLER_LIST(ToolTipRequested);	
	EVENT_HANDLER_LIST(ToolTip);	

	ADD_EVENT(ControlSized);	
	ADD_EVENT(ControlPositioned);	
	ADD_EVENT(ControlParentChanged);	
	ADD_EVENT(MouseDoubleClicked);	
	ADD_EVENT(MouseClicked);	
	ADD_EVENT(MouseMove);	
	ADD_EVENT(MouseUp);	
	ADD_EVENT(MouseDown);	
	ADD_EVENT(KeyPressed);	
	ADD_EVENT(KeyDown);	
	ADD_EVENT(KeyUp);	
	ADD_EVENT(ControlHelpRequested);	
	ADD_EVENT(HelpRequested);	
	ADD_EVENT(FocusLost);	
	ADD_EVENT(FocusGained);	
	ADD_EVENT(ToolTipRequested);	
	ADD_EVENT(ToolTip);	
	
	REMOVE_EVENT(ControlSized);	
	REMOVE_EVENT(ControlPositioned);	
	REMOVE_EVENT(ControlParentChanged);	
	REMOVE_EVENT(MouseDoubleClicked);	
	REMOVE_EVENT(MouseClicked);	
	REMOVE_EVENT(MouseMove);	
	REMOVE_EVENT(MouseUp);	
	REMOVE_EVENT(MouseDown);	
	REMOVE_EVENT(KeyPressed);	
	REMOVE_EVENT(KeyDown);	
	REMOVE_EVENT(KeyUp);	
	REMOVE_EVENT(ControlHelpRequested);	
	REMOVE_EVENT(HelpRequested);	
	REMOVE_EVENT(FocusLost);	
	REMOVE_EVENT(FocusGained);	
	REMOVE_EVENT(ToolTipRequested);	
	REMOVE_EVENT(ToolTip);	

	/**
	*macro adds event notifier methods. These are equivalent to the fireXXX methods
	*of java. The macro expands to code that automatically calls all the listeners and
	*fires off the appropriate listener method.
	*/
	/**
	*Control events
	*/
	FIRE_EVENT(ControlSized,ControlEvent);	
	FIRE_EVENT(ControlPositioned,ControlEvent);	
	FIRE_EVENT(ControlParentChanged,ControlEvent);	
	
	/**
	*Mouse events
	*/
	FIRE_EVENT(MouseDoubleClicked,MouseEvent);	
	FIRE_EVENT(MouseClicked,MouseEvent);	
	FIRE_EVENT(MouseMove,MouseEvent);	
	FIRE_EVENT(MouseUp,MouseEvent);	
	FIRE_EVENT(MouseDown,MouseEvent);	
	
	/**
	*Keyboard events
	*/
	FIRE_EVENT(KeyPressed,KeyboardEvent);	
	FIRE_EVENT(KeyDown,KeyboardEvent);	
	FIRE_EVENT(KeyUp,KeyboardEvent);	
	
	/**
	*WhatThisHelp events
	*/
	FIRE_EVENT(ControlHelpRequested,WhatsThisHelpEvent);	
	
	/**
	*Help events
	*/
	FIRE_EVENT(HelpRequested,HelpEvent);	

	/**
	*Focus events
	*/
	FIRE_EVENT(FocusLost,FocusEvent);	
	FIRE_EVENT(FocusGained,FocusEvent);	

	/**
	*ToolTip events
	*/
	FIRE_EVENT(ToolTipRequested,ToolTipEvent);	
	FIRE_EVENT(ToolTip,ToolTipEvent);	
	
	
	

	Border* getBorder();

	void setBorder( Border* border );

    /**
     * returns the bounds in parent coordinates of the Control. The Control derived class must call it's Peer's getBounds() method 
     */
    virtual Rect* getBounds() throw( InvalidPeer );
	
	virtual Rect* getClientBounds( const bool& includeBorder = true ) throw( InvalidPeer );

    virtual double getLeft() throw( InvalidPeer );

    virtual double getWidth() throw( InvalidPeer );

    virtual double getTop() throw( InvalidPeer );

    virtual double getHeight() throw( InvalidPeer );

    virtual bool getVisible() throw( InvalidPeer );

    AlignmentType getAlignment();

    virtual void setBounds( Rect* rect, const bool& anchorDeltasNeedUpdating=true ) throw( InvalidPeer );

    void setAlignment( const AlignmentType& alignment );

    virtual void setLeft( const double& left ) throw( InvalidPeer );

    virtual void setWidth( const double& width ) throw( InvalidPeer );

    virtual void paint( GraphicsContext * context )=0;

    virtual void setTop( const double& top ) throw( InvalidPeer );

    virtual void setHeight( const double& height ) throw( InvalidPeer );


    virtual void setVisible( const bool& visible ) throw( InvalidPeer );

    virtual void saveToStream( OutputStream * stream );

    virtual void loadFromStream( InputStream * stream );

    /**
     * This gets called by the ControlPeer.
     *  Once inside the event the Control determines the type, and behaves accordingly,
     * as well as notifying any appropriate listeners. 
     */
    void processMouseEvent( MouseEvent* mouseEvent );

    void processKeyboardEvent( KeyboardEvent* keyboardEvent );

    void processControlEvent( ControlEvent* ControlEvent );

    /**
     *if m_autoStartDragDrop is true then this method checks to see if the Controls dragging is with 
	 *the windowing system's tolernace for a drag drop operation to start. other wise it returns false. 
     *If  m_autoStartDragDrop is false then it returns false unless some derived class overides the functionality. 
     */
    virtual bool canBeginDragDrop();

    void setAutoStartDragDrop(const bool& canAutoStartDragDrop);

	bool getAutoStartDragDrop();

    virtual void beginDragDrop();
	
	virtual ControlPeer* getPeer();

	virtual void setParent( Control* parent ) throw( InvalidPeer );
	virtual Control* getParent() throw( InvalidPeer );

	bool isFocused();
	void setFocus( const bool& focused );

	bool isEnabled();
	void setEnabled( const bool& enabled );
	
	virtual void mouseDown( MouseEvent* event );

	virtual void mouseMove( MouseEvent* event );

	virtual void mouseUp( MouseEvent* event );	

	virtual void mouseClick(  MouseEvent* event );
	
	virtual void mouseDblClick(  MouseEvent* event );

	virtual void keyDown( KeyboardEvent* event );
	virtual void keyPressed( KeyboardEvent* event );
	virtual void keyUp( KeyboardEvent* event );

	void translateToParent( Point* point );
	void translateToLocal( Point* point );

	/**
	*identifies the control as a lightweight control. Lightweight control's
	*do not take up windowing system resources, instead, they rely on all events
	*and paint notification be sent to them via their parents. The default return for 
	*this is false, so only those controls specifically actign as light weight Controls
	*need to return true
	*/
	virtual bool isLightWeight();

	/**
	*Returns the first parent of the Control that is a heavweight Control, i.e it's isLighweight()
	*method return false.
	*/
	virtual Control* getHeavyweightParent();

	virtual String toString();

	Color* getColor();

	void setColor( Color* color );

	virtual void beforeDestroy( ControlEvent* event );
	
	virtual void afterCreate( ComponentEvent* event );
	
	Font* getFont();

	void setFont( Font* font );

	bool useParentFont();

	void setUseParentFont( const bool& useParentFont );

	void repaint( Rect* repaintRect=NULL );

	/**
	*is this component double buffered
	*/
	bool isDoubleBuffered();

	void setDoubleBuffered( const bool& doubleBuffered );

	/**
	*this keeps the mouse events being sent to this control, even is the 
	*mouse leaves the physical bounds of the control
	*/
	void keepMouseEvents();

	/**
	*releases mouse events - goes back to normal event handling
	*/
	void releaseMouseEvents();

	/**
	*return a pointer to the graphics context of the control. 
	*/
	GraphicsContext* getContext();

	/**
	*returns the Control's PopupMenu, if any
	*/
	PopupMenu* getPopupMenu();

	void setPopupMenu( PopupMenu* popupMenu );

	View* getView();

	void setView( View* view );

	virtual double getPreferredWidth(){
		return 100.0;	
	};

	virtual double getPreferredHeight() {
		return 25.0;
	}
	
	virtual void setPreferredWidth( const double& width ){};

	virtual void setPreferredHeight( const double& height ){};

	/**
	*returns an object implementing the Scrollable interface
	*The default value is NULL, menaing the control does not support 
	*scrolling behaviour
	*/
	Scrollable* getScrollable() {
		return m_scrollable;
	}

	void setScrollable( Scrollable* scrollable );

	String getWhatThisHelpString() {
		return m_whatThisHelpString;
	}

	void setWhatThisHelpString( const String& whatThisHelpString ) {
		m_whatThisHelpString = whatThisHelpString;
	}

	void processWhatsThisHelpEvent();

	String getToolTip() {
		return m_toolTip;
	}

	void setToolTip( const String& tooltip );

	long getCursorID() {
		return m_cursorID;
	}

	void setCursorID( const long& cursorID );

	long getAnchor() {
		return m_anchor;
	}

	void setAnchor( const long& anchor );

	/**
	*returns the current delta for each of the anchors.
	*There will be a delta for the left, the top, the right,
	*and the bottom sides. These deltas are recalculated whenever
	*the controls bounds are set with the anchorDeltasNeedUpdating
	*param set to true. 
	*/
	float* getAnchorDeltas() {
		return m_anchorDeltas;
	}

	static Control* getCapturedMouseControl();
	
	static void setCapturedMouseControl( Control* control );
protected:
	ControlPeer* m_Peer;
	ControlGraphicsContext* m_context;

	void updateAnchorDeltas();

    Control * m_parent;
    AlignmentType m_aligment;
	long m_anchor;
    Rect* m_bounds;
	Rect* m_clientBounds;
	Border* m_border;
	Color* m_color;
	Font* m_font;
	View* m_view;
	bool m_useParentFont;
	bool m_doubleBuffered;
	bool m_hasMouseCapture;	
	bool m_autoStartDragDrop;
	PopupMenu* m_popupMenu;
	Scrollable* m_scrollable;
	String m_whatThisHelpString;
	String m_toolTip;
	static Control* capturedMouseControl;
	long m_cursorID;
	Cursor* m_cursor;
	float m_anchorDeltas[4]; 
};


};

#endif //_CONTROL_H__


