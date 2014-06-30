//MainWindow.h
#include "ApplicationKit.h"
#include "MainWindow.h"
#include "DefaultMenuItem.h"

#include "ListControlsApplication.h"
#include "ListboxControl.h"
#include "ListviewControl.h"
#include "Splitter.h"

#include "DefaultListItem.h"
#include "Light3DBorder.h"

using namespace VCF;



class MyListItem : public DefaultListItem {
public:
	MyListItem( Model* model, const String& caption ):
	  DefaultListItem( model, caption ) {

	}

	virtual void paint( GraphicsContext* gc, Rect* paintRect ) {
		DefaultListItem::paint( gc, paintRect );
		//Light3DBorder bdr;
		//bdr.paint( paintRect, gc );
		gc->rectangle( paintRect );
		gc->strokePath();
	}
	
	virtual bool canPaint() {
		return true;
	}

};


MainWindow::MainWindow()
{
	//build main menu
	MenuBar* menuBar = new MenuBar();
	this->setMenuBar( menuBar );
	MenuItem* root = menuBar->getRootMenuItem();
	MenuItemEventHandler<MainWindow>* menuItemHandler = NULL;

	DefaultMenuItem* file = new DefaultMenuItem( "&File", root, menuBar );

	DefaultMenuItem* fileOpenProject = new DefaultMenuItem( "&Open...", file, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onFileOpen, "fileOpen" );
	fileOpenProject->addMenuItemClickedHandler( menuItemHandler );
	
	
	DefaultMenuItem* fileSaveProject = new DefaultMenuItem( "&Save...", file, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onFileSave, "fileSave" );	
	fileSaveProject->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* sep = new DefaultMenuItem( "", file, menuBar );
	sep->setSeparator( true );
	
	DefaultMenuItem* fileExit = new DefaultMenuItem( "E&xit", file, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onFileExit, "fileExit" );	
	fileExit->addMenuItemClickedHandler( menuItemHandler );

	DefaultMenuItem* edit = new DefaultMenuItem( "&Edit", root, menuBar );

	DefaultMenuItem* editAddItemToListView = new DefaultMenuItem( "Add to List &View", edit, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditAddItemToListView,"editAddItemToListView");	
	editAddItemToListView->addMenuItemClickedHandler( menuItemHandler );

	DefaultMenuItem* editAddItemToListBox = new DefaultMenuItem( "Add to List &Box", edit, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditAddItemToListBox,"editAddItemToListBox");	
	editAddItemToListBox->addMenuItemClickedHandler( menuItemHandler );

	DefaultMenuItem* view = new DefaultMenuItem( "&View", root, menuBar );
	DefaultMenuItem* viewListView = new DefaultMenuItem( "ListView", view, menuBar );
	DefaultMenuItem* viewListViewIconStyle = new DefaultMenuItem( "Icon Style", viewListView, menuBar );
	DefaultMenuItem* iconStyle1 = new DefaultMenuItem( "Large Icon", viewListViewIconStyle, menuBar );
	DefaultMenuItem* iconStyle2 = new DefaultMenuItem( "Small Icon", viewListViewIconStyle, menuBar );
	DefaultMenuItem* iconStyle3 = new DefaultMenuItem( "List", viewListViewIconStyle, menuBar );
	DefaultMenuItem* iconStyle4 = new DefaultMenuItem( "Report", viewListViewIconStyle, menuBar );

	EventHandler* iconStyleHandler = new MenuItemEventHandler<MainWindow>( this, MainWindow::onViewListViewIconStyle, "iconStyleHandler" );  
	iconStyle1->addMenuItemClickedHandler( iconStyleHandler );
	iconStyle2->addMenuItemClickedHandler( iconStyleHandler );
	iconStyle3->addMenuItemClickedHandler( iconStyleHandler );
	iconStyle4->addMenuItemClickedHandler( iconStyleHandler );	


	DefaultMenuItem* viewListViewAddColumn = new DefaultMenuItem( "Add Column", viewListView, menuBar );
	viewListViewAddColumn->addMenuItemClickedHandler( 
										new MenuItemEventHandler<MainWindow>( this, 
																				MainWindow::onViewListViewAddColumn,
																				"viewListViewAddColumnHandler" ) );
	

	m_listBox = new ListBoxControl();
	m_listBox->setWidth( 200 );
	m_listBox->setVisible( true );
	add( m_listBox, ALIGN_LEFT );

	Splitter* splitter = new Splitter();
	add( splitter, ALIGN_LEFT );

	m_listView = new ListViewControl();
	m_listView->setVisible( true );
	add( m_listView, ALIGN_CLIENT );
}

MainWindow::~MainWindow()
{
	
}
void MainWindow::onFileOpen( MenuItemEvent* e )
{
	CommonFileOpen openDlg;	
	openDlg.addFilter( "All Files", "*.*" );
	if ( true == openDlg.execute() ) {
		
	}
	
}

void MainWindow::onFileSave( MenuItemEvent* e )
{
	CommonFileSave saveDlg;
	if ( true == saveDlg.execute() ) {
		
	}
}

void MainWindow::onFileExit( MenuItemEvent* e )
{
	close();
}

void MainWindow::onEditAddItemToListView(  VCF::MenuItemEvent* e )
{
	ListModel* listModel = m_listView->getListModel();
	DefaultListItem* listItem = new MyListItem( listModel,
													"List View Item " + StringUtils::toString( listModel->getCount() ) );

	listModel->addItem( listItem );
}	

void MainWindow::onEditAddItemToListBox(  VCF::MenuItemEvent* e )
{
	ListModel* listModel = m_listBox->getListModel();
	DefaultListItem* listItem = new DefaultListItem( listModel,
													"List Item " + StringUtils::toString( listModel->getCount() ) );

	listModel->addItem( listItem );
}

void MainWindow::onViewListViewIconStyle(  VCF::MenuItemEvent* e )
{
	MenuItem* item = (MenuItem*)e->getSource();
	switch ( item->getIndex() ) {
		case 0 : {
			m_listView->setIconStyle( IS_LARGE_ICON );
		}
		break;

		case 1 : {
			m_listView->setIconStyle( IS_SMALL_ICON );
		}
		break;

		case 2 : {
			m_listView->setIconStyle( IS_LIST );
		}
		break;

		case 3 : {
			m_listView->setIconStyle( IS_DETAILS );
		}
		break;
	}
}

void MainWindow::onViewListViewAddColumn(  VCF::MenuItemEvent* e )
{
	static int i = 0;
	i++;
	m_listView->addHeaderColumn( "Column " + StringUtils::toString(i) );
}