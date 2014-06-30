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




#ifndef TEXTCONTROL_H
#define TEXTCONTROL_H



namespace VCF{

class TextModel;

class TextPeer;

class DefaultTextModel;

#define TEXTCONTROL_CLASSID			"ED88C09E-26AB-11d4-B539-00C04F0196DA"

class APPKIT_API TextControl : public Control {
public:    
	BEGIN_CLASSINFO(TextControl, "VCF::TextControl", "VCF::Control", TEXTCONTROL_CLASSID )	
	END_CLASSINFO(TextControl)

	TextControl( const bool& multiLineControl=false );

	virtual ~TextControl();

	void init();

	virtual void paint( GraphicsContext * context ){};

    void setTextModel( TextModel * model );

    TextModel* getTextModel();

	unsigned long getCaretPosition() throw (InvalidPeer);
private:    
    TextPeer * m_textPeer;
	TextModel* m_model;
};

};
#endif //TEXTCONTROL_H