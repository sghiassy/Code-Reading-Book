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
#include "DefaultMenuItem.h"

using namespace VCF;

MenuBar::MenuBar()
{
	this->init();
	UIToolkit* toolkit = UIToolkit::getDefaultUIToolkit();
	if ( NULL != toolkit ){
		m_Peer = toolkit->createMenuBarPeer( this );
	}
	else {
		//throw exception
	}

	if ( NULL == m_Peer ){
		//throw exception
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
}

MenuBar::~MenuBar()
{
	if ( NULL != m_Peer ){
		delete m_Peer;
		m_Peer = NULL;
	}
}

void MenuBar::init()
{
	m_Peer = NULL;

	this->setRootMenuItem( new DefaultMenuItem() );
	MenuItem* item = this->getRootMenuItem();
	
	ItemEventHandler<MenuBar>* mih = 
		new ItemEventHandler<MenuBar>( this, MenuBar::onMenutItemAdded, "MenuBar::onMenutItemAdded" );

	item->addItemAddedHandler( mih );

	mih = 
		new ItemEventHandler<MenuBar>( this, MenuBar::onMenutItemDeleted, "MenuBar::onMenutItemDeleted" );

	item->addItemDeletedHandler( mih );
}

void MenuBar::onMenutItemAdded( ItemEvent* event )
{
	m_Peer->update();
}

void MenuBar::onMenutItemDeleted( ItemEvent* event )
{
	m_Peer->update();
}

void MenuBar::setFrame( Frame* frame )
{
	if ( NULL == m_Peer ){
		//throw exception
		throw InvalidPeer(MAKE_ERROR_MSG(NO_PEER), __LINE__);
	}
	m_Peer->setFrame( frame );
}