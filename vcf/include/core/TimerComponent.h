//TimerComponent.h

#ifndef _TIMERCOMPONENT_H__
#define _TIMERCOMPONENT_H__


#include "VCF.h"
#include "Component.h"
#include "Runnable.h"
#include "TimerListener.h"
#include "TimerEvent.h"	

using namespace VCF;


#define TIMERCOMPONENT_CLASSID		"69f9637c-6273-4e10-a9d1-fad815095b61"


namespace VCF  {

class ThreadLooper;

/**
*Class TimerComponent documentation
*/
class TimerComponent : public Component, public Runnable { 
public:
	BEGIN_CLASSINFO(TimerComponent, "VCF::TimerComponent", "VCF::Component", TIMERCOMPONENT_CLASSID)
	PROPERTY( bool, "active", TimerComponent::isActive, TimerComponent::setActivated, PROP_BOOL );
	PROPERTY( long, "timeoutInterval", TimerComponent::getTimeoutInterval, TimerComponent::setTimeoutInterval, PROP_LONG );
	END_CLASSINFO(TimerComponent)

	TimerComponent();

	virtual ~TimerComponent();

	virtual void afterCreate( ComponentEvent* event );

	bool isActive() {
		return m_isActive;
	}

	void setActivated( const bool& isActive );

	virtual bool run();

	long getTimeoutInterval() {
		return m_timeoutInterval;
	}

	void setTimeoutInterval( const long& interval );


	/**
	*This macro declares a std::vector<> of TimerListener listeners
	*/
	LISTENER_LIST(TimerListener);

	/**
	*This macro creates a method for adding a listener to the TimerComponent's events
	*/
	ADD_LISTENER(TimerListener);

	/**
	*This macro creates a method for removing a listener to the TimerComponent's events
	*/
	REMOVE_LISTENER(TimerListener);

	NOTIFIER(TimerListener, onTimer, TimerEvent);
protected:
	bool m_isActive;
	long m_timeoutInterval;
	ThreadLooper* m_timerThread;
private:
};


}; //end of namespace VCF

#endif //_TIMERCOMPONENT_H__


