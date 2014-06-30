//Scrollable.h
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


#ifndef _SCROLLABLE_H__
#define _SCROLLABLE_H__





namespace VCF  {

/**
*Class Scrollable documentation
*/
class Scrollable : public Interface { 
public:
	virtual ~Scrollable(){};

	virtual void setHasVerticalScrollbar( const bool& hasVerticalScrollbar ) = 0;		

	virtual void setHasHorizontalScrollbar( const bool& hashorizontalScrollbar ) = 0;

	virtual bool hasVerticalScrollBar() = 0;

	virtual bool hasHorizontalScrollBar() = 0;
	
	virtual double getVirtualViewHeight() = 0;

	virtual double getVirtualViewWidth() = 0;

	virtual void setVirtualViewHeight( const double& virtualViewHeight ) = 0;
	
	virtual void setVirtualViewWidth( const double& virtualViewWidth ) = 0;

	virtual void recalcScrollPositions() = 0;

	virtual void setVerticalPosition( const double& vertPosition ) = 0;

	virtual void setHorizontalPosition( const double& horzPosition ) = 0;

	virtual double getVerticalPosition() = 0;
	
	virtual double getHorizontalPosition() = 0;

	virtual double getVerticalTopScrollSpace() = 0;
	
	virtual double getVerticalBottomScrollSpace() = 0;

	virtual void setVerticalTopScrollSpace( const double& topScrollSpace ) = 0;

	virtual void setVerticalBottomScrollSpace( const double& bottomScrollSpace ) = 0;

	virtual double getHorizontalLeftScrollSpace() = 0;
	
	virtual double getHorizontalRightScrollSpace() = 0;

	virtual void setHorizontalLeftScrollSpace( const double& leftScrollSpace ) = 0;

	virtual void setHorizontalRightScrollSpace( const double& rightScrollSpace ) = 0;

	virtual Control* getScrollableControl() = 0;

	virtual void setScrollableControl( Control* scrollableControl ) = 0;
};


}; //end of namespace VCF

#endif //_SCROLLABLE_H__


