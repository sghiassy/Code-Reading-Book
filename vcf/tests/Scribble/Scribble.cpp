//Scribble.cpp


#include "ApplicationKit.h"
#include "CommandButton.h"
#include "Panel.h"
#include "Label.h"
#include "DefaultMenuItem.h"

using namespace VCF;

class ScribbleView : public CustomControl {
public:
	ScribbleView():
		CustomControl( false )
		{

	};

	virtual ~ScribbleView(){};

	virtual void paint( GraphicsContext* ctx ) {
		CustomControl::paint(ctx); 
		Color color(0.85f,0.85f,0.85f);
		ctx->setColor( &color );
		Rect r = *getBounds();
		r.setRect( 5, 5, getWidth() - 5, getHeight()-5 );
		ctx->rectangle( &r );
		ctx->fillPath();
	};

	virtual void mouseDown( MouseEvent* event ) {
		CustomControl::mouseDown( event );
		dragPt = *event->getPoint();
		GraphicsContext* ctx = this->getContext();
		ctx->moveTo( dragPt.m_x, dragPt.m_y );
	}

	virtual void mouseMove( MouseEvent* event )
	{
		CustomControl::mouseMove( event );
		if ( event->hasLeftButton() ) {
			Point* pt = event->getPoint();
			GraphicsContext* ctx = this->getContext();
			ctx->moveTo( dragPt.m_x, dragPt.m_y );
			dragPt = *pt;
			ctx->lineTo( dragPt.m_x, dragPt.m_y );
			ctx->setColor( Color::getColor( "red" ) );
			ctx->strokePath();	
		}
	}

	virtual void mouseUp( MouseEvent* event )
	{
		CustomControl::mouseUp( event );
	}

	Point dragPt;
};


class ScribbleWindow : public Window {
public:
	
	ScribbleWindow(){
		
		panel1 = new Panel();
		panel1->setBounds( &Rect(0,0,100,200) );
		this->add( panel1, ALIGN_RIGHT );

		panel2 = new Panel();
		add( panel2, ALIGN_CLIENT );
		
		btn1 = new CommandButton();
		

		label1 = new Label();
		label1->setCaption( "Commands" );
		label1->setBounds( &Rect(10, 10, 80, 35) );
		panel1->add( label1, ALIGN_TOP );

		btn1->setBounds( &Rect(10, 50, 80, 75) );
		panel1->add( btn1 );
		btn1->setCaption( "Clear" );
		btn1->setEnabled( false );
		btn2 = new CommandButton();
		btn2->setBounds( &Rect(10, 90, 80, 115) );
		panel1->add( btn2 );
		btn2->setCaption( "Exit" );

		scribbleView = new ScribbleView();
		panel2->add( scribbleView, ALIGN_CLIENT );
		
		MouseEventHandler<ScribbleWindow>* mh = 
			new MouseEventHandler<ScribbleWindow>( this, ScribbleWindow::onScribbleViewMouseUp, "MouseHandler" );		
		scribbleView->addMouseUpHandler ( mh );
	
		ButtonEventHandler<ScribbleWindow>* bh = 
			new ButtonEventHandler<ScribbleWindow>( this,ScribbleWindow::onBtn1Clicked,"ButtonHandler"  );
		
		btn1->addButtonClickHandler( bh );

		bh = new ButtonEventHandler<ScribbleWindow>( this,ScribbleWindow::onBtn2Clicked,"ButtonHandler2" );
		
		btn2->addButtonClickHandler( bh );

		this->setMenuBar( new MenuBar() );

		MenuBar* menuBar = this->getMenuBar();
		MenuItem* item = menuBar->getRootMenuItem();
		
		DefaultMenuItem* file = new DefaultMenuItem( "&File", item, menuBar ); 	

		DefaultMenuItem* fileExit = new DefaultMenuItem( "&Exit", file, menuBar ); 
		
		DefaultMenuItem* view = new DefaultMenuItem( "&View", item, menuBar ); 	

		viewClear = new DefaultMenuItem( "&Clear", view, menuBar ); 	

		MenuItemEventHandler<ScribbleWindow>* menuHandler = 
			new MenuItemEventHandler<ScribbleWindow>( this, ScribbleWindow::onFileExitClicked, "FileExit" );
		
		fileExit->addMenuItemClickedHandler( menuHandler );		

		MenuItemEventHandler<ScribbleWindow>* viewClearMenuHandler = 
			new MenuItemEventHandler<ScribbleWindow>( this, ScribbleWindow::onViewClearClicked, "viewClear" );		
		viewClear->addMenuItemClickedHandler( viewClearMenuHandler );
		

		PopupMenu* popup = new PopupMenu( this );
		
		DefaultMenuItem* popupRoot = new DefaultMenuItem( "root", NULL, popup );
		DefaultMenuItem* popupEditClear = new DefaultMenuItem( "&Clear", popupRoot, popup );

		popupEditClear->addMenuItemClickedHandler( viewClearMenuHandler );

		popup->setRootMenuItem( popupRoot );

		scribbleView->setPopupMenu( popup );

		setVisible( true );
	};

	virtual ~ScribbleWindow() {};

	void onScribbleViewMouseUp( MouseEvent* e ) {
		btn1->setEnabled( true );
		viewClear->setEnabled( true );
	}
	
	void onFileExitClicked( MenuItemEvent* e ) {
		this->close();
	}
	
	void onViewClearClicked( MenuItemEvent* e ) {
		btn1->setEnabled( false );
		viewClear->setEnabled( false );
		scribbleView->repaint();
	}

	void onBtn2Clicked( ButtonEvent* e ) {
		this->close();
	}

	void onBtn1Clicked( ButtonEvent* e ) {
		btn1->setEnabled( false );
		viewClear->setEnabled( false );
		scribbleView->repaint();
	}

	CommandButton* btn1;
	CommandButton* btn2;
	Panel* panel1;
	Panel* panel2;	
	Label* label1;
	ScribbleView* scribbleView;
	DefaultMenuItem* viewClear;
};


class ScribbleApplication : public Application {
public:

	virtual bool initRunningApplication(){
		StringUtils::trace( "initRunningApplication\n" );
		bool result = Application::initRunningApplication();
		
		Window* helloWorldWindow = new ScribbleWindow();
		setMainWindow(helloWorldWindow);
		helloWorldWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		helloWorldWindow->setCaption( "Scribble" );
		

		return result;
	}

};


int main(int argc, char *argv[])
{
	StringUtils::trace( "main\n" );
	ScribbleApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

