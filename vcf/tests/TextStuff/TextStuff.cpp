//TextStuff.cpp


#include "ApplicationKit.h"
#include "TextStuffApplication.h"
#include "UndoRedoStack.h"
#include "MainWindow.h"



using namespace VCF;



int main(int argc, char *argv[])
{
	TextStuffApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

