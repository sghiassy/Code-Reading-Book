//UIApplication.h

#ifndef _UIAPPLICATION_H__
#define _UIAPPLICATION_H__


#include "VCF.h"
#include "ClassInfo.h"


using namespace VCF;


#define UIAPPLICATION_CLASSID		"ec9764b9-cb25-4c3b-9ac7-eeea0cf3b26f"


namespace VCF  {

/**
*Class UIApplication documentation
*/
class UIApplication : public Application { 
public:
	BEGIN_CLASSINFO(UIApplication, "UIApplication", "Application", UIAPPLICATION_CLASSID)
	END_CLASSINFO(UIApplication)

	UIApplication();

	virtual ~UIApplication();

protected:

private:
};


}; //end of namespace VCF

#endif //_UIAPPLICATION_H__


