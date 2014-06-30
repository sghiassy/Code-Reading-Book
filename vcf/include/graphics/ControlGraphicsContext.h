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

#ifndef _CONTROLGRAPHICSCONTEXT_H__
#define _CONTROLGRAPHICSCONTEXT_H__


namespace VCF {

class Control;

class APPKIT_API ControlGraphicsContext : public GraphicsContext{

public:
	/**
	*Creates a context based on the component. The context is attached to the component
	*for the duration of it's lifetime.
	*/
	ControlGraphicsContext( Control* control );

	virtual ~ControlGraphicsContext();

	void init();

	/**
	*returns the component the Graphics context belongs to. This is almost always a heavweight
	*component. If the context was created from scratch (in other words, not as the result of 
	*a component being created, then it will return NULL.
	*@return Control* the component that owns this context or NULL for memory contexts
	*/
	Control* getOwningControl(){
		return 	m_owningControl;
	};

	void setOwningControl( Control* owningComponent );
private:
	Control* m_owningControl;
};

}; //end of namespace VCF

#endif //_CONTROLGRAPHICSCONTEXT_H__