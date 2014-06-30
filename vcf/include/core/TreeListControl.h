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

// TreeListControl.h

#ifndef _TREELISTCONTROL_H__
#define _TREELISTCONTROL_H__



namespace VCF
{

class ImageList;

class TreeModel;

class DefaultTreeListModel;

class APPKIT_API DefaultColumnHeaderItem : public DefaultListItem {
public:
	DefaultColumnHeaderItem( const String& caption, Control* owningControl, DefaultTreeListModel* model );

	virtual ~DefaultColumnHeaderItem();

	virtual void paint( GraphicsContext* context, Rect* paintRect );

	double getWidth() {
		return m_columnWidth;
	}

protected:
	double m_columnWidth;
};



class APPKIT_API DefaultTreeListModel : public DefaultTreeModel {
public:
	DefaultTreeListModel( Control* owningControl );

	virtual ~DefaultTreeListModel();

	void addColumnHeaderItem( DefaultColumnHeaderItem* columnItem );

	void insertColumnHeaderItem( DefaultColumnHeaderItem* columnItem, const ulong32& index );

	void removeColumnHeaderItem( DefaultColumnHeaderItem* columnItem );

	void removeColumnHeaderItem( const ulong32& index );

	Enumerator<DefaultColumnHeaderItem*>* getColumnHeaders();

	DefaultColumnHeaderItem* getColumnHeaderItem( const ulong32& index );

	ulong32 getColumnHeaderCount() {
		return m_columnItems.size();
	}

	virtual void setControl( Control* owningControl );

	virtual Control* getControl() {
		return m_owningControl;
	}
protected:
	Control* m_owningControl;
	std::vector<DefaultColumnHeaderItem*> m_columnItems;
	EnumeratorContainer<std::vector<DefaultColumnHeaderItem*>,DefaultColumnHeaderItem*> m_columnContainer;
};




class APPKIT_API DefaultTreeListItem : public DefaultTreeItem 
{
public:
	DefaultTreeListItem( const String& caption="", Control* owningControl=NULL, TreeModel* model=NULL );
	
	virtual ~DefaultTreeListItem();

	virtual void paint( GraphicsContext* context, Rect* paintRect );

	virtual void paintExpander( GraphicsContext* context );
	
	virtual void paintSubItem( GraphicsContext* context, const ulong32& subItemIndex );

protected:
	Rect m_expanderRect;
};



class APPKIT_API TreeListControl : public CustomControl
{
public:
	TreeListControl();
	virtual ~TreeListControl();

	void setTreeModel(TreeModel * model);
    
	TreeModel* getTreeModel();

    void getImageList();

    void setImageList();

	virtual void paint( GraphicsContext * context );

	void init();

	void addItem( DefaultTreeListItem* item, DefaultTreeListItem* parent = NULL );

	void addColumnHeaderItem( DefaultColumnHeaderItem* columnItem );

	void insertColumnHeaderItem( DefaultColumnHeaderItem* columnItem, const ulong32& index );

	void removeColumnHeaderItem( DefaultColumnHeaderItem* columnItem );

	void removeColumnHeaderItem( const ulong32& index );

	ulong32 getItemIndent() {
		return m_itemIndent;
	}

	void setItemIndent( const ulong32& itemIndent );

	virtual void mouseDown( MouseEvent* event );	

	virtual void mouseMove( MouseEvent* event );

	virtual void mouseUp( MouseEvent* event );
protected:
	DefaultTreeListItem* hitTest( Point* pt, DefaultTreeListItem* itemToTest );
private:
	ImageList* m_imageList;
	DefaultTreeListModel* m_treeModel;
	double m_itemHeight;
	double m_columnHeight;
	ulong32 m_itemIndent;
	DefaultTreeListItem* m_currentSelectedItem;
};


};

#endif
