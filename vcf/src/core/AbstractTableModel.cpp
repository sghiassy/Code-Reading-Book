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

using namespace VCF;

AbstractTableModel::AbstractTableModel()
{
	INIT_EVENT_HANDLER_LIST(ModelEmptied)
	INIT_EVENT_HANDLER_LIST(ModelValidate)

	INIT_EVENT_HANDLER_LIST(TableCellAdded)
	INIT_EVENT_HANDLER_LIST(TableCellDeleted)
	INIT_EVENT_HANDLER_LIST(TableRowAdded)
	INIT_EVENT_HANDLER_LIST(TableRowDeleted)
	INIT_EVENT_HANDLER_LIST(TableColumnAdded)
	INIT_EVENT_HANDLER_LIST(TableColumnDeleted)
}

AbstractTableModel::~AbstractTableModel()
{
	
}

void AbstractTableModel::addRow()
{

}

void AbstractTableModel::insertRow( const uint32& afterRow )
{

}

void AbstractTableModel::deleteRow( const uint32& row )
{

}

void AbstractTableModel::addColumn()
{

}

void AbstractTableModel::insertColumn( const uint32& afterColumn )
{

}

void AbstractTableModel::deleteColumn( const uint32& column )
{
	
}

bool AbstractTableModel::isCellEditable( const uint32& row, const uint32& column )
{
	return false;
}

TableCellItem* AbstractTableModel::getItem( const uint32& row, const uint32& column )
{	
	return NULL;
}

uint32 AbstractTableModel::getRowCount()
{
	return 0;
}

uint32 AbstractTableModel::getColumnCount()
{
	return 0;
}