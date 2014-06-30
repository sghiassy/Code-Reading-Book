//TreeList.cpp


#include "ApplicationKit.h"
#include "Window.h"
#include "Rect.h"
#include "TreeListControl.h"

using namespace VCF;


class TreeListApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* helloWorldWindow = new Window();
		setMainWindow(helloWorldWindow);
		helloWorldWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		helloWorldWindow->setCaption( "Hello World !!!!" );

		TreeListControl* tlc = new TreeListControl();
		tlc->setBounds( &Rect(25, 25, 300, 300 ) );
		tlc->setVisible( true );
		DefaultColumnHeaderItem* dci = new DefaultColumnHeaderItem();
		dci->setCaption( "Column2" );
		tlc->addColumnHeaderItem( dci );
		helloWorldWindow->add( tlc );

		helloWorldWindow->setVisible( true );
		return result;
	}

};


int main(int argc, char *argv[])
{
	TreeListApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

