//ApplicationKit.h
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


#ifndef _APPLICATION_KIT_H__
#define _APPLICATION_KIT_H__

#include "GraphicsKit.h" //GraphicsKit include FoundationKit.h"
#include "Item.h"
#include "ItemListener.h"
#include "ItemEvent.h"
#include "KeyboardEvent.h"
#include "KeyboardListener.h"
#include "NotifyListener.h"
#include "NotifyEvent.h"
#include "ScrollListener.h"
#include "ScrollEvent.h"
#include "SelectionListener.h"
#include "ValidationEvent.h"
#include "InvalidStateException.h"
#include "ApplicationException.h"
#include "BadComponentStateException.h"
#include "BadModelStateException.h"
#include "ItemEditorEvent.h"
#include "ItemEditorListener.h"
#include "ListItem.h"

#include "TreeItem.h"
#include "ComponentEvent.h"
#include "ComponentListener.h"
#include "Component.h"
#include "Menu.h"
#include "MenuBar.h"
#include "MenuBarPeer.h"
#include "PopupMenu.h"
#include "PopupMenuPeer.h"
#include "MenuListener.h"
#include "MenuItem.h"
#include "MenuItemPeer.h"
#include "MenuItemEvent.h"
#include "MenuItemListener.h"
#include "MenuItemShortCut.h"

#include "TableCellItem.h"

#include "Model.h"
#include "ModelValidationListener.h"
#include "ModelEvent.h"
#include "ModelListener.h"
#include "ListModel.h"
#include "ListModelEvent.h"
#include "ListModelListener.h"
#include "AbstractListModel.h"

#include "TableModel.h"
#include "TableModelListener.h"
#include "TableModelEvent.h"
#include "AbstractTableModel.h"

#include "TabModelEvent.h"
#include "TabModelListener.h"
#include "TabModel.h"
#include "TabPage.h"

#include "TextEvent.h"
#include "TextModelListener.h"
#include "TextModel.h"
#include "AbstractTextModel.h"

#include "TreeModelEvent.h"
#include "TreeModelListener.h"
#include "TreeModel.h"
#include "AbstractTreeModel.h"

#include "Resource.h"
#include "ResourceBundle.h"
#include "ResourceException.h"
#include "ResourceStream.h"

#include "ClipboardDataObject.h"
#include "ClipboardEvent.h"
#include "ClipboardListener.h"
#include "ClipboardPeer.h"
#include "Clipboard.h"

#include "DataObjectPeer.h"
#include "DataType.h"
#include "DataTypeNotSupportedException.h"

#include "DragDropPeer.h"
#include "DragEvent.h"
#include "DragScrollEvent.h"

#include "DropEvent.h"
#include "DragSourceListener.h"
#include "DragSource.h"

#include "DropTargetListener.h"
#include "DropTarget.h"

#include "DropTargetPeer.h"

#include "ApplicationPeer.h"
#include "AbstractApplication.h"
#include "Application.h"

#include "Border.h"

#include "Cursor.h"

#include "MouseEvent.h"
#include "MouseListener.h"
#include "FocusEvent.h"
#include "FocusListener.h"	
#include "ToolTipEvent.h"
#include "ToolTipListener.h"
#include "KeyBoardEvent.h"	
#include "KeyBoardListener.h"
#include "WhatsThisHelpEvent.h"
#include "WhatsThisHelpListener.h"
#include "HelpEvent.h"
#include "HelpListener.h"
#include "ControlEvent.h"
#include "ControlListener.h"
#include "ControlPeer.h"
#include "ControlGraphicsContext.h"
#include "View.h"
#include "Controller.h"
#include "Control.h"
#include "CustomControl.h"
#include "Container.h"
#include "Frame.h"
#include "Dialog.h"
#include "DialogPeer.h"
#include "CommonDialog.h"
#include "CommonDialogPeer.h"
#include "CommonFileDialog.h"
#include "CommonFileOpen.h"
#include "CommonFileSave.h"
#include "CommonFont.h"
#include "CommonPrint.h"
#include "CommonColor.h"
#include "Button.h"
#include "ButtonPeer.h"
#include "ButtonEvent.h"
#include "ButtonListener.h"
#include "CheckBoxControl.h"
#include "ComboBoxListener.h"
#include "ComboBoxControl.h"
#include "CommandButton.h"
#include "LightweightComponent.h"
#include "OpenGLControl.h"
#include "OpenGLControlContext.h"
#include "PrintPeer.h"
#include "PrintContextPeer.h"
#include "PropertyEditor.h"
#include "PropertyEditorManager.h"
#include "TextPeer.h"
#include "RichTextPeer.h"
#include "TreePeer.h"
#include "ListviewPeer.h"
#include "WindowEvent.h"
#include "WindowListener.h"
#include "Window.h"
#include "WindowPeer.h"
#include "NoToolKitFoundException.h"
#include "UIToolkit.h"

#include "Basic3DBorder.h"
#include "Light3DBorder.h"
#include "ControlContainer.h"

#include "Command.h"
#include "AbstractCommand.h"
#include "DesktopPeer.h"
#include "Desktop.h"
#include "ScrollPeer.h"
#include "ComboBoxPeer.h"
#include "Scrollable.h"
#include "HeaderPeer.h"
#include "ResourceStreamPeer.h"
#include "CursorPeer.h"
#include "CursorManager.h"

#include "ImageListEvent.h"
#include "ImageListListener.h"
#include "ImageList.h"

/**
*initializes the Application Kit runtime.
*This includes registering the basic classes in the runtime system
*This MUST be called first off in the main() function of the app
*This will automatically call 
	initFoundationKit();
and
	initGraphicsKit();
*/



APPKIT_API void initApplicationKit();

APPKIT_API void terminateApplicationKit();


#endif //_APPLICATION_KIT_H__