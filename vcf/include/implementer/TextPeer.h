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



#ifndef TEXTPEER_H
#define TEXTPEER_H




namespace VCF{

/** @interface */

/**
*A interface for supporting text controls
*This results in a native text control widget being 
*created, with all painting responsibilities being 
*controlled by the native widget. This means that
*some advanced graphics features and Font capabilities
*may not appear in the control. FOr example, setting 
*the Font's setShear() method will have no effect
*on the widget's rendering of it's text.
*@version 1.0
*author Jim Crafton
*/
class APPKIT_API TextPeer : public ControlPeer {
public:    
    virtual void setRightMargin( const double & rightMargin ) = 0;
    
	virtual void setLeftMargin( const double & leftMargin ) = 0;

    virtual unsigned long getLineCount() = 0;

    virtual void getCurrentLinePosition() = 0;

    virtual double getLeftMargin() = 0;

    virtual double getRightMargin() = 0;

    virtual Point* getPositionFromCharIndex( const unsigned long& index ) = 0;

    virtual unsigned long getCharIndexFromPosition( Point* point ) = 0;

	/**
	*returns the current caret position with in the text control
	*this is specified by a zero based number representing the 
	*insertion point with the text control's text (stored in the text
	*control's Model).
	*@return long the index of the current insertion point in the Model's text
	*/
	virtual unsigned long getCaretPosition() = 0;
};

};

#endif //TEXTPEER_H