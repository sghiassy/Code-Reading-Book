//MainWindow.h

#ifndef _MAINWINDOW_H__
#define _MAINWINDOW_H__



/**
*Class MainWindow documentation
*/
class MainWindow : public VCF::Window { 
public:
	MainWindow();

	virtual ~MainWindow();
	void onFileOpen( VCF::MenuItemEvent* e );

	void onFileSave( VCF::MenuItemEvent* e );

	void onFileExit( VCF::MenuItemEvent* e );
	void onHelpAbout( VCF::MenuItemEvent* e );
protected:

private:
};


#endif //_MAINWINDOW_H__


