//MainWindow.h
#include "ApplicationKit.h"
#include "MainWindow.h"
#include "Splitter.h"
#include "MenuBar.h"
#include "DefaultMenuItem.h"
#include "NewProjectDlg.h"
#include "Command.h"
#include "MenuItemEvent.h"
#include "VCFBuilderUI.h"
#include "MainUIPanel.h"
#include "MenuItemEvent.h"
#include "StatusBar.h"

using namespace VCF;

using namespace VCFBuilder;


MainWindow::MainWindow()
{
	m_mainUI = NULL;
	setVisible( false );	
	this->m_statusBar = NULL;
}

MainWindow::~MainWindow()
{
	
}

void MainWindow::onMenuItemSelectionChanged( ItemEvent* e )
{
	MenuItem* item = dynamic_cast<MenuItem*>(e->getSource());
	m_statusBar->setStatusCaption( item->getCaption() );
}

void MainWindow::onFileNewProject( MenuItemEvent* e )
{
	m_mainUI->newProject( e );
}

void MainWindow::onFileOpenProject( MenuItemEvent* e )
{
	m_mainUI->openProject( e );
}

void MainWindow::onFileSaveProject( MenuItemEvent* e )
{
	m_mainUI->saveProject( e );
}

void MainWindow::onFileSaveProjectAs( MenuItemEvent* e )
{
	m_mainUI->saveProjectAs( e );
}

void MainWindow::onFileCloseProject( MenuItemEvent* e )
{
	m_mainUI->closeProject( e );
}

void MainWindow::onFileNewForm( MenuItemEvent* e )
{	
	m_mainUI->newForm( e );
}

void MainWindow::onFileSaveForm( MenuItemEvent* e )
{
	m_mainUI->saveForm( e );
}

void MainWindow::onFileSaveFormAs( MenuItemEvent* e )
{
	m_mainUI->saveFormAs( e );
}

void MainWindow::onFileExit( MenuItemEvent* e )
{
	close();
}

void MainWindow::onEditUndo( MenuItemEvent* e )
{
	m_mainUI->editUndo( e );	
}

void MainWindow::onEditUndoUpdate( VCF::MenuItemEvent* e )
{
	MenuItem* itemToUpdate = dynamic_cast<MenuItem*>(e->getSource());
	if ( NULL != itemToUpdate ) {
		UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
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
	m_mainUI->editRedo( e );	
}

void MainWindow::onEditRedoUpdate( VCF::MenuItemEvent* e )
{
	MenuItem* itemToUpdate = dynamic_cast<MenuItem*>(e->getSource());
	if ( NULL != itemToUpdate ) {
		UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
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

void MainWindow::onEditCut( MenuItemEvent* e )
{
	m_mainUI->cut( e );
}

void MainWindow::onEditCopy( MenuItemEvent* e )
{	
	m_mainUI->copy( e );
}

void MainWindow::onEditPaste( MenuItemEvent* e )
{
	m_mainUI->paste( e );
}

void MainWindow::onEditSelectAll( MenuItemEvent* e )
{
	m_mainUI->selectAll( e );
}

void MainWindow::onEditDelete( MenuItemEvent* e )
{
	m_mainUI->deleteSelection( e );
}

void MainWindow::onEditPreferences( MenuItemEvent* e )
{
	m_mainUI->editPreferences( e );
}

void MainWindow::onEditProjectSettings( MenuItemEvent* e )
{
	m_mainUI->editProjectSettings( e );
}

void MainWindow::afterCreate( ComponentEvent* event )
{	

	m_statusBar = new StatusBar();
	m_statusBar->setHeight( 25 );
	add( m_statusBar, ALIGN_BOTTOM );

	VCFBuilderUIApplication::getVCFBuilderUI()->initMainWindow();

	m_mainUI = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();

	add( m_mainUI, ALIGN_CLIENT );

	//build main menu
	MenuBar* menuBar = new MenuBar();
	this->setMenuBar( menuBar );
	MenuItem* root = menuBar->getRootMenuItem();
	DefaultMenuItem* file = new DefaultMenuItem( "&File", root, menuBar );
	
	DefaultMenuItem* fileNewProject = new DefaultMenuItem( "&New Project...", file, menuBar );
	MenuItemEventHandler<MainWindow>* menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileNewProject,"fileNewProject" );
	
	fileNewProject->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* fileOpenProject = new DefaultMenuItem( "&Open Project...", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileOpenProject,"fileOpenProject" );	
	fileOpenProject->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* fileSaveProject = new DefaultMenuItem( "&Save Project...", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileSaveProject,"fileSaveProject");	
	fileSaveProject->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* fileSaveProjectAs = new DefaultMenuItem( "Save Project &As...", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileSaveProjectAs,"fileSaveProjectAs" );	
	fileSaveProjectAs->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* fileCloseProject = new DefaultMenuItem( "&Close Project", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileCloseProject,"fileCloseProject");	
	fileCloseProject->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* sep = new DefaultMenuItem( "", file, menuBar );
	sep->setSeparator( true );

	DefaultMenuItem* fileNewForm = new DefaultMenuItem( "New Form...", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileNewForm,"fileNewForm");	
	fileNewForm->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* fileSaveForm = new DefaultMenuItem( "Save Form...", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileSaveForm,"fileSaveForm");	
	fileSaveForm->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* fileSaveFormAs = new DefaultMenuItem( "Save Form As...", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileSaveFormAs,"fileSaveFormAs");	
	fileSaveFormAs->addMenuItemClickedHandler( menuItemHandler );
	

	sep = new DefaultMenuItem( "", file, menuBar );
	sep->setSeparator( true );

	DefaultMenuItem* fileExit = new DefaultMenuItem( "E&xit", file, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileExit,"fileExit");	
	fileExit->addMenuItemClickedHandler( menuItemHandler );
	


	DefaultMenuItem* edit = new DefaultMenuItem( "&Edit", root, menuBar );
	DefaultMenuItem* editUndo = new DefaultMenuItem( "Undo", edit, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditUndo,"editUndoClick");
	editUndo->addMenuItemClickedHandler( menuItemHandler );

	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditUndoUpdate,"editUndoUpdate");
	editUndo->addMenuItemUpdateHandler( menuItemHandler );

	DefaultMenuItem* editRedo = new DefaultMenuItem( "Redo", edit, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditRedo,"editRedoClick");
	editRedo->addMenuItemClickedHandler( menuItemHandler );

	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditRedoUpdate,"editRedoUpdate");
	editRedo->addMenuItemUpdateHandler( menuItemHandler );	
	
	sep = new DefaultMenuItem( "", edit, menuBar );
	sep->setSeparator( true );

	DefaultMenuItem* editCut = new DefaultMenuItem( "Cu&t", edit, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditCut,"editCut");
	editCut->addMenuItemClickedHandler( menuItemHandler );	

	DefaultMenuItem* editCopy = new DefaultMenuItem( "&Copy", edit, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditCopy,"editCopy");	
	editCopy->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* editPaste = new DefaultMenuItem( "&Paste", edit, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditPaste,"editPaste");	
	editPaste->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* editDelete = new DefaultMenuItem( "&Delete", edit, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditDelete,"editDelete");	
	editDelete->addMenuItemClickedHandler( menuItemHandler );
	

	sep = new DefaultMenuItem( "", edit, menuBar );
	sep->setSeparator( true );

	DefaultMenuItem* editSelectAll = new DefaultMenuItem( "&Select All", edit, menuBar );
	menuItemHandler = 
		new MenuItemEventHandler<MainWindow>(this,MainWindow::onEditSelectAll,"editSelectAll");	
	editSelectAll->addMenuItemClickedHandler( menuItemHandler );
	
	
	sep = new DefaultMenuItem( "", edit, menuBar );
	sep->setSeparator( true );

	DefaultMenuItem* editPreferences = new DefaultMenuItem( "Preferences...", edit, menuBar );

	DefaultMenuItem* editProject = new DefaultMenuItem( "Project Settings...", edit, menuBar );

	DefaultMenuItem* view = new DefaultMenuItem( "&View", root, menuBar );
	DefaultMenuItem* components = new DefaultMenuItem( "&Components", root, menuBar );
	DefaultMenuItem* help = new DefaultMenuItem( "&Help", root, menuBar );

	ItemEventHandler<MainWindow>* ih = 
		new ItemEventHandler<MainWindow>(this,MainWindow::onMenuItemSelectionChanged,"MenuItemEventHandler");	

	fileNewProject->addItemChangedHandler( ih );
	fileOpenProject->addItemChangedHandler( ih );
	fileSaveProject->addItemChangedHandler( ih );
	fileSaveProjectAs->addItemChangedHandler( ih );
	fileNewForm->addItemChangedHandler( ih );
	fileSaveForm->addItemChangedHandler( ih );
	fileSaveFormAs->addItemChangedHandler( ih );
	fileExit->addItemChangedHandler( ih );
	editUndo->addItemChangedHandler( ih );
	editRedo->addItemChangedHandler( ih );
	editCut->addItemChangedHandler( ih );
	editCopy->addItemChangedHandler( ih );
	editPaste->addItemChangedHandler( ih );
	editSelectAll->addItemChangedHandler( ih );
	editDelete->addItemChangedHandler( ih );
	editPreferences->addItemChangedHandler( ih );
	editProject->addItemChangedHandler( ih );		
	
	setVisible( true );
}

void MainWindow::setActiveForm( Form* activeForm )
{
	m_mainUI->setActiveForm( activeForm );
}

Form* MainWindow::getActiveForm()
{
	return m_mainUI->getActiveForm();
}

Frame* MainWindow::getActiveFrameControl()
{
	return m_mainUI->getActiveFrameControl();
}

FormView* MainWindow::getFormViewer()
{
	return m_mainUI->getFormViewer();
}

ObjectExplorer* MainWindow::getObjectExplorer()
{
	return m_mainUI->getObjectExplorer();
}




