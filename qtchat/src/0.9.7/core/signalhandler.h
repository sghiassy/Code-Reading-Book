#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <signal.h>

class SignalHandler;

typedef void (SignalHandler::*sig_t)(int);

class SignalHandler {
	public:
		SignalHandler() :
			signal_handler_ptr(&signal_handler) {}
		operator sig_t() const {
			return signal_handler_ptr;
		}

	private:
		virtual void signal_handler(int) const {}	// override to create new signal handler
		sig_t signal_handler_ptr;
};

#endif	// _SIGNAL_H
