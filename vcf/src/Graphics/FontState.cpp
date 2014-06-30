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

//FontState.cpp
#include "GraphicsKit.h"
#include "FontState.h"

using namespace VCF;
 

FontState::FontState()
{
	this->init();
}

FontState::~FontState()
{

}

void FontState::init()
{
	this->m_angle = 0.0;
	this->m_bold = false;
	this->m_height = 12.0;
	this->m_italicized = false;
	this->m_name = "Arial";
	this->m_strikeOut = false;
	this->m_underlined = false;
}

String FontState::toString()
{
	String result = Object::toString();
	char info[256];
	sprintf( info, "angle : %f \nbold : %d \nheight : %f \nitalic : %d \nName : %s \nstrikeOut : %d \nunderlined : %d\n",
		     this->m_angle,this->m_bold,this->m_height,
			 this->m_italicized, this->m_name.c_str(), this->m_strikeOut, this->m_underlined );		     
	result += "\n";
	result += info;
	return result;
}
