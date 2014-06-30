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
$$IF(STD_FILE_MNU)
	void onFileOpenProject( VCF::MenuItemEvent* e );

	void onFileSaveProject( VCF::MenuItemEvent* e );

	void onFileExit( VCF::MenuItemEvent* e );
$$ENDIF
$$IF(UNDO_REDO)
	void onEditUndo( VCF::MenuItemEvent* e );

	void onEditUndoUpdate( VCF::MenuItemEvent* e );

	void onEditRedo( VCF::MenuItemEvent* e );

	void onEditRedoUpdate( VCF::MenuItemEvent* e );
$$ENDIF
$$IF(STD_HELP_MNU)
	void onHelpAbout( VCF::MenuItemEvent* e );
$$ENDIF
protected:

private:
};


#endif //_MAINWINDOW_H__


