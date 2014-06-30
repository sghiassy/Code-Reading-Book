//SplashTest.cpp


#include "ApplicationKit.h"
#include "SplashTestApplication.h"
#include "UndoRedoStack.h"
#include "MainWindow.h"



using namespace VCF;



int main(int argc, char *argv[])
{
	SplashTestApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

