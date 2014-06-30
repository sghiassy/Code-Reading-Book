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

#ifndef _TABLEITEMEDITOR_H__
#define _TABLEITEMEDITOR_H__


namespace VCF {

class Control;

class ItemEditorListener;

class TableCellItem;

#define TABLEITEMEDITOR_CLASSID		"53EA0BA4-7068-11d4-8F12-00207811CFAB"

class APPKIT_API TableItemEditor : public Object{
public:
	BEGIN_ABSTRACT_CLASSINFO(TableItemEditor, "VCF::TableItemEditor", "VCF::Object", TABLEITEMEDITOR_CLASSID)
	EVENT("VCF::ItemEditorEventHandler", "ItemEditorEvent", "CellItemChanged" )
	EVENT("VCF::ItemEditorEventHandler", "ItemEditorEvent", "CellItemValidateChange" )
	END_CLASSINFO(TableItemEditor)

	TableItemEditor(){};

	virtual ~TableItemEditor(){};	

	virtual bool isCellEditable() = 0;

	virtual void addCellItemChangedHandler( EventHandler* handler ) = 0;

	virtual void removeCellItemChangedHandler( EventHandler* handler ) = 0;

	virtual void addCellItemValidateChangeHandler( EventHandler* handler ) = 0;

	virtual void removeCellItemValidateChangeHandler( EventHandler* handler ) = 0;

	/**
	*this is called to initialize the table item editor with some value
	*from the TableCellItem, typically the caption.
	*/
	virtual void setItemToEdit( TableCellItem* itemToEdit ) = 0;

	virtual TableCellItem* getItemToEdit() = 0;

	/**
	*updates the item passed in during the setItemToEdit() call, to 
	*change to whatever new value has been set in the editor.
	*prior to the data transfer, all ItemEditorListeners should have their
	*ItemEditorListener::onValidateChange() method called. Implementers of
	*ItemEditorListener::onValidateChange() should throw an exception if 
	*invalid data is detected. This can done by simply calling the 
	*ItemEditrEvent's fireInvalidStateException() method.
	*the data is presumed to be stored in the Event's setUserData/getUserData
	*calls.
	*/
	virtual void updateItem() = 0;

	virtual Control* getEditingControl() = 0;
};

}; //end of namespace VCF

#endif //_TABLEITEMEDITOR_H__