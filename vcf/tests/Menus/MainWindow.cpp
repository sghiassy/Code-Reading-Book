//MainWindow.h
#include "ApplicationKit.h"

#include "MainWindow.h"
#include "DefaultMenuItem.h"
#include "MenusApplication.h"
#include "Command.h"

#include "MenusAbout.h"

using namespace VCF;


/**
*Lets make a simple class that demonstrates 
*customizing the drawing of menu items
*We'll derive from DefaultMenuItem because that has 
*all of the core functionality already implemented
*In our class we only need to override 3 methods:
*getBounds() to return our own internal bounds data
*
*canPaint() which will return true, indicating that 
*instance of this class can paint themselves
*
*and finally paint(), where we provide our
*own custom paint to the menu item
*/
class DrawMenuItem : public DefaultMenuItem {
public:
	//keeps the constrcutor identical to the derived classes
	DrawMenuItem( const String& caption, MenuItem* parent=NULL, Menu* menuOwner=NULL ):
	  //we pass in a NULL for the parent item to the derived class
	  //because we will add ourselves to the parent
	  DefaultMenuItem(caption, NULL, menuOwner ){
		  
		  if ( NULL != parent ) {
			  parent->addChild( this );
		  }
		  m_imageList = NULL;
		  // we can leave the m_bounds member alone
		  //as it will be initialized to zero.
		  //by doing this, we will let them system
		  //automatically determine the correct size for 
		  //the menu item
	  }
	  
	  virtual ~DrawMenuItem(){};
	  
	  //return true to indicate that we can paint ourselves
	  virtual bool canPaint() {
		  return true;    
	  }
	  
	  virtual Rect* getBounds() {
		  return &m_bounds;
	  }
	  
	  //the paint method will paint an image from the 
	  virtual void paint( GraphicsContext* context, Rect* paintRect ) {
		  if ( NULL != m_imageList ) {
			  Rect r = *paintRect;                    
			  
			  if ( isSelected() ) {
				  Light3DBorder bdr;
				  r.m_right = r.m_left + paintRect->getHeight();
				  
				  bdr.paint( &r, context );
				  
				  r = *paintRect;
			  }
			  
			  r.m_left = paintRect->getHeight()/2.0 - m_imageList->getImageWidth()/2.0;
			  r.m_right = r.m_left + m_imageList->getImageWidth();
			  r.m_top = (paintRect->m_top + paintRect->getHeight()/2.0) - m_imageList->getImageHeight()/2.0;
			  r.m_bottom = r.m_top + m_imageList->getImageHeight();
			  
			  m_imageList->draw( context, getImageIndex(), &r );                      
		  }
	  }
	  
	  void setImageList( ImageList* il ) {
		  m_imageList = il;
	  }
	  Rect m_bounds;
	  ImageList* m_imageList;
};



MainWindow::MainWindow()
{
	ImageList* il = new ImageList();
	il->setImageHeight( 16 );
	il->setImageWidth( 16 );
	il->setTransparentColor( &Color(1.0f, 0.0f, 1.0f) );
	
	ResourceBundle* resBdl = Application::getRunningInstance()->getResourceBundle();
	Image* img = resBdl->getImage( "FILE_OPEN" );
	il->addImage( img ); 
	delete img;
	
	img = resBdl->getImage( "FILE_SAVE" );
	il->addImage( img ); 
	delete img;
	
	this->addComponent( il );
	
	//build main menu
	//first create a menu bar to house the menu items
	MenuBar* menuBar = new MenuBar();

	//set the menu bar on the main window
	this->setMenuBar( menuBar );

	//get the root item of the menu bar
	//this will be used to attach all the other sub menu items
	MenuItem* root = menuBar->getRootMenuItem();

	//the menu item handler
	MenuItemEventHandler<MainWindow>* menuItemHandler = NULL;

	//create the top level "File" menu item
	DefaultMenuItem* file = new DefaultMenuItem( "&File", root, menuBar );

	//create the "Open" menu item and attach a menu item handler
	DrawMenuItem* fileOpenProject = new DrawMenuItem( "&Open...", file, menuBar );
	fileOpenProject->setEnabled( false );
	fileOpenProject->setImageIndex( 0 );
	fileOpenProject->setImageList( il );
	
	//creates the new handler
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileOpen,"fileOpen" );	
	fileOpenProject->addMenuItemClickedHandler( menuItemHandler );
	

	DrawMenuItem* fileSaveProject = new DrawMenuItem( "&Save...", file, menuBar );
	fileSaveProject->setImageIndex( 1 );
	fileSaveProject->setImageList( il );

	menuItemHandler = new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileSave,"fileSave" );	
	fileSaveProject->addMenuItemClickedHandler( menuItemHandler );
	

	DefaultMenuItem* sep = new DefaultMenuItem( "", file, menuBar );
	sep->setSeparator( true );
	
	DefaultMenuItem* fileExit = new DefaultMenuItem( "E&xit", file, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this,MainWindow::onFileExit,"fileExit");
	fileExit->addMenuItemClickedHandler( menuItemHandler );	

	//add Help menu
	DefaultMenuItem* help = new DefaultMenuItem( "&Help", root, menuBar );
	DefaultMenuItem* helpAbout = new DefaultMenuItem( "About...", help, menuBar );
	menuItemHandler = new MenuItemEventHandler<MainWindow>(this,MainWindow::onHelpAbout,"helpAbout");	
	helpAbout->addMenuItemClickedHandler( menuItemHandler );
	
}

MainWindow::~MainWindow()
{
	
}

void MainWindow::onFileOpen( MenuItemEvent* e )
{
	CommonFileOpen openDlg;	
	openDlg.addFilter( "Foo Files", "*.foo" );
	openDlg.addFilter( "Bar Files", "*.bar" );
	openDlg.addFilter( "All Files", "*.*" );
	if ( true == openDlg.execute() ) {
		
	}
	
}

void MainWindow::onFileSave( MenuItemEvent* e )
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

void MainWindow::onHelpAbout( MenuItemEvent* e )
{
	MenusAbout aboutDlg;

	aboutDlg.showModal();
	
}

