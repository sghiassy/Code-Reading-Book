//MainWindow.h
#include "ApplicationKit.h"
#include "MainWindow.h"
#include "DefaultMenuItem.h"

#include "ToolbarsApplication.h"
#include "Command.h"

#include "ToolbarsAbout.h"

#include "ToolbarDock.h"
#include "ToolbarButton.h"
#include "Toolbar.h"

#include "ImageList.h"
#include "Label.h"

using namespace VCF;



MainWindow::MainWindow()
{
	setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );

	//build main menu
	MenuBar* menuBar = new MenuBar();
	this->setMenuBar( menuBar );
	MenuItem* root = menuBar->getRootMenuItem();
	MenuItemEventHandler<MainWindow>* menuItemHandler = NULL;

	DefaultMenuItem* file = new DefaultMenuItem( "&File", root, menuBar );

	DefaultMenuItem* fileOpenProject = new DefaultMenuItem( "&Open...", file, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onFileOpenProject, "fileOpen" );
	fileOpenProject->addMenuItemClickedHandler( menuItemHandler );
	
	
	DefaultMenuItem* fileSaveProject = new DefaultMenuItem( "&Save...", file, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onFileSaveProject, "fileSave" );	
	fileSaveProject->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* sep = new DefaultMenuItem( "", file, menuBar );
	sep->setSeparator( true );
	
	DefaultMenuItem* fileExit = new DefaultMenuItem( "E&xit", file, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onFileExit, "fileExit" );	
	fileExit->addMenuItemClickedHandler( menuItemHandler );

	//edit Undo/Redo support
	DefaultMenuItem* edit = new DefaultMenuItem( "&Edit", root, menuBar );
	DefaultMenuItem* editUndo = new DefaultMenuItem( "Undo", edit, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onEditUndo, "editUndoClick" );	
	editUndo->addMenuItemClickedHandler( menuItemHandler );

	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onEditUndoUpdate, "editUndoUpdate" );	
	editUndo->addMenuItemUpdateHandler( menuItemHandler );
	
	
	DefaultMenuItem* editRedo = new DefaultMenuItem( "Redo", edit, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onEditRedo, "editRedoClick" );	
	editRedo->addMenuItemClickedHandler( menuItemHandler );

	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onEditRedoUpdate, "editRedoUpdate" );	
	editRedo->addMenuItemUpdateHandler( menuItemHandler );

	//add Help menu
	DefaultMenuItem* help = new DefaultMenuItem( "&Help", root, menuBar );
	DefaultMenuItem* helpAbout = new DefaultMenuItem( "About...", help, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this, MainWindow::onHelpAbout, "helpAbout" );	
	helpAbout->addMenuItemClickedHandler( menuItemHandler );

	//Tool bar stuff
	ToolbarDock* mainDock = new ToolbarDock();
	mainDock->setHeight( 35 );
	add( mainDock, ALIGN_TOP );

	Toolbar* mainToolbar = new Toolbar();
	mainToolbar->setHeight( 32 );
	mainDock->addToolbar( mainToolbar );

	ImageList* imageList = new ImageList();
	imageList->setImageHeight( 16 );
	imageList->setImageWidth( 16 );
	imageList->setTransparentColor( &Color(1.0f, 0.0f, 1.0f) );

	this->addComponent( imageList );	
	ResourceBundle* resBdl = Application::getRunningInstance()->getResourceBundle();
	Image* img = resBdl->getImage( "FILE_OPEN" );
	imageList->addImage( img ); 
	delete img;
	
	img = resBdl->getImage( "FILE_SAVE" );
	imageList->addImage( img ); 
	delete img;


	//add a new button
	ToolbarButton* openFileBtn = new ToolbarButton();
	mainToolbar->addButton( openFileBtn );
	openFileBtn->setImageList( imageList );
	openFileBtn->setBtnImageIndex( 0 );	
	openFileBtn->setShowCaption(false);
	openFileBtn->setAutoSizeCaption( false );	
	openFileBtn->setStateToggle( false );

	Label* l = new Label();
	//Light3DBorder* bdr = new Light3DBorder();
	l->setCaption( "Toolbar app" );
	l->setTransparent( false );
	//l->setBorder( bdr );
	l->setColor( &Color(0.8f,0.8f,0.8f) );
	l->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );
	add( l );
	l->setBounds( &Rect(20, 60, getWidth()-20, 90) );
	
}

MainWindow::~MainWindow()
{
	
}
void MainWindow::onFileOpenProject( MenuItemEvent* e )
{
	CommonFileOpen openDlg;	
	openDlg.addFilter( "Foo Files", "*.foo" );
	openDlg.addFilter( "Bar Files", "*.bar" );
	openDlg.addFilter( "All Files", "*.*" );
	if ( true == openDlg.execute() ) {
		
	}
	
}

void MainWindow::onFileSaveProject( MenuItemEvent* e )
{
	CommonFileSave saveDlg;
	saveDlg.addFilter( "Foo Files", "*.foo" );
	saveDlg.addFilter( "Bar Files", "*.bar" );
	if ( true == saveDlg.execute() ) {
		
	}
}

void MainWindow::onFileExit( MenuItemEvent* e )
{
	close();
}

void MainWindow::onEditUndo( MenuItemEvent* e )
{
	UndoRedoStack* cmdStack = ToolbarsApplication::getToolbarsApplication()->getCommandStack();
	cmdStack->undo();
}

void MainWindow::onEditUndoUpdate( VCF::MenuItemEvent* e )
{
	MenuItem* itemToUpdate = dynamic_cast<MenuItem*>(e->getSource());
	if ( NULL != itemToUpdate ) {
		UndoRedoStack* cmdStack = ToolbarsApplication::getToolbarsApplication()->getCommandStack();
		bool hasUndoableCmds = cmdStack->hasUndoableItems();
		if ( true == hasUndoableCmds ) {
			itemToUpdate->setCaption( "Undo \"" + cmdStack->getCurrentUndoComand()->getName() + "\"" );
		}
		else {
			itemToUpdate->setCaption( "Nothing to Undo" );
		}
		itemToUpdate->setEnabled( hasUndoableCmds );
	}
}

void MainWindow::onEditRedo( MenuItemEvent* e )
{
	UndoRedoStack* cmdStack = ToolbarsApplication::getToolbarsApplication()->getCommandStack();
	cmdStack->redo();
}

void MainWindow::onEditRedoUpdate( VCF::MenuItemEvent* e )
{
	MenuItem* itemToUpdate = dynamic_cast<MenuItem*>(e->getSource());
	if ( NULL != itemToUpdate ) {
		UndoRedoStack* cmdStack = ToolbarsApplication::getToolbarsApplication()->getCommandStack();
		bool hasRedoableCmds = cmdStack->hasRedoableItems();
		if ( true == hasRedoableCmds ) {
			itemToUpdate->setCaption( "Redo \"" + cmdStack->getCurrentRedoComand()->getName() + "\"" );
		}
		else {
			itemToUpdate->setCaption( "Nothing to Redo" );
		}
		itemToUpdate->setEnabled( hasRedoableCmds );
	}
}

void MainWindow::onHelpAbout( MenuItemEvent* e )
{
	ToolbarsAbout aboutDlg;

	aboutDlg.showModal();
	
}

