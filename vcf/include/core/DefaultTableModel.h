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

#ifndef _DEFAULTTABLEMODEL_H__
#define _DEFAULTTABLEMODEL_H__


namespace VCF {

#define DEFAULTTABLEMODEL_CLASSID		"53EA0BA8-7068-11d4-8F12-00207811CFAB"

class TableCellItem;

class APPKIT_API DefaultTableModel : public AbstractTableModel {
public:
	BEGIN_CLASSINFO(DefaultTableModel, "VCF::DefaultTableModel", "VCF::AbstractTableModel", DEFAULTTABLEMODEL_CLASSID )
	END_CLASSINFO(DefaultTableModel)
	DefaultTableModel();

	virtual ~DefaultTableModel();	

	
    /**
     * validate the model. 
     * The implementation for this can vary widely, or even be nonexistant for model's that do not require validation.
     * The basic idea is to call all the listeners in the list , passing in a local variable to the
     * onModelValidate() methods of the listener's. The variable is initialized to true, and if it is
     * still true at the end of the listener iterations, then it is safe to apply the changes to the
     * model, other wise the changes are removed. 
     */
    virtual void validate();

    /**
     * clears out the model's data 
     */
    virtual void empty();

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

	virtual TableRowItemEnumerator* getRowItemEnumerator( const uint32& row );

	virtual void createCell( const uint32& row, const uint32& column, TableCellItem** createdCell );
protected:
	/**
	*defines the a vector of TableCellItem's, or one row of data m_columnCount long
	*/
	typedef std::vector<TableCellItem*> TTableColumn;
private:
	void init();
	uint32 m_rowCount;
	uint32 m_columnCount;
	/*row/column data
	*access is by row column.
	*so m_tableData[0] is row 0, and (*m_tableData[0])[2] is the item in 
	*row 0 at column 3
	*/
	std::vector<TTableColumn*> m_tableData;

	EnumeratorContainer<std::vector<TableCellItem*>,TableCellItem*> m_rowEnumContainer;
};

}; //end of namespace VCF

#endif //_DEFAULTTABLEMODEL_H__