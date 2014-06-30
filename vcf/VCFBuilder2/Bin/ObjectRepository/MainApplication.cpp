//MainApplication.cpp

#include "ApplicationKit.h"
#include "MainApplication.h"


using namespace VCF;


MainApplication::MainApplication()
{

}

MainApplication::~MainApplication()
{

}

bool MainApplication::initRunningApplication()
{

}

void MainApplication::terminateRunningApplication()
{

}



int main( int argc, char** argv ) 
{
	MainApplication app;

	Application::appMain( argc, argv );

	return 0;
}