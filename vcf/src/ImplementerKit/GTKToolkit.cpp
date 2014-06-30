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


#include "Dialog.h"

#include "PopupMenuPeer.h"
#include "PopupMenu.h"


#include "GTKToolkit.h"
#include "GTKApplication.h"


using namespace VCF;

GTKToolkit::GTKToolkit()
{

}

GTKToolkit::~GTKToolkit()
{

}

Color* GTKToolkit::getSystemColor( const unsigned long& systemColor )
{
	Color* result = NULL;
	std::map<unsigned long,Color*>::iterator found = this->m_systemColors.find( systemColor );
	if ( found != m_systemColors.end() ){
		result = found->second;
	}
	return result;
}

void GTKToolkit::loadSystemColors()
{
	/*
	Color* sysColor = NULL;
	sysColor = new Color( ::GetSysColor( COLOR_BTNSHADOW ) );
	m_systemColors[SYSCOLOR_SHADOW] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_3DFACE ) );
	m_systemColors[SYSCOLOR_FACE] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_3DHILIGHT ) );
	m_systemColors[SYSCOLOR_HIGHLIGHT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_ACTIVECAPTION ) );
	m_systemColors[SYSCOLOR_ACTIVE_CAPTION] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_ACTIVEBORDER ) );
	m_systemColors[SYSCOLOR_ACTIVE_BORDER] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_DESKTOP ) );
	m_systemColors[SYSCOLOR_DESKTOP] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_CAPTIONTEXT ) );
	m_systemColors[SYSCOLOR_CAPTION_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_HIGHLIGHT ) );
	m_systemColors[SYSCOLOR_SELECTION] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_HIGHLIGHTTEXT ) );
	m_systemColors[SYSCOLOR_SELECTION_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INACTIVEBORDER ) );
	m_systemColors[SYSCOLOR_INACTIVE_BORDER] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INACTIVECAPTION ) );
	m_systemColors[SYSCOLOR_INACTIVE_CAPTION] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INFOBK ) );
	m_systemColors[SYSCOLOR_TOOLTIP] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_INFOTEXT ) );
	m_systemColors[SYSCOLOR_TOOLTIP_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_MENU ) );
	m_systemColors[SYSCOLOR_MENU] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_MENUTEXT ) );
	m_systemColors[SYSCOLOR_MENU_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_WINDOW ) );
	m_systemColors[SYSCOLOR_WINDOW] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_WINDOWTEXT ) );
	m_systemColors[SYSCOLOR_WINDOW_TEXT] = sysColor;

	sysColor = new Color( ::GetSysColor( COLOR_WINDOWFRAME ) );
	m_systemColors[SYSCOLOR_WINDOW_FRAME] = sysColor;
	*/
}

ApplicationPeer* GTKToolkit::createApplicationPeer()
{
	return new GTKApplication();
}

TextPeer* GTKToolkit::createTextPeer( TextControl* component, ComponentType componentType)
{
	return NULL;
}

TreePeer* GTKToolkit::createTreePeer( TreeControl* component, ComponentType componentType )
{
	return NULL;
}

ListBoxPeer* GTKToolkit::createListboxPeer( ListBoxControl* component, ComponentType componentType)
{
	return NULL;
}

ListviewPeer* GTKToolkit::createListViewPeer( ListViewControl* component, ComponentType componentType )
{
	return NULL;
}

DialogPeer* GTKToolkit::createDialogPeer( Frame* owner, Dialog* component, ComponentType componentType )
{
	return NULL;
}

DialogPeer* GTKToolkit::createDialogPeer()
{
	return NULL;
}

MenuItemPeer* GTKToolkit::createMenuItemPeer( MenuItem* item )
{
	return NULL;
}

MenuBarPeer* GTKToolkit::createMenuBarPeer( MenuBar* menuBar )
{
	return NULL;
}

ComboBoxPeer* GTKToolkit::createComboBoxPeer( ComboBoxControl* component, ComponentType componentType)
{
	return NULL;
}

ButtonPeer* GTKToolkit::createButtonPeer( CommandButton* component, ComponentType componentType)
{
	return NULL;
}

ContextPeer* GTKToolkit::createContextPeer( Control* component )
{
	return NULL;
}

CommonFileDialogPeer* GTKToolkit::createCommonFileOpenDialogPeer( Control* owner )
{
	return NULL;
}

CommonFileDialogPeer* GTKToolkit::createCommonFileSaveDialogPeer( Control* owner )
{
	return NULL;
}

CommonColorDialogPeer* GTKToolkit::createCommonColorDialogPeer( Control* owner )
{
	return NULL;
}

CommonFolderBrowseDialogPeer* GTKToolkit::createCommonFolderBrowseDialogPeer( Control* owner )
{
	return NULL;
}

DragDropPeer* GTKToolkit::createDragDropPeer()
{
	return NULL;
}

DataObjectPeer* GTKToolkit::createDataObjectPeer()
{
	return NULL;
}

DropTargetPeer* GTKToolkit::createDropTargetPeer()
{
	return NULL;
}

PopupMenuPeer* GTKToolkit::createPopupMenuPeer( PopupMenu* popupMenu )
{
	return NULL;
}

CommonFontDialogPeer* GTKToolkit::createCommonFontDialogPeer( Control* owner )
{
	return NULL;
}
