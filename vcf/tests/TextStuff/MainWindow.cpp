//MainWindow.h
#include "ApplicationKit.h"
#include "MainWindow.h"
#include "DefaultMenuItem.h"

#include "TextStuffApplication.h"
#include "Command.h"

#include "TextStuffAbout.h"

using namespace VCF;



MainWindow::MainWindow()
{
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
}

MainWindow::~MainWindow()
{
	
}
void MainWindow::onFileOpenProject( MenuItemEvent* e )
{
	CommonFileOpen openDlg;	
	openDlg.addFilter( "Text Files", "*.txt" );
	openDlg.addFilter( "All Files", "*.*" );
	if ( true == openDlg.execute() ) {
		
	}
	
}

void MainWindow::onFileSaveProject( MenuItemEvent* e )
{
	CommonFileSave saveDlg;
	saveDlg.addFilter( "Text Files", "*.txt" );
	if ( true == saveDlg.execute() ) {
		
	}
}

void MainWindow::onFileExit( MenuItemEvent* e )
{
	close();
}

void MainWindow::onEditUndo( MenuItemEvent* e )
{
	UndoRedoStack* cmdStack = TextStuffApplication::getTextStuffApplication()->getCommandStack();
	cmdStack->undo();
}

void MainWindow::onEditUndoUpdate( VCF::MenuItemEvent* e )
{
	MenuItem* itemToUpdate = dynamic_cast<MenuItem*>(e->getSource());
	if ( NULL != itemToUpdate ) {
		UndoRedoStack* cmdStack = TextStuffApplication::getTextStuffApplication()->getCommandStack();
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
	UndoRedoStack* cmdStack = TextStuffApplication::getTextStuffApplication()->getCommandStack();
	cmdStack->redo();
}

void MainWindow::onEditRedoUpdate( VCF::MenuItemEvent* e )
{
	MenuItem* itemToUpdate = dynamic_cast<MenuItem*>(e->getSource());
	if ( NULL != itemToUpdate ) {
		UndoRedoStack* cmdStack = TextStuffApplication::getTextStuffApplication()->getCommandStack();
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
	TextStuffAbout aboutDlg;

	aboutDlg.showModal();
	
}

