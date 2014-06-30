//SplashScreen.h

#ifndef _SPLASHSCREEN_H__
#define _SPLASHSCREEN_H__


#include "VCF.h"
#include "Window.h"
#include "Thread.h"


using namespace VCF;


/**
*Class SplashScreen documentation
*/
class SplashScreen : public VCF::Window, public VCF::Runnable  { 
public:
	SplashScreen();

	virtual ~SplashScreen();

	virtual bool run();

	virtual void paint( GraphicsContext* context );
protected:
	Image* m_splashImage;
private:

	VCF::ThreadLooper* m_thread;
};


#endif //_SPLASHSCREEN_H__


