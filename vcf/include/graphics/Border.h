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

//Border.h

#ifndef _BORDER_H__
#define _BORDER_H__



namespace VCF
{

/*@interface*/

#define BORDER_CLASSID		"3126B221-2819-11d4-B53A-00C04F0196DA"		

class APPKIT_API Border : public Object{
public:
	BEGIN_ABSTRACT_CLASSINFO(Border, "VCF::Border", "VCF::Object", BORDER_CLASSID)
	END_CLASSINFO(Border)

	Border() {
		
	};

	virtual ~Border(){};
	
	/**
	*Paint the border on context
	*/
	virtual void paint( Control* component, GraphicsContext* context ) = 0;

	/**
	*paints a border in the given bounds on the context
	*@param Rect the rectangle that makes up the outer most region
	*of the border
	*@param GraphicsContext the context on which to paint the border
	*/
	virtual void paint( Rect* bounds, GraphicsContext* context ) = 0;

	/**
	*returns a client rect in which contained components may live.
	*Thus if a particular implementation required a 3 pixel
	*border at the edges to draw itself, then it would retrieve the
	*component's border and then reduce it by 3 pixels. This new
	*value is the available area that the component could use to
	*place child components
	*
	*@param Control* - component that the border belongs to 
	*
	*@return Rect* the client rectangle 
	*/
	virtual Rect* getClientRect( Control* component ) = 0;
	
};

};

#endif