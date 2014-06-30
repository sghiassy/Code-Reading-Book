//MainWindow.h
#include "ApplicationKit.h"
#include "MainWindow.h"
#include "DefaultMenuItem.h"

#include "ErrorLoggingApplication.h"
#include "Command.h"


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
}

MainWindow::~MainWindow()
{
	
}
void MainWindow::onFileOpenProject( MenuItemEvent* e )
{
	CommonFileOpen openDlg;	
	openDlg.addFilter( "Text Files", "*.txt" );
	openDlg.addFilter( "OPP Files", "*.opp" );
	openDlg.addFilter( "All Files", "*.*" );
	if ( true == openDlg.execute() ) {
		
	}
	
}

void MainWindow::onFileSaveProject( MenuItemEvent* e )
{
	CommonFileSave saveDlg;
	saveDlg.addFilter( "Text Files", "*.txt" );
	saveDlg.addFilter( "OPP Files", "*.opp" );
	if ( true == saveDlg.execute() ) {
		
	}
}

void MainWindow::onFileExit( MenuItemEvent* e )
{
	close();
}

