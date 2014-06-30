//Tabs.cpp


#include "ApplicationKit.h"
#include "Panel.h"
#include "TabbedPages.h"
#include "TabPage.h"
#include "ListViewControl.h"
#include "DefaultListItem.h"
#include "TreeControl.h"
#include "DefaultTreeItem.h"

using namespace VCF;


class TabWindow : public Window {
public:
	TabWindow() {
		Panel* panel = new Panel();		
		
		//create a new TabbedPages control
		TabbedPages* tabs = new TabbedPages();
		tabs->setToolTip( "Tooltip for TabbedPages 1" );

		//add it to the window, align it to the entire client area of the parent
		add( tabs, ALIGN_CLIENT );

		//add a new page
		TabPage* page1 = tabs->addNewPage( "Tab Page 1 Free For All !" );
		//get the panel control, which represents the page's 
		//Control, to which other controls may now be added
		Panel* panel1 = (Panel*)page1->getPageComponent();
		panel1->setBorderSize( 10 );

		panel1->setToolTip( "Tooltip for panel 1" );

		//create a tree control
		TreeControl* treeControl = new TreeControl();
		treeControl->setVisible( true );
		treeControl->setToolTip( "Tooltip for treeControl 1" );

		//add the tree control to page1's panel control,
		//with client alignment
		panel1->add( treeControl, ALIGN_CLIENT );

		//get the tree model from the TreeControl
		TreeModel* treeModel = treeControl->getTreeModel();

		//add node item's to the tree model - update's happen
		//automatically in the control
		treeModel->addNodeItem( new DefaultTreeItem("Root1") );
		treeModel->addNodeItem( new DefaultTreeItem("Root2") );
		TreeItem* root3 = new DefaultTreeItem("Root3");
		treeModel->addNodeItem( root3 );
		treeModel->addNodeItem( new DefaultTreeItem("sub item1"), root3 );
		treeModel->addNodeItem( new DefaultTreeItem("sub item2"), root3 );
		treeModel->addNodeItem( new DefaultTreeItem("sub item3"), root3 );

		//add page 2
		TabPage* page2 = tabs->addNewPage( "Tab Page 2" );

		//get the panel control for page 2
		Panel* panel2 = (Panel*)page2->getPageComponent();

		//create a list view control
		ListViewControl* lv = new ListViewControl();
		//set the style - to details, which will display column headers
		lv->setIconStyle( IS_DETAILS );
		//add a new column named "Foo"
		lv->addHeaderColumn( "Foo" );

		//get the ListModel
		ListModel* lm = lv->getListModel();
		//add a new list item to the list model
		lm->addItem( new DefaultListItem( lm, "Foobar" ) );		
		lv->setVisible( true );

		//add the list view to page2's panel control
		panel2->add( lv, ALIGN_CLIENT );

		//add page 3
		TabPage* page3 = tabs->addNewPage( "Tab Page 3" );		

		setVisible( true ); 

	};

	virtual ~TabWindow() {};

	

};


class TabsApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* tabWindow = new TabWindow();
		
		setMainWindow(tabWindow);

		tabWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		
		tabWindow->setCaption( "Tab Example" );

		return result;
	}

};


int main(int argc, char *argv[])
{
	TabsApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

