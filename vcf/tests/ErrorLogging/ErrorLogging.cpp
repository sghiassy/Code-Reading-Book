//ErrorLogging.cpp


#include "ApplicationKit.h"
#include "ErrorLoggingApplication.h"
#include "UndoRedoStack.h"
#include "MainWindow.h"



using namespace VCF;



int main(int argc, char *argv[])
{
	ErrorLoggingApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

