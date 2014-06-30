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

#ifndef _TABLEMODELEVENT_H__
#define _TABLEMODELEVENT_H__


namespace VCF {

#define TABLEMODEL_EVENT_CONST						2000
#define COLUMN_DELETED					CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 1
#define COLUMN_ADDED					CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 2
#define ROW_DELETED						CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 3
#define ROW_ADDED						CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 4
#define CELL_CHANGED					CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 5
#define ALL_COLUMNS_CHANGED				CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 6	
#define ALL_ROWS_CHANGED				CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 7	
#define ALL_ITEMS_DELETED				CUSTOM_EVENT_TYPES + TABLEMODEL_EVENT_CONST + 8

#define NO_ROW_CHANGED					-1
#define NO_COLUMN_CHANGED				-1


class APPKIT_API TableModelEvent : public Event {
public:

	TableModelEvent( Object* source, const ulong32& eventType,
		             const int& rowThatChanged =-1,
					 const int& columnThatChanged =-1 );

	virtual ~TableModelEvent();

	/**
	*get the row that changed. a return of -1 means no rows
	*changed for this event
	*/
	int getRowThatChanged();

	/**
	*get the column that changed. a return of -1 means no columns
	*changed for this event
	*/
	int getColumnThatChanged();

private:
	int m_rowThatChanged;
	int m_columnThatChanged;
};

}; //end of namespace VCF

#endif //_TABLEMODELEVENT_H__