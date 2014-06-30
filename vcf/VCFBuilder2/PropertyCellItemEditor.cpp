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
#include "ApplicationKit.h"
#include "TableItemEditor.h"
#include "PropertyCellItemEditor.h"
#include "PropertyCellItem.h"
#include "PropertyChangedCmd.h"
#include "VCFBuilderUI.h"
#include "MainUIPanel.h"
#include "ComboBoxControl.h"
#include "DefaultListItem.h"


using namespace VCF;
using namespace VCFBuilder;

PropertyCellItemEditor::PropertyCellItemEditor( TableCellItem* itemToEdit ):
	BasicTableItemEditor( itemToEdit )
{
	m_currentPropertyCellItem = NULL;
	m_currentPropertyEditor = NULL;
	m_editorIsComponentList = false;
	m_componentListEditorControl = NULL;
}

PropertyCellItemEditor::~PropertyCellItemEditor()
{

}

void PropertyCellItemEditor::setItemToEdit( TableCellItem* itemToEdit )
{
	BasicTableItemEditor::setItemToEdit( itemToEdit );
	m_currentPropertyCellItem = dynamic_cast<PropertyCellItem*>( itemToEdit );
	m_currentPropertyEditor = NULL;
}

void PropertyCellItemEditor::updateItem()
{
	if ( NULL != m_currentPropertyCellItem ){
		
		Property* property = m_currentPropertyCellItem->getProperty();

		if ( NULL != m_currentPropertyEditor ) {			
			if ( NULL != property ){
				if ( m_currentPropertyEditor->hasCustomEditor() ){
					//assume the value is already in the PropertyEditor
					//don't do anything cause the Custom editor will handle this
				}
				else {
					BasicTableItemEditor::updateItem();
					m_currentPropertyEditor->setValueAsText( m_currentPropertyCellItem->getCaption() );
				}
				
				Component* propSource = (Component*)property->getSource();
				PropertyChangedCmd* newPropertyEditCmd = 
					new PropertyChangedCmd( property->getName(), propSource->getName(), m_currentPropertyEditor->getValue() );
				UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
				cmdStack->addCommand( newPropertyEditCmd );
				
			}
		}
		else if ( true == m_editorIsComponentList ) {
			if ( NULL != m_componentListEditorControl ) {
				ListItem* selectedItem = m_componentListEditorControl->getSelectedItem();
				Object* objVal = (Object*)selectedItem->getData();
				VariantData val;
				val = objVal;
				if ( NULL != objVal ) {					
					BasicTableItemEditor::updateItem();
					
					Component* propSource = (Component*)property->getSource();
					PropertyChangedCmd* newPropertyEditCmd = 
						new PropertyChangedCmd( property->getName(), propSource->getName(), &val );
					UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
					cmdStack->addCommand( newPropertyEditCmd );
				}
				else {
					property->set( &val );
					BasicTableItemEditor::updateItem();
				}
			}
		}
	}
	m_editorIsComponentList = false;
}

Control* PropertyCellItemEditor::getEditingControl()
{
	Control* result = NULL;
	m_componentListEditorControl = NULL;	
	
	m_editorIsComponentList = false;

	if ( NULL != m_currentPropertyCellItem ){
		Property* property = m_currentPropertyCellItem->getProperty();
		if ( NULL != property ){
			String propertyClassName = Class::getClassNameForProperty( property );
			PropertyEditor* pe = PropertyEditorManager::findEditor( propertyClassName );	
			m_currentPropertyEditor = pe;
			if ( NULL != pe ){
				pe->setValue( property->get() );
				if ( true == pe->hasCustomEditor() ){
					result = pe->getCustomEditor();
				}
				else {
					result = BasicTableItemEditor::getEditingControl();
				}
			}
			else if ( PROP_OBJECT == property->getType() ){ 
				/**
				*if we don't have a property editor, and the property is a
				*component then create a combobox that lists all the known 
				*components of correct class name
				*/
				std::vector<Component*> componentList;
				generateMatchingComponentList( property->getTypeClassName(), componentList );
				if ( false == componentList.empty() ) {
					m_componentListEditorControl = new ComboBoxControl();
					ListModel* lm = m_componentListEditorControl->getListModel();
					std::vector<Component*>::iterator it = componentList.begin();
					while ( it != componentList.end() ) {
						Component* component = *it;
						ListItem* li = new DefaultListItem( lm, component->getName() );
						li->setData( (void*)component );
						lm->addItem( li );
						it++;
					}
					ListItem* li = new DefaultListItem( lm, "null" );
					li->setData( NULL );
					lm->addItem( li );
					m_editorIsComponentList = true;
					result = m_componentListEditorControl;
				}
				
			}
		}
	}
	return result;
}


void PropertyCellItemEditor::findMatchingComponentsFromComponent( Component* srcComponent, const String& componentClassName, std::vector<Component*>& componentList )
{
	Enumerator<Component*>* components = srcComponent->getComponents();
	while ( true == components->hasMoreElements() ) {
		Component* component = components->nextElement();
		if ( component->getClassName() == componentClassName ) {
			componentList.push_back( component );
		}
	}

	Container* container = dynamic_cast<Container*>(srcComponent);
	if ( NULL != container ) {
		Enumerator<Control*>* controls = container->getChildren();
		while ( true == controls->hasMoreElements() ) {
			Control* control = controls->nextElement();
			findMatchingComponentsFromComponent( control, componentClassName, componentList );
		}
	}
}
void PropertyCellItemEditor::generateMatchingComponentList( const String& componentClassName, std::vector<Component*>& componentList )
{
	MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
	Frame* activeFrame = mainUIPanel->getActiveFrameControl();
	
	findMatchingComponentsFromComponent( activeFrame, componentClassName, componentList );
}