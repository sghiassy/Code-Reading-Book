//Layout.cpp
/**
*This application serves as a demonstration of control layout in the 
*VCF.
*/

#include "ApplicationKit.h"
#include "Panel.h"
#include "CommandButton.h"

using namespace VCF;


/**
*Our main window class
*/
class LayoutWindow : public Window {
public:
	LayoutWindow() {
		setCaption( "Layout" );
		setVisible( true );
		setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );

		/**
		*here we'll create a new Panel class
		*A Panel is a control class that can act as a 
		*container for other controls, and has a default 
		*border specified. The border draws a nice 3D 
		*edge around the control. In the VCF a control is 
		*determined to be a container when it implements 
		*the VCF::Container interface, which specifies 
		*methods for adding, removing, and resizing/repainting 
		*the child controls that it contains.
		*/
		Panel* p = new Panel();
		p->setHeight( 200 );

		/**
		*this is how we add a control to another parent control.
		*we can specify the alignment type here, or pass in nothing
		*accepting the value of the default parameter, which is 
		*ALIGN_NONE. In this case, the control will align itself
		*to the top of the 
		*/
		add( p, ALIGN_TOP );

		CommandButton* cb = new CommandButton();
		cb->setBounds( &Rect(20, 220, 100, 250) );
		add( cb );
		cb->setCaption( "Button1" );

		EventHandler* bh = 
			new ButtonEventHandler<LayoutWindow>(this, LayoutWindow::onBtnClick, "bh" );
		
		cb->addButtonClickHandler( bh );
		

		f = new Panel();		
		f->setColor( Color::getColor( "red" ) );
		f->setBounds( &Rect(110, 220, 200, 350) );
		f->setAnchor( ANCHOR_TOP | ANCHOR_RIGHT );

		add( f );

		
		
	}

	virtual ~LayoutWindow(){};

	void onBtnClick( ButtonEvent* e ) {		

		f->setBounds( &Rect(250, 280, 400, 400) );

	}
	Panel* f;
};




class LayoutApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new LayoutWindow();
		setMainWindow(mainWindow);		
		
		return result;
	}

};


int main(int argc, char *argv[])
{
	LayoutApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

