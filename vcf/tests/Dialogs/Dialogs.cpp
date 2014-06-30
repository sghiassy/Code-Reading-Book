//Dialogs.cpp


#include "ApplicationKit.h"
#include "Window.h"
#include "Rect.h"

using namespace VCF;


class TestDialog : public Dialog {
public:
	TestDialog() 	{
		setWidth( 400 );
		setHeight( 400 );
		OKBtn = new CommandButton();
		OKBtn->setLeft( 10 );
		OKBtn->setTop( 10 );	

		add( OKBtn );
	}

	virtual ~TestDialog() {
		
	}

	CommandButton* OKBtn;
	CommandButton* CancelBtn;	
};

class DialogsApplication : public Application {
public:

	virtual bool initRunningApplication();

	void showModalDialog( ButtonEvent* e );

	void showModelessDialog( ButtonEvent* e );
};

class MainWindow : public Window {
public:
	MainWindow(DialogsApplication* app) {
		btn1 = new CommandButton();
		btn1->setCaption( "&Modal Dialog" );
		btn1->setBounds( &Rect( 50, 50, 150, 75 ) );
		add( btn1 );

		btn2 = new CommandButton();
		btn2->setCaption( "Mode&less Dialog" );
		btn2->setBounds( &Rect( 175, 50, 325, 75 ) );
		add( btn2 );

		ButtonEventHandler<DialogsApplication>* bh = 
			new ButtonEventHandler<DialogsApplication>( app, DialogsApplication::showModalDialog, "btn1Handler" );
		
		btn1->addButtonClickHandler( bh );
		

		bh = new ButtonEventHandler<DialogsApplication>( app, DialogsApplication::showModelessDialog, "btn2Handler" );		
		btn2->addButtonClickHandler( bh );

	}

	virtual ~MainWindow() {

	}


	CommandButton* btn1;
	CommandButton* btn2;
};


bool DialogsApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	
	Window* helloWorldWindow = new MainWindow(this);
	setMainWindow(helloWorldWindow);
	helloWorldWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	helloWorldWindow->setCaption( "Hello World !!!!" );
	helloWorldWindow->show();
	return result;
}

void DialogsApplication::showModalDialog( ButtonEvent* e ) {
	TestDialog* dlg = new TestDialog();
	dlg->showModal();
	delete dlg;
}

void DialogsApplication::showModelessDialog( ButtonEvent* e )
{
	TestDialog* dlg = new TestDialog();
	dlg->show();
}

int main(int argc, char *argv[])
{
	DialogsApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

