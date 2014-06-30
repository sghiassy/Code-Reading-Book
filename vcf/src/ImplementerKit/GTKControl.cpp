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

#include "GTKControl.h"
#include "Control.h"


using namespace VCF;

GTKControl::GTKControl()
{
	m_control = NULL;
	m_gtkWidget = NULL;
}

GTKControl::~GTKControl()
{

}

long GTKControl::getHandleID()
{
	return (long)m_gtkWidget;
}

String GTKControl::getText()
{
	return "";
}

void GTKControl::setText( const String& text )
{

}

void GTKControl::setBounds( Rect* rect )
{

}


Rect* GTKControl::getBounds()
{
	return NULL;
}

void GTKControl::setVisible( const bool& visible )
{

}

bool GTKControl::getVisible()
{
	return false;
}

unsigned long GTKControl::getStyleMask()
{
	return 0;
}

void GTKControl::setStyleMask( const unsigned long& styleMask )
{

}

Control* GTKControl::getControl()
{
	return m_control;
}

void GTKControl::setControl( Control* component )
{
	m_control = component;
}

void GTKControl::getCursor()
{

}

void GTKControl::setCursor()
{

}

void GTKControl::setParent( Control* parent )
{

}

Control* GTKControl::getParent()
{
	return NULL;
}

bool GTKControl::isFocused()
{
	return false;
}

void GTKControl::setFocus( const bool& focused )
{

}

bool GTKControl::isEnabled()
{
	return false;
}

void GTKControl::setEnabled( const bool& enabled )
{

}

void GTKControl::setFont( Font* font )
{

}

void GTKControl::repaint( Rect* repaintRect )
{

}

void GTKControl::keepMouseEvents()
{

}

void GTKControl::releaseMouseEvents()
{

}