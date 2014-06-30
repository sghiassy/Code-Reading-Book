//VCFBuilder2.cpp


#include "ApplicationKit.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "VCFBuilderApp.h"

using namespace VCF;
using namespace VCFBuilder;


int main(int argc, char *argv[])
{
	VCFBuilderApp app;

	Application::appMain( argc, argv );
	
	return 0;
}

