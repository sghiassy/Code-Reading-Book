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

// Win32CustomControl.h

#ifndef _WIN32CUSTOMCONTROL_H__
#define _WIN32CUSTOMCONTROL_H__


namespace VCF
{

class APPKIT_API Win32CustomControl : public Object, public ControlPeer  
{
public:
	Win32CustomControl( Control* component );
	virtual ~Win32CustomControl();
	
	static bool m_windowClassRegistered;

	/**
     * returns a ID that identifies this control. On Win32 this corresponds to an HWND. 
     */
    virtual long getHandleID();

    /**
     * returns a text associated with the component. This usually gets used in the Component::getCaption() method. 
     */
    virtual VCF::String getText();

    /**
     * sets the text for the widget 
     */
    virtual void setText( const VCF::String& text );

    /**
     * sets the bounds for the component. Bounds are specified in the coordinate system of the componenents parent. 
     */
    virtual void setBounds( VCF::Rect* rect );

    /**
     * returns the bounds of the component in the coordinate system of the parent. 
     */
    virtual VCF::Rect* getBounds();

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
    virtual VCF::Control* getControl();

    /**
     * attahces the Peer to a particular component. This should only be done once. 
     */
    virtual void setControl( VCF::Control* component );

    virtual void setParent( VCF::Control* parent );

	virtual VCF::Control* getParent();

	virtual bool isFocused();

	virtual void setFocus( const bool& focused );

	virtual bool isEnabled();

	virtual void setEnabled( const bool& enabled );

	virtual void repaint( Rect* repaintRect=NULL );

	virtual void keepMouseEvents();

	virtual void releaseMouseEvents();

private:
	void registerWndClass();

	HWND m_wndHandle;
	VCF::Control* m_component;
	VCF::Rect m_bounds;	
	VCF::Point m_clickPt;	
};

};

#endif
