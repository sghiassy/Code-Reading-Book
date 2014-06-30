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

#ifndef _ABSTRACTTABLEMODEL_H__
#define _ABSTRACTTABLEMODEL_H__



namespace VCF {

#define ABSTRACTTABLEMODEL_CLASSID		"BCA97CD4-6567-11d4-8EFF-00207811CFAB"

class APPKIT_API AbstractTableModel : public TableModel{
public:
	BEGIN_ABSTRACT_CLASSINFO(AbstractTableModel, "VCF::AbstractTableModel", "VCF::TableModel", ABSTRACTTABLEMODEL_CLASSID );	
	END_CLASSINFO(AbstractTableModel);

	AbstractTableModel();

	virtual ~AbstractTableModel();

	EVENT_HANDLER_LIST(ModelEmptied)
	EVENT_HANDLER_LIST(ModelValidate)


    virtual void addModelValidationHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelValidate) 
	}
	
	virtual void removeModelValidationHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelValidate) 
	}

	virtual void addModelHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelEmptied) 
	}

	virtual void removeModelHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelEmptied) 
	}

	FIRE_EVENT(ModelValidate,ValidationEvent)

	FIRE_EVENT(ModelEmptied,ModelEvent)


	EVENT_HANDLER_LIST(TableCellAdded)
	EVENT_HANDLER_LIST(TableCellDeleted)
	EVENT_HANDLER_LIST(TableRowAdded)
	EVENT_HANDLER_LIST(TableRowDeleted)
	EVENT_HANDLER_LIST(TableColumnAdded)
	EVENT_HANDLER_LIST(TableColumnDeleted)

	virtual void addTableCellAddedHandler( EventHandler* handler ){
		ADD_EVENT_IMPL(TableCellAdded) 
	}
	
	virtual void removeTableCellAddedHandler( EventHandler* handler ){
		REMOVE_EVENT_IMPL(TableCellAdded) 
	}

	virtual void addTableCellDeletedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TableCellDeleted) 
	}
	
	virtual void removeTableCellDeletedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TableCellDeleted) 
	}

	virtual void addTableRowAddedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TableRowAdded) 
	}

	virtual void removeTableRowAddedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TableRowAdded) 
	}

	virtual void addTableRowDeletedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TableRowDeleted) 
	}

	virtual void removeTableRowDeletedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TableRowDeleted) 
	}

	virtual void addTableColumnAddedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TableColumnAdded) 
	}

	virtual void removeTableColumnAddedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TableColumnAdded) 
	}

	virtual void addTableColumnDeletedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TableColumnDeleted) 
	}

	virtual void removeTableColumnDeletedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TableColumnDeleted) 
	}

	FIRE_EVENT(TableCellAdded,TableModelEvent)
	FIRE_EVENT(TableCellDeleted,TableModelEvent)
	FIRE_EVENT(TableRowAdded,TableModelEvent)
	FIRE_EVENT(TableRowDeleted,TableModelEvent)
	FIRE_EVENT(TableColumnAdded,TableModelEvent)
	FIRE_EVENT(TableColumnDeleted,TableModelEvent)

	virtual void addRow();

	virtual void insertRow( const uint32& afterRow );

	virtual void deleteRow( const uint32& row );

    virtual void addColumn();

	virtual void insertColumn( const uint32& afterColumn );

	virtual void deleteColumn( const uint32& column );

	virtual bool isCellEditable( const uint32& row, const uint32& column );

	virtual TableCellItem* getItem( const uint32& row, const uint32& column );
	
	virtual uint32 getRowCount();

	virtual uint32 getColumnCount();
};

}; //end of namespace VCF

#endif //_ABSTRACTTABLEMODEL_H__