//TimerEvent.h

#ifndef _TIMEREVENT_H__
#define _TIMEREVENT_H__


#include "VCF.h"
#include "Event.h"


using namespace VCF;


namespace VCF  {

#define TIMER_EVENT_ACTIVATED				CUSTOM_EVENT_TYPES + 5000
#define TIMER_EVENT_DEACTIVATED				CUSTOM_EVENT_TYPES + 5001
#define TIMER_EVENT_PULSE					CUSTOM_EVENT_TYPES + 5002



/**
*Class TimerEvent documentation
*/
class TimerEvent : public VCF::Event { 
public:
	TimerEvent( Object* source, const unsigned long& eventType );

	virtual ~TimerEvent();

protected:

private:
};


}; //end of namespace VCF

#endif //_TIMEREVENT_H__


