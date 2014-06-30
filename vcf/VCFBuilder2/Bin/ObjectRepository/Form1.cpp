//Form1.cpp

#include "ApplicationKit.h"
#include "Form1.h"


using namespace VCF;

Form1::Form1()
{

	Application* app = Application::getRunningInstance();
	if ( NULL != app ){
		Frame* frame = this;
		app->loadFrame( &frame );
	}

	vcfInit();
}

Form1::~Form1()
{

}

void Form1::vcfInit()
{
	
}//vcfInit finished 