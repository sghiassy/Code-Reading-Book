//TimerListener.h

#ifndef _TIMERLISTENER_H__
#define _TIMERLISTENER_H__


#include "VCF.h"
#include "Listener.h"


using namespace VCF;


namespace VCF  {

class TimerEvent;


/**
*Class TimerListener documentation
*/
class TimerListener : public Listener { 
public:
	virtual ~TimerListener(){};

	virtual void onTimer( TimerEvent* event ) = 0;
};


typedef void (Object::*OnTimerEvent)( TimerEvent* event );


class TimerHandler : public Object, public TimerListener {
public:
	TimerHandler( Object* source ) {
		m_source = source;
	}

	virtual ~TimerHandler(){};

	virtual void onTimer( TimerEvent* event ) {
		if ( (NULL != m_source) && (NULL != m_timer) )  {
			(m_source->*m_timer)( event );
		}
	}
	
	OnTimerEvent m_timer;
protected:
	Object* m_source;
};



}; //end of namespace VCF

#endif //_TIMERLISTENER_H__


