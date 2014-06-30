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
#include "DefaultTableModel.h"
#include "DefaultTableCellItem.h"

using namespace VCF;

DefaultTableModel::DefaultTableModel()
{
	this->init();
}

DefaultTableModel::~DefaultTableModel()
{
	std::vector<TTableColumn*>::iterator rowIter = m_tableData.begin();
	while ( rowIter != m_tableData.end() ){
		TTableColumn* row = *rowIter;
		std::vector<TableCellItem*>::iterator it = row->begin();
		while ( it != row->end() ){
			TableCellItem* item = *it;
			delete item;
			item = NULL;
			it++;
		}
		row->clear();
		delete row;
		row = NULL;
		rowIter ++;
	}	
	m_tableData.clear();
}

void DefaultTableModel::init()
{
	this->m_columnCount = 0;
	this->m_rowCount = 0;
}
   

void DefaultTableModel::validate()
{

}

void DefaultTableModel::empty()
{
	std::vector<TTableColumn*>::iterator rowIter = m_tableData.begin();
	while ( rowIter != m_tableData.end() ){
		TTableColumn* row = *rowIter;
		std::vector<TableCellItem*>::iterator it = row->begin();
		while ( it != row->end() ){
			TableCellItem* item = *it;
			delete item;
			item = NULL;
			it++;
		}
		row->clear();
		delete row;
		row = NULL;
		rowIter ++;
	}	
	m_tableData.clear();
	this->m_columnCount = 0;
	this->m_rowCount = 0;	

	ModelEvent modelEvent(this);
	fireOnModelEmptied( &modelEvent );
}

void DefaultTableModel::addRow()
{
	bool notifyFirstColumnAdded = false;
	if ( 0 == this->m_columnCount ){
		m_columnCount++; //gotta have at least one column !
		notifyFirstColumnAdded = true;
	}
	this->m_rowCount ++;
	TTableColumn* newRow = new TTableColumn();
	for ( int i=0;i<m_columnCount;i++){
		TableCellItem* newItem = NULL;
		createCell( m_rowCount-1, i, &newItem );
		newItem->setColumn( i );
		newItem->setRow( m_rowCount -1 );
		newRow->push_back( newItem );
	}
	
	this->m_tableData.push_back( newRow );

	if ( true == notifyFirstColumnAdded ){
		TableModelEvent columnEvent( this, COLUMN_ADDED, NO_ROW_CHANGED, m_columnCount-1 );
		
		fireOnTableColumnAdded( &columnEvent );	
	}

	TableModelEvent event( this, ROW_ADDED, m_rowCount-1 );
	fireOnTableRowAdded( &event );
}

void DefaultTableModel::insertRow( const uint32& afterRow )
{
	if ( 0 == this->m_columnCount ){
		m_columnCount++; //gotta have at least one column !
	}

	this->m_rowCount ++;
	TTableColumn* newRow = new TTableColumn();
	for ( int i=0;i<m_columnCount;i++){
		TableCellItem* newItem = NULL;
		createCell( afterRow, i, &newItem );
		newItem->setColumn( i );
		newItem->setRow( afterRow );
		newRow->push_back( newItem );
	}

	this->m_tableData.insert( m_tableData.begin() + afterRow, newRow );
	TableModelEvent event( this, ROW_ADDED, afterRow );
	fireOnTableRowAdded( &event );
}

void DefaultTableModel::deleteRow( const uint32& row )
{
	TableModelEvent event( this, ROW_DELETED, row );
	fireOnTableRowDeleted( &event );

	std::vector<TTableColumn*>::iterator found = m_tableData.begin() + row;
	if ( found != m_tableData.end() ){
		TTableColumn* row = *found;
		std::vector<TableCellItem*>::iterator it = row->begin();
		while ( it != row->end() ){
			TableCellItem* item = *it;
			delete item;
			item = NULL;
			it++;
		}
		m_tableData.erase( found );
		delete row;
		row = NULL;
		if ( m_rowCount > 0 ){
			this->m_rowCount --;
		}
	}	
}

void DefaultTableModel::addColumn()
{	
	this->m_columnCount ++;
	
	if ( 0 == this->m_rowCount ){
		this->addRow();//gotta have at least one row !
	}
	else {
		int i = 0;
		std::vector<TTableColumn*>::iterator rowIter = m_tableData.begin();
		while ( rowIter != m_tableData.end() ){
			TTableColumn* row = *rowIter;
			TableCellItem* newItem = NULL;
			createCell( i, m_columnCount-1, &newItem );	
			newItem->setColumn( m_columnCount-1 );
			newItem->setRow( i );	
			row->push_back( newItem );
			rowIter++;
			i++;
		}
	}
	TableModelEvent event( this, COLUMN_ADDED, NO_ROW_CHANGED, m_columnCount-1 );
	fireOnTableColumnAdded( &event );
}

void DefaultTableModel::insertColumn( const uint32& afterColumn )
{
	this->m_columnCount ++;
	if ( 0 == this->m_rowCount ){
		this->addRow();//gotta have at least one row !
	}
	else {
		int i = 0;
		std::vector<TTableColumn*>::iterator rowIter = m_tableData.begin();
		while ( rowIter != m_tableData.end() ){
			TTableColumn* row = *rowIter;
			TableCellItem* newItem = NULL;
			createCell( i, afterColumn, &newItem );	
			newItem->setColumn( afterColumn );
			newItem->setRow( i );
			row->insert( row->begin() + afterColumn, newItem );
			rowIter++;
			i++;
		}
	}

	TableModelEvent event( this, COLUMN_ADDED, NO_ROW_CHANGED, afterColumn );
	fireOnTableColumnAdded( &event );
}

void DefaultTableModel::deleteColumn( const uint32& column )
{

	TableModelEvent event( this, COLUMN_DELETED, NO_ROW_CHANGED, column );
	fireOnTableColumnDeleted( &event );

	std::vector<TTableColumn*>::iterator rowIter = m_tableData.begin();
	while ( rowIter != m_tableData.end() ){
		TTableColumn* row = *rowIter;
		std::vector<TableCellItem*>::iterator found = row->begin() + column;
		if ( found != row->end() ){
			TableCellItem* item = *found;
			row->erase( found );
			delete item;
			item = NULL;
		}
		rowIter++;
	}
	if ( m_columnCount > 0 ){
		m_columnCount--;
	}
}

bool DefaultTableModel::isCellEditable( const uint32& row, const uint32& column )
{
	bool result = false;
	TableCellItem* item = getItem( row, column );
	if ( NULL != item ){
		result = item->isItemEditable();
	}
	return result;
}

TableCellItem* DefaultTableModel::getItem( const uint32& row, const uint32& column )
{
	TableCellItem* result = NULL;
	TTableColumn* rows = this->m_tableData[row];
	result = (*rows)[column];
	return result;
}

uint32 DefaultTableModel::getRowCount()
{
	return m_rowCount;
}

uint32 DefaultTableModel::getColumnCount()
{
	return this->m_columnCount;
}

TableRowItemEnumerator* DefaultTableModel::getRowItemEnumerator( const uint32& row )
{
	TableRowItemEnumerator* result = NULL;
	m_rowEnumContainer.initContainer( *(m_tableData[row]) );
	
	result = m_rowEnumContainer.getEnumerator();

	return result;
}

void DefaultTableModel::createCell( const uint32& row, const uint32& column, TableCellItem** createdCell )
{
	*createdCell = new DefaultTableCellItem();
}