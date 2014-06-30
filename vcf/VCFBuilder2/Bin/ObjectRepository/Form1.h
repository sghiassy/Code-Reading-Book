//Form1.h

#ifndef _FORM1_H__
#define _FORM1_H__


using namespace VCF;


#define FORM1_CLASSID		"4B32A3BC-BB2D-4EAC-9E15-B66F5C5B9AA8"

class Form1 : public VCF::Window{
public:
	BEGIN_CLASSINFO(Form1, "Form1", "VCF::Window", FORM1_CLASSID )
	END_CLASSINFO(Form1)

	Form1();

	virtual ~Form1();

	/**
	*vcfInit() is used by VCF Builder to initialize variables
	*and linkup listener objects.
	*/
	void vcfInit();

protected:

};


#endif // _FORM1_H 