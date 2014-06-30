//MainWindow.h

#ifndef _MAINWINDOW_H__
#define _MAINWINDOW_H__


#include "VCF.h"
#include "Window.h"


/**
*Class MainWindow documentation
*/
class MainWindow : public VCF::Window { 
public:
	MainWindow();

	virtual ~MainWindow();
	void onFileOpenProject( VCF::MenuItemEvent* e );

	void onFileSaveProject( VCF::MenuItemEvent* e );

	void onFileExit( VCF::MenuItemEvent* e );
	void onHelpAbout( VCF::MenuItemEvent* e );
protected:

private:
};


#endif //_MAINWINDOW_H__


