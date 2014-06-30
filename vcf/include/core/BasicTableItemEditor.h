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

#ifndef _BASICTABLEITEMEDITOR_H__
#define _BASICTABLEITEMEDITOR_H__


namespace VCF {

class TableCellItem;

class TextModelHandler;

class TextEvent;


#define BASICTABLEITEMEDITOR_CLASSID		"7E202381-F9EB-4585-9685-889FA9B07D47"

class APPKIT_API BasicTableItemEditor : public TableItemEditor{
public:

	BEGIN_CLASSINFO(BasicTableItemEditor, "VCF::BasicTableItemEditor", "VCF::TableItemEditor", BASICTABLEITEMEDITOR_CLASSID)
	END_CLASSINFO(BasicTableItemEditor)

	BasicTableItemEditor( TableCellItem* editingItem = NULL );

	virtual ~BasicTableItemEditor();

	virtual bool isCellEditable();

	EVENT_HANDLER_LIST(CellItemChanged)
	EVENT_HANDLER_LIST(CellItemValidateChange)

	virtual void addCellItemChangedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(CellItemChanged)
	}

	virtual void removeCellItemChangedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(CellItemChanged)
	}

	virtual void addCellItemValidateChangeHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(CellItemValidateChange)
	}

	virtual void removeCellItemValidateChangeHandler( EventHandler* handler )  {
		REMOVE_EVENT_IMPL(CellItemValidateChange)
	}

	FIRE_EVENT(CellItemChanged,ItemEditorEvent)
	FIRE_EVENT(CellItemValidateChange,ItemEditorEvent)

	virtual void setItemToEdit( TableCellItem* itemToEdit );

	virtual TableCellItem* getItemToEdit(){
		return 	m_editingItem;
	}

	virtual void updateItem();

	virtual Control* getEditingControl();

	Control* getCurrentEditingControl( TableCellItem* currentItem );
private:
	TableCellItem* m_editingItem;
	/**
	*this is here just in case we get reused by multiple items
	*/
	std::map<TableCellItem*,Control*> m_editorMap;

	void onEditorTextChanged( TextEvent* event );

	TextModelEventHandler<BasicTableItemEditor>* m_textModelHandler;
};

}; //end of namespace VCF

#endif //_BASICTABLEITEMEDITOR_H__