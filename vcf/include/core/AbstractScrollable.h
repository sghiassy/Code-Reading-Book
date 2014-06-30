//AbstractScrollable.h

/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/

#ifndef _ABSTRACTSCROLLABLE_H__
#define _ABSTRACTSCROLLABLE_H__




namespace VCF  {

class ScrollPeer;

/**
*Class AbstractScrollable documentation
*/
class APPKIT_API AbstractScrollable : public Scrollable { 
public:
	class ControlResizeHandler : public ObjectWithEvents {
	public:
		ControlResizeHandler( AbstractScrollable* scrollable ); 
		
		virtual ~ControlResizeHandler(){}; 
		
		void controlResized( ControlEvent* event ); 
	
	private:
		  AbstractScrollable* m_scrollable;
	};
	
	AbstractScrollable();

	virtual ~AbstractScrollable();	

	friend class ControlResizeHandler;

	virtual void setHasVerticalScrollbar( const bool& hasVerticalScrollbar ) {
		m_hasVertScrollbar = hasVerticalScrollbar;
		recalcScrollPositions();
	}

	virtual void setHasHorizontalScrollbar( const bool& hasHorizontalScrollbar ) {
		m_hasHorzScrollbar = hasHorizontalScrollbar;
		recalcScrollPositions();
	}

	virtual bool hasVerticalScrollBar() {
		return m_hasVertScrollbar;	
	}

	virtual bool hasHorizontalScrollBar() {
		return m_hasHorzScrollbar;
	}
	
	virtual double getVirtualViewHeight() {
		return m_virtualViewHeight;
	}

	virtual double getVirtualViewWidth() {
		return m_virtualViewWidth;
	}

	virtual void setVirtualViewHeight( const double& virtualViewHeight ) {
		m_virtualViewHeight = virtualViewHeight;
		recalcScrollPositions();
	}
	
	virtual void setVirtualViewWidth( const double& virtualViewWidth ) {
		m_virtualViewWidth = virtualViewWidth;
		recalcScrollPositions();
	}

	virtual void recalcScrollPositions();

	virtual Control* getScrollableControl() {
		return m_scrollableControl;
	}

	virtual void setScrollableControl( Control* scrollableControl );
	
	virtual void setVerticalPosition( const double& vertPosition );

	virtual void setHorizontalPosition( const double& horzPosition );

	virtual double getVerticalPosition() {
		return m_vertPosition;
	}
	
	virtual double getHorizontalPosition() {
		return m_horzPosition;
	}
	
	virtual double getVerticalTopScrollSpace() {
		return m_topScrollSpace;
	}
	
	virtual double getVerticalBottomScrollSpace() {
		return m_bottomScrollSpace;
	}

	virtual void setVerticalTopScrollSpace( const double& topScrollSpace );

	virtual void setVerticalBottomScrollSpace( const double& bottomScrollSpace );

	virtual double getHorizontalLeftScrollSpace() {
		return m_leftScrollSpace;
	}
	
	virtual double getHorizontalRightScrollSpace() {
		return m_rightScrollSpace;
	}

	virtual void setHorizontalLeftScrollSpace( const double& leftScrollSpace );

	virtual void setHorizontalRightScrollSpace( const double& rightScrollSpace );
protected:

	void onControlResized( ControlEvent* event );

	ControlResizeHandler* m_resizeHandler;

	ScrollPeer* m_scrollPeer;
	Control* m_scrollableControl;
	double m_virtualViewHeight;
	double m_virtualViewWidth;
	bool m_hasVertScrollbar;
	bool m_hasHorzScrollbar;
	double m_vertPosition;
	double m_horzPosition;
	double m_topScrollSpace;
	double m_bottomScrollSpace;
	double m_leftScrollSpace;
	double m_rightScrollSpace;
};


}; //end of namespace VCF

#endif //_ABSTRACTSCROLLABLE_H__


