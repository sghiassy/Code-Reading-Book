//TimerComponent.h
#include "ApplicationKit.h"
#include "TimerComponent.h"
#include "Thread.h"


using namespace VCF;



TimerComponent::TimerComponent()
{
	m_timerThread = NULL;
	m_timeoutInterval = 1000;
	m_isActive = false;
}

TimerComponent::~TimerComponent()
{
	if ( true == m_isActive ) {
		delete m_timerThread;
		m_timerThread = NULL;
	}
}

void TimerComponent::afterCreate( ComponentEvent* event )
{

}

void TimerComponent::setActivated( const bool& isActive )
{
	m_isActive = isActive;

	if ( true == m_isActive ) {
		m_timerThread = new ThreadLooper( this, false );
		
		TimerEvent te(this, TIMER_EVENT_ACTIVATED );
		notify_onTimer( &te );
		
		m_timerThread->start();
	}
	else {
		delete m_timerThread;
		m_timerThread = NULL;

		TimerEvent te(this, TIMER_EVENT_DEACTIVATED );
		notify_onTimer( &te );
	}

}

bool TimerComponent::run()
{
	TimerEvent te(this, TIMER_EVENT_PULSE );
	notify_onTimer( &te );

	m_timerThread->sleep( m_timeoutInterval );
	return true;
}

void TimerComponent::setTimeoutInterval( const long& interval )
{
	m_timeoutInterval = interval;
}