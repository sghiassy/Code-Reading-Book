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

// Basic3DBorder.h


#ifndef _BASIC3DBORDER_H__
#define _BASIC3DBORDER_H__


#include "Border.h"
#include "Object.h"

namespace VCF
{


#define BASIC3DBORDER_CLASSID			"3126B222-2819-11d4-B53A-00C04F0196DA"

/**
*Draws a basic 3D border around a component. The border is 2 pixels thick
*
*@version 1.0
*@author Jim Crafton
*/
class APPKIT_API Basic3DBorder : public Border {

public:
	BEGIN_CLASSINFO(Basic3DBorder, "VCF::Basic3DBorder", "VCF::Border", BASIC3DBORDER_CLASSID)
	END_CLASSINFO(Basic3DBorder)

	Basic3DBorder();

	virtual ~Basic3DBorder();

	/**
	*Paint the border on context
	*/
	virtual void paint( Control* control, GraphicsContext* context );

	virtual void paint( Rect* bounds, GraphicsContext* context );

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
	virtual Rect* getClientRect( Control* component );

};

};

#endif
