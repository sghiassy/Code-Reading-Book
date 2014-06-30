/**
*Visual Component Framework for C++
*
*Copyright © 2000 Jim Crafton
*
*This program is free software; you can redistribute it and/or
*modify it under the terms of the GNU General Public License
*as published by the Free Software Foundation; either version 2
*of the License, or (at your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*
*Contact Jim Crafton at ddiego@one.net for more information about the 
*Visual Component Framework
*/
//TabModel.h

#ifndef _TABMODEL_H__
#define _TABMODEL_H__


namespace VCF {

#define TABMODEL_CLASSID	"4A9D66D7-3B26-11d4-B54C-00C04F0196DA"

class TabPage;

class APPKIT_API TabModel : public Model {
public:
	BEGIN_ABSTRACT_CLASSINFO(TabModel, "VCF::TabModel", "VCF::Model", TABMODEL_CLASSID )
	EVENT("VCF::TabModelEventHandler", "VCF::TabModelEvent", "TabPageAdded")
	EVENT("VCF::TabModelEventHandler", "VCF::TabModelEvent", "TabPageRemoved")
	EVENT("VCF::TabModelEventHandler", "VCF::TabModelEvent", "TabPageSelected")
	END_CLASSINFO(TabModel)

	TabModel(){
		
	};

	virtual ~TabModel(){};

	virtual void addTabPageAddedHandler( EventHandler* handler ) = 0;
    
	virtual void removeTabPageAddedHandler( EventHandler* handler ) = 0;

	virtual void addTabPageRemovedHandler( EventHandler* handler ) = 0;
    
	virtual void removeTabPageRemovedHandler( EventHandler* handler ) = 0;

	virtual void addTabPageSelectedHandler( EventHandler* handler ) = 0;
    
	virtual void removeTabPageSelectedHandler( EventHandler* handler ) = 0;
	

	virtual void addTabPage( TabPage* page ) = 0;

	virtual void insertTabPage( const ulong32& index, TabPage* page ) = 0;
	
	virtual void deleteTabPage( TabPage* page ) = 0;

	virtual void deleteTabPage( const ulong32& index ) = 0;

	virtual void clearTabPages() = 0;
	
	virtual TabPage* getPageAt( const ulong32& index ) = 0;

	virtual TabPage* getSelectedPage() = 0;

	virtual void setSelectedPage( TabPage* page ) = 0;

	virtual void setSelectedPage( const ulong32& index ) = 0;

	virtual Enumerator<TabPage*>* getPages() = 0;
};

};

#endif