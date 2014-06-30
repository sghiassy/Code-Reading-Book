//Trees.cpp


#include "ApplicationKit.h"
#include "TreeControl.h"
#include "Splitter.h"

using namespace VCF;


class TreesWindow : public Window {
public:
	TreesWindow() {
		setCaption( "Hello World !!!!" );


		TreeControl* tree1 = new TreeControl();
		tree1->setWidth( 200 );
		tree1->setVisible( true );
		add(tree1, ALIGN_LEFT );

		Splitter* vertSplitter = new Splitter();
		add( vertSplitter, ALIGN_LEFT );

		TreeControl* tree2 = new TreeControl();
		tree2->setVisible( true );
		add( tree2, ALIGN_CLIENT );

		setVisible( true );
	}

	virtual ~TreesWindow(){};

};




class TreesApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new TreesWindow();
		setMainWindow(mainWindow);
		mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );

		return result;
	}

};


int main(int argc, char *argv[])
{
	TreesApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

