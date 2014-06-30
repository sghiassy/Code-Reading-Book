//SplashScreen.h
#include "ApplicationKit.h"
#include "SplashScreen.h"
#include "Color.h"

using namespace VCF;



SplashScreen::SplashScreen()
{
	setTop( 400 );
	setLeft( 300 );

	m_splashImage = Application::getRunningInstance()->getResourceBundle()->getImage( "SPLASH" );
	if ( NULL != m_splashImage ) {
		setHeight( m_splashImage->getHeight() );
		setWidth( m_splashImage->getWidth() );
	}
	else {
		setWidth( 250 );
		setHeight( 300 );
	}

	setVisible( true );	

	setFrameStyle( FST_NOBORDER_FIXED );
	setFrameTopmost( true );
	setColor( Color::getColor( "white" ) );

	m_thread = new ThreadLooper( this );
	m_thread->start();
}


SplashScreen::~SplashScreen()
{
	if ( NULL != m_splashImage ) {
		delete m_splashImage;
		m_splashImage = NULL;
	}
}


void SplashScreen::paint( GraphicsContext* context )
{
	Window::paint( context );
	Rect bounds(0,0,getWidth(), getHeight() );
	context->rectangle( &bounds );
	context->fillPath();

	if ( NULL != m_splashImage ) {
		context->drawImage( 0, 0, m_splashImage );
	}
	context->drawString( bounds.getWidth()/2-50, bounds.getHeight()/2.0, "Welcome to the $$Root$$..." );
}

static int sleepCount = 0;

bool SplashScreen::run()
{	
	repaint();
	m_thread->sleep( 500 );
	sleepCount ++;
	if ( sleepCount == 6 ) {
		setVisible( false );
		close();
		delete this;
		return false;
	}
	
	return true;
}


