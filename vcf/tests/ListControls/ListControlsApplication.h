//UndoAppApplication.h

#ifndef _LISTCONTROLSAPPAPPLICATION_H__
#define _LISTCONTROLSAPPAPPLICATION_H__



using namespace VCF;


/**
*Class ListControlsApplication documentation
*/
class ListControlsApplication : public VCF::Application { 
public:
	ListControlsApplication();

	virtual ~ListControlsApplication();

	virtual bool initRunningApplication();	
	

	static ListControlsApplication* getListControlsApplication() {
		return (ListControlsApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_LISTCONTROLSAPPAPPLICATION_H__


