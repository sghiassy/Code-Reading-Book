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

#ifndef _PROPERTYCELLITEMEDITOR_H__
#define _PROPERTYCELLITEMEDITOR_H__

#include "BasicTableItemEditor.h"

namespace VCF {

#define PROPERTYCELLITEMEDITOR_CLASSID		"B7D1BC51-7AFC-11d4-8F27-00207811CFAB"

class PropertyCellItem;

class PropertyEditor;

class PropertyCellItemEditor : public BasicTableItemEditor{
public:

	BEGIN_CLASSINFO(PropertyCellItemEditor, "VCF::PropertyCellItemEditor", "VCF::BasicTableItemEditor", PROPERTYCELLITEMEDITOR_CLASSID)
	END_CLASSINFO(PropertyCellItemEditor)

	PropertyCellItemEditor( TableCellItem* itemToEdit=NULL );

	virtual ~PropertyCellItemEditor();

	virtual void setItemToEdit( TableCellItem* itemToEdit );

	virtual void updateItem();

	virtual Control* getEditingControl();

private:
	PropertyCellItem* m_currentPropertyCellItem;
	PropertyEditor* m_currentPropertyEditor;
	//editor is a list of possible component objects to choose from
	bool m_editorIsComponentList;
	
	ComboBoxControl* m_componentListEditorControl;
	
	void findMatchingComponentsFromComponent( Component* srcComponent, const String& componentClassName, std::vector<Component*>& componentList );
	
	void generateMatchingComponentList( const String& componentClassName, std::vector<Component*>& componentList );
};

}; //end of namespace VCF

#endif //_PROPERTYCELLITEMEDITOR_H__