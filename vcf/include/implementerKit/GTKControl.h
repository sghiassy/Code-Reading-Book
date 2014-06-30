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

#ifndef _GTKCONTROL_H__
#define _GTKCONTROL_H__

#include <gtk/gtk.h>
#include "ControlPeer.h"
#include "GTKObject.h"

namespace VCF {

class Control;

class GTKControl : public GTKObject, public ControlPeer{

public:
	GTKControl();

	virtual ~GTKControl();

	/**
     * returns a ID that identifies this control. On Win32 this corresponds to an HWND. 
     */
    virtual long getHandleID();

    /**
     * returns a text associated with the component. This usually gets used in the Control::getCaption() method. 
     */
    virtual String getText();

    /**
     * sets the text for the widget 
     */
    virtual void setText( const String& text );

    /**
     * sets the bounds for the component. Bounds are specified in the coordinate system of the componenents parent. 
     */
    virtual void setBounds( Rect* rect );

    /**
     * returns the bounds of the component in the coordinate system of the parent. 
     */
    virtual Rect* getBounds();

    /**
     * shows or hides the component. 
     * This does NOT close the component (if invoked on a frame based component ). 
     */
    virtual void setVisible( const bool& visible );

    /**
     * returns wether or not the component is currently visible. 
     */
    virtual bool getVisible();

    /**
     * returns a bit-masked unsigned long that contains style constants. 
     *  These style constants are defined in the VCF, and must 
     * be translated to the particular windowing system being used. 
     */
    virtual unsigned long getStyleMask();

    /**
     * sets the current style mask.
     *  Should cause a repaint of the component, if neccessary. 
     */
    virtual void setStyleMask( const unsigned long& styleMask );

    /**
     * returns the component that this Peer is attached to. 
     */
    virtual Control* getControl();

    /**
     * attahces the Peer to a particular component. This should only be done once. 
     */
    virtual void setControl( Control* component );

    virtual void getCursor();

    virtual void setCursor();    
	
	virtual void setParent( Control* parent );

	virtual Control* getParent();

	virtual bool isFocused();

	virtual void setFocus( const bool& focused );

	virtual bool isEnabled();

	virtual void setEnabled( const bool& enabled );

	virtual void setFont( Font* font );

	virtual void repaint( Rect* repaintRect=NULL );

	/**
	*this keeps the mouse events being sent to this control, even is the 
	*mouse leaves the physical bounds of the control
	*/
	virtual void keepMouseEvents();

	/**
	*releases mouse events - goes back to normal event handling
	*/
	virtual void releaseMouseEvents();

private:
	Control* m_control;

	GtkWidget* m_gtkWidget;
};

}; //end of namespace VCF

#endif //_GTKCONTROL_H__