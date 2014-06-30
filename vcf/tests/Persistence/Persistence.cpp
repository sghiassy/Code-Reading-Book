//Persistence.cpp


#include "ApplicationKit.h"
#include "PersistenceApplication.h"
#include "UndoRedoStack.h"
#include "MainWindow.h"



using namespace VCF;



int main(int argc, char *argv[])
{
	PersistenceApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

