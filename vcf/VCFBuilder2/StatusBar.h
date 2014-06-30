//StatusBar.h

#ifndef _STATUSBAR_H__
#define _STATUSBAR_H__

#include "CustomControl.h"


#define STATUSBAR_CLASSID		"3e78a960-2110-4a5d-8731-2fae741c4504"


namespace VCF  {

/**
*Class StatusBar documentation
*/
class StatusBar : public CustomControl { 
public:
	BEGIN_CLASSINFO(StatusBar, "VCF::StatusBar", "VCF::CustomControl", STATUSBAR_CLASSID)
	END_CLASSINFO(StatusBar)

	StatusBar();

	virtual ~StatusBar();

	void setStatusCaption( const String& caption ) {
		m_statusCaption = caption;
		repaint();
	}

	virtual void paint( GraphicsContext* ctx );
protected:
	String m_statusCaption;
private:
};


}; //end of namespace VCF

#endif //_STATUSBAR_H__


