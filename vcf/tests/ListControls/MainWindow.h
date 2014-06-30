//MainWindow.h

#ifndef _MAINWINDOW_H__
#define _MAINWINDOW_H__



namespace VCF {
	class ListBoxControl;
};
/**
*Class MainWindow documentation
*/
class MainWindow : public VCF::Window { 
public:
	MainWindow();

	virtual ~MainWindow();
	void onFileOpen( VCF::MenuItemEvent* e );

	void onFileSave( VCF::MenuItemEvent* e );

	void onFileExit( VCF::MenuItemEvent* e );

	void onEditAddItemToListView(  VCF::MenuItemEvent* e );

	void onEditAddItemToListBox(  VCF::MenuItemEvent* e );

	void onViewListViewIconStyle(  VCF::MenuItemEvent* e );

	void onViewListViewAddColumn(  VCF::MenuItemEvent* e );
protected:
	VCF::ListBoxControl* m_listBox;
	VCF::ListViewControl* m_listView;
private:
};


#endif //_MAINWINDOW_H__


