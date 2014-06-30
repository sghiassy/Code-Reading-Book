//MainApplication.h

#ifndef _MAINAPPLICATION_H__
#define _MAINAPPLICATION_H__

class MainApplication : public Application {
public:
	MainApplication();

	virtual ~MainApplication();

	virtual bool initRunningApplication();

	virtual void terminateRunningApplication();
};

#endif //_MAINAPPLICATION_H__