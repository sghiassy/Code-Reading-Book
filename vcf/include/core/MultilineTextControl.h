//MultilineTextControl.h

#ifndef _MULTILINETEXTCONTROL_H__
#define _MULTILINETEXTCONTROL_H__



using namespace VCF;


#define MULTILINETEXTCONTROL_CLASSID		"5e9c50e2-f9fc-46d8-9439-cf182d212df3"


namespace VCF  {

/**
*Class MultilineTextControl documentation
*/
class APPKIT_API MultilineTextControl : public TextControl { 
public:
	BEGIN_CLASSINFO(MultilineTextControl, "VCF::MultilineTextControl", "VCF::TextControl", MULTILINETEXTCONTROL_CLASSID)
	END_CLASSINFO(MultilineTextControl)

	MultilineTextControl();

	virtual ~MultilineTextControl();

protected:

private:
};


}; //end of namespace VCF

#endif //_MULTILINETEXTCONTROL_H__


