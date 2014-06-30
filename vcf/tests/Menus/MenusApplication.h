//UndoAppApplication.h

#ifndef _MENUSAPPAPPLICATION_H__
#define _MENUSAPPAPPLICATION_H__



using namespace VCF;


/**
*Class MenusApplication documentation
*/
class MenusApplication : public VCF::Application { 
public:
	MenusApplication();

	virtual ~MenusApplication();

	virtual bool initRunningApplication();	

	static MenusApplication* getMenusApplication() {
		return (MenusApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_MENUSAPPAPPLICATION_H__


