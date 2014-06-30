//UndoRedo.cpp


#include "ApplicationKit.h"
#include "UndoRedoApplication.h"
#include "UndoRedoStack.h"
#include "MainWindow.h"



using namespace VCF;



int main(int argc, char *argv[])
{
	UndoRedoApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

