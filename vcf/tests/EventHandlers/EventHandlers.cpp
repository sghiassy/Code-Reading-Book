//EventHandlers.cpp
/**
*This tutorial will show you how to respond to
*events in the VCF framework
*/

#include "ApplicationKit.h"
#include "Panel.h"
#include "Label.h"
#include "TextControl.h"

using namespace VCF;


class EventHandlersWindow : public Window {
public:
	EventHandlersWindow() {
		
		setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );

		setCaption( "EventHandlers" );
		setVisible( true );

		m_bottomPanel = new Panel();
		m_bottomPanel->setHeight(25);
		add( m_bottomPanel, ALIGN_BOTTOM );

		m_panelLabel = new Label();
		m_bottomPanel->add( m_panelLabel, ALIGN_CLIENT );

		CommandButton* button1 = new CommandButton();
		button1->addButtonClickHandler( 
			new ButtonEventHandler<EventHandlersWindow>(this, EventHandlersWindow::onButton1Clicked, "button1ClickHandler") );
		
		EventHandler* posChanged = 
			new ControlEventHandler<EventHandlersWindow>( this, 
															EventHandlersWindow::onButton1PosChanged,
															"buttonPosChangedHandler" );
		button1->addControlPositionedHandler( posChanged );
		button1->addControlSizedHandler( posChanged );
		
		button1->setTop( 25 );
		button1->setLeft( (getWidth() - 20) - button1->getPreferredWidth() );
		button1->setHeight( button1->getPreferredHeight() );
		button1->setWidth( button1->getPreferredWidth() );
		button1->setAnchor( ANCHOR_RIGHT | ANCHOR_TOP );
		add( button1 );
		button1->setCaption( "Click Me!" );		


		Label* label1 = new Label();
		label1->setCaption( "Text Control: " );
		label1->setBounds( &Rect(25, 25, 100, 25 + label1->getPreferredHeight()) );
		add( label1 );

		TextControl* tc = new TextControl();
		tc->addKeyPressedHandler( 
			new KeyboardEventHandler<EventHandlersWindow>( this,
															EventHandlersWindow::onTextControlKeyPressed,
															"EventHandlersWindow::onTextControlKeyPressed" ) );
		tc->setVisible( true );
		tc->setBounds( &Rect(105, 25, 180, 25 + label1->getPreferredHeight()) );
		add( tc );
	}

	virtual ~EventHandlersWindow(){};

	void onButton1Clicked( ButtonEvent* e ) {
		Dialog::showMessage( "Button1Clicked \n" + e->getSource()->toString() );
	}

	void onButton1PosChanged( ControlEvent* e ) {
		Control* c = (Control*)e->getSource();
	
		String s = c->getBounds()->toString();

		m_panelLabel->setCaption( s );
	}

	void onTextControlKeyPressed( KeyboardEvent* e ) {
		String s = StringUtils::format( "key pressed: %c", e->getKeyValue() );

		m_panelLabel->setCaption( s );
	}
	Panel* m_bottomPanel;
	Label* m_panelLabel;
};




class EventHandlersApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new EventHandlersWindow();
		setMainWindow(mainWindow);
		
		
		return result;
	}

};


int main(int argc, char *argv[])
{
	StringUtils::traceWithArgs( "sizeof(EventHandlersWindow): %d\n", sizeof(EventHandlersWindow) );
	EventHandlersApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

