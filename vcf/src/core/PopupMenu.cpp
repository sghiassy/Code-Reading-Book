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
#include "ApplicationKit.h"



using namespace VCF;

PopupMenu::PopupMenu( Control* control )
{
	UIToolkit* toolkit = UIToolkit::getDefaultUIToolkit();
	if ( NULL != toolkit ){
		m_popupPeer = toolkit->createPopupMenuPeer( this );
	}
	else {
		//throw exception
	}

	if ( NULL == m_popupPeer ){
		//throw exception
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}

	if ( NULL != control )	{
		control->addComponent( this );
	}
	this->setControl( control );
}

PopupMenu::~PopupMenu()
{
	if ( NULL != m_popupPeer ){
		delete m_popupPeer;
	}

	m_popupPeer = NULL;
}

void PopupMenu::popup( Point* pt )
{
	if ( NULL == m_popupPeer ){	
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	
	m_popupPeer->popup( pt );		
}

void PopupMenu::setControl( Control* control )
{
	if ( NULL == m_popupPeer ){	
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	
	m_popupPeer->setControl( control );	
}
