//Resources.cpp


#include "ApplicationKit.h"


using namespace VCF;


class ResourcesWindow : public Window {
public:
	ResourcesWindow() {
		setCaption( "Resources" );
		setVisible( true );
	}

	virtual ~ResourcesWindow(){};

	virtual void paint( GraphicsContext* ctx ) {
		Window::paint( ctx );

		//get the image resource from the running application's resource bundle
		Image* img = Application::getRunningInstance()->getResourceBundle()->getImage( "IDB_BITMAP1" );
		ctx->drawString( 100, 80, "Here's a sample resource image" );

		//draw the image
		ctx->drawImage( 100, 100, img );

		double y = 100 + img->getHeight() + 20;

		//get the string resource from the running application's resource bundle
		String resString = Application::getRunningInstance()->getResourceBundle()->getString( "STRING1" );

		ctx->drawString( 100, y, "Here's a sample resource image" );

		//draw the string
		ctx->drawString( 100, y+20, resString );

		delete img;
	}
};




class ResourcesApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new ResourcesWindow();
		setMainWindow(mainWindow);
		mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		
		return result;
	}

};


int main(int argc, char *argv[])
{
	ResourcesApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

