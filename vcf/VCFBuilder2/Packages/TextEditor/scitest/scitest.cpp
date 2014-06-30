//scitest.cpp


#include "ApplicationKit.h"
#include "Win32Peer.h"
#include "Win32ToolKit.h"
#include "AbstractWin32Component.h"

using namespace VCF;

static AbstractWin32Component* sciWnd = NULL;
WNDPROC sciWndProc = NULL;

LRESULT CALLBACK SciWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	Win32Object* w32O = Win32Object::getWin32ObjectFromHWND( hWnd );
	if ( NULL != w32O ) {
		result = w32O->handleEventMessages( message, wParam, lParam );
	}

	result = CallWindowProc( sciWndProc, hWnd, message, wParam, lParam );
	return result;
}

class ScintillaPeer : public AbstractWin32Component { 
public:
	ScintillaPeer() {
		m_scintillaWndProc = NULL;
		Win32ToolKit* toolkit = (Win32ToolKit*)UIToolkit::getDefaultUIToolkit();
		HWND parent = toolkit->getDummyParent();	
		
		this->m_styleMask = WS_CHILD | WS_VISIBLE;//| WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		this->m_exStyleMask = 0;
		
		
		
		m_hwnd = ::CreateWindowEx( m_exStyleMask, "Scintilla", "", m_styleMask, 0, 0, 0, 0,
			parent, NULL, ::GetModuleHandle(NULL), 0 );//(HINSTANCE)textLib->getPeer()->getHandleID(), 0);
		
		if ( NULL != m_hwnd ) {
			Win32Object::registerWin32Object( this );
			m_defaultWndProc = NULL;
			//m_scintillaWndProc
			//sciWndProc 
			m_scintillaWndProc = (WNDPROC)::SetWindowLong( this->m_hwnd, GWL_WNDPROC, (LONG)SciWndProc );//m_wndProc ); 
			sciWnd = this;
			sciWndProc = m_scintillaWndProc;
			//this->m_defaultWndProc = NULL;
		}	
	}

	virtual ~ScintillaPeer(){};

	/**
     * returns a ID that identifies this control. On Win32 this corresponds to an HWND. 
     */
    virtual long getHandleID(){
		return AbstractWin32Component::getHandleID();	
	};

    /**
     * returns a text associated with the component. This usually gets used in the Component::getCaption() method. 
     */
    virtual VCF::String getText(){
		return AbstractWin32Component::getText();	
	};

    /**
     * sets the text for the widget 
     */
    virtual void setText( const VCF::String& text ){
		AbstractWin32Component::setText( text );		
	};

    /**
     * sets the bounds for the component. Bounds are specified in the coordinate system of the componenents parent. 
     */
    virtual void setBounds( VCF::Rect* rect ){
		AbstractWin32Component::setBounds( rect );	
	};

    /**
     * returns the bounds of the component in the coordinate system of the parent. 
     */
    virtual VCF::Rect* getBounds(){
		return AbstractWin32Component::getBounds();	
	};

    /**
     * shows or hides the component. 
     * This does NOT close the component (if invoked on a frame based component ). 
     */
    virtual void setVisible( const bool& visible ){
		AbstractWin32Component::setVisible( visible );	
	};

    /**
     * returns wether or not the component is currently visible. 
     */
    virtual bool getVisible(){
		return AbstractWin32Component::getVisible();	
	};

    /**
     * returns a bit-masked unsigned long that contains style constants. 
     *  These style constants are defined in the VCF, and must 
     * be translated to the particular windowing system being used. 
     */
    virtual unsigned long getStyleMask(){
		return AbstractWin32Component::getStyleMask();	
	};

    /**
     * sets the current style mask.
     *  Should cause a repaint of the component, if neccessary. 
     */
    virtual void setStyleMask( const unsigned long& styleMask ){
		AbstractWin32Component::setStyleMask( styleMask );	
	};

    /**
     * returns the component that this Peer is attached to. 
     */
    virtual VCF::Control* getControl(){
		return AbstractWin32Component::getControl();	
	};

    /**
     * attahces the Peer to a particular component. This should only be done once. 
     */
    virtual void setControl( VCF::Control* component ){
		AbstractWin32Component::setControl( component );	
	};

    virtual void getCursor(){
		AbstractWin32Component::getCursor();	
	};

    virtual void setCursor(){
		AbstractWin32Component::setCursor();	
	};    

	virtual void setParent( VCF::Control* parent ){
		AbstractWin32Component::setParent( parent );	
	};

	virtual VCF::Control* getParent(){
		return AbstractWin32Component::getParent();	
	};

	virtual bool isFocused(){
		return AbstractWin32Component::isFocused();	
	};

	virtual void setFocus( const bool& focused ){
		AbstractWin32Component::setFocus( focused );
	};

	virtual bool isEnabled(){
		return AbstractWin32Component::isEnabled();	
	};

	virtual void setEnabled( const bool& enabled ){
		AbstractWin32Component::setEnabled( enabled );	
	};

	virtual LRESULT handleEventMessages( UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultWndProc = NULL ) {
		LRESULT result = 0;
		/*
		switch ( message ) {
			case WM_PAINT : {
				result = CallWindowProc( m_scintillaWndProc, m_hwnd, message, wParam, lParam );
			}
			break;

			default : {
				if ( message > WM_USER ) {
					//result = CallWindowProc( m_scintillaWndProc, m_hwnd, message, wParam, lParam );
				}
				//result = AbstractWin32Component::handleEventMessages( message, wParam, lParam, defaultWndProc);

				
				if ( !result ) {
					
				}
			}
			break;
		}
		*/
		result = AbstractWin32Component::handleEventMessages( message, wParam, lParam, defaultWndProc);
		return result;
	}
protected:

private:
	WNDPROC m_scintillaWndProc;
};


class SciEditor : public Control {
public:
	SciEditor() {
		m_Peer = new ScintillaPeer();

		m_Peer->setControl( this );

		
	}

	virtual void paint( GraphicsContext* g ) {

	};

	virtual void afterCreate( ComponentEvent* event ) {

	}

	virtual bool isLightWeight() {
		return false;
	}
};

class Editor : public CustomControl {
public:

	Editor() {
		setColor( Color::getColor( "white" ) );

		sci = CreateWindowEx( 0, "Scintilla", "", WS_CHILD | WS_VISIBLE, 0,0,0,0, (HWND)getPeer()->getHandleID(),
								NULL, (HINSTANCE)Application::getRunningInstance()->getPeer()->getHandleID(), 0 );					

		
		
	}

	virtual ~Editor(){};

	

	virtual void setParent( Control* c ) {
		CustomControl::setParent( c );
		SetParent( sci, (HWND)getPeer()->getHandleID() );
	}

	void onParentChanged( ControlEvent* e ) {

	}

	virtual void setBounds( Rect* r ) {
		CustomControl::setBounds( r );
		MoveWindow( sci, 0, 0, r->getWidth(), r->getHeight(), TRUE );
	}

	HWND sci;
};


class scitestWindow : public Window {
public:
	scitestWindow() {
		setCaption( "Hello World !!!!" );
		setVisible( true );

		Control* ed = new SciEditor();
		add( ed, ALIGN_CLIENT );

		MouseHandler* mh = new MouseHandler( this );
		mh->m_mouseDown = (OnMouseEvent)scitestWindow::onMouseDown;
		addEventHandler( "MouseHandler", mh );
		ed->addMouseListener( mh );

		ControlHandler<scitestWindow>* ch = new ControlHandler<scitestWindow>( this );
		ch->setMethodPtr( scitestWindow::onSized, 0 );
		addEventHandler( "ControlHandler", ch );
		ed->addControlListener( ch );

	}

	virtual ~scitestWindow(){};

	void onSized( ControlEvent* e ) {

	}

	void onMouseDown( MouseEvent* e ) {

	}
};




class scitestApplication : public Application {
public:

	virtual void terminateRunningApplication() {
		delete lib;
	}

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		lib = new Library( "SciLexer.DLL" );

		Window* mainWindow = new scitestWindow();
		setMainWindow(mainWindow);
		mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	
		
		return result;
	}

	Library* lib;
};


int main(int argc, char *argv[])
{
	scitestApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

