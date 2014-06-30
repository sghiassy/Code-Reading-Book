//WebForm.h

#ifndef _WEBFORM_H__
#define _WEBFORM_H__


using namespace VCF;


#define WEBFORM_CLASSID		"62cfe8b7-6964-4b89-9b7c-8bcec80ce4f1"


namespace VCFBuilder  {

class Project;


/**
*Class WebForm documentation
*/
class WebForm : public Form { 
public:
	WebForm( ProjectObject* parent, Project* ownerProject );

	virtual ~WebForm();

protected:

private:
};


}; //end of namespace VCFBuilder

#endif //_WEBFORM_H__


