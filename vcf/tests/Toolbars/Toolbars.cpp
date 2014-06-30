//Toolbars.cpp


#include "ApplicationKit.h"
#include "ToolbarsApplication.h"
#include "UndoRedoStack.h"
#include "MainWindow.h"



using namespace VCF;



int main(int argc, char *argv[])
{
	ToolbarsApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

