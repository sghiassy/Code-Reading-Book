//MainWindow.h

#ifndef _MAINWINDOW_H__
#define _MAINWINDOW_H__

#include "VCFBuilderUIConfig.h"
#include "Window.h"
#include "StatusBar.h"

#define MAINWINDOW_CLASSID		"be07f0a1-35c1-41a3-bdc5-9f97db8edb93"
#define	PALETTE_BUTTON_HANDLER	"PaletteButtonHandler"
#define PROJECT_HANDLER			"CurrentProjectHandler"


class VCF::MenuItemEvent;

using namespace VCF;

namespace VCFBuilder  {

class Form;
class VCFBuilderAppEvent;
class ProjectEvent;
class Project;
class ObjectExplorer;
class MainUIPanel;
class FormView;


/**
*Class MainWindow documentation
*/
class VCFBUILDER_API MainWindow : public VCF::Window { 
public:
	BEGIN_CLASSINFO(MainWindow, "VCFBuilder::MainWindow", "VCF::Window", MAINWINDOW_CLASSID)
	END_CLASSINFO(MainWindow)

	MainWindow();

	virtual ~MainWindow();


	void onFileNewProject( VCF::MenuItemEvent* e );

	void onFileOpenProject( VCF::MenuItemEvent* e );

	void onFileSaveProject( VCF::MenuItemEvent* e );

	void onFileSaveProjectAs( VCF::MenuItemEvent* e );

	void onFileCloseProject( VCF::MenuItemEvent* e );

	void onFileNewForm( VCF::MenuItemEvent* e );

	void onFileSaveForm( VCF::MenuItemEvent* e );

	void onFileSaveFormAs( VCF::MenuItemEvent* e );

	void onFileExit( VCF::MenuItemEvent* e );

	
	void onEditUndo( VCF::MenuItemEvent* e );

	void onEditUndoUpdate( VCF::MenuItemEvent* e );

	void onEditRedo( VCF::MenuItemEvent* e );

	void onEditRedoUpdate( VCF::MenuItemEvent* e );

	void onEditCut( VCF::MenuItemEvent* e );

	void onEditCopy( VCF::MenuItemEvent* e );

	void onEditPaste( VCF::MenuItemEvent* e );

	void onEditSelectAll( VCF::MenuItemEvent* e );

	void onEditDelete( VCF::MenuItemEvent* e );

	void onEditPreferences( VCF::MenuItemEvent* e );

	void onEditProjectSettings( VCF::MenuItemEvent* e );

	void onMenuItemSelectionChanged( ItemEvent* e );

	virtual void afterCreate( ComponentEvent* event );

	

	/**
	sets the current active form object. This causes a new 
	Frame to be displayed within the FormView's form grid.
	The new Frame is loaded from the VFF file name specified
	in the Form's getVFFFileName() method.
	*/
	void setActiveForm( Form* activeForm );

	Form* getActiveForm();

	Frame* getActiveFrameControl();

	FormView* getFormViewer();

	ObjectExplorer* getObjectExplorer();

protected:
	MainUIPanel* m_mainUI;

	StatusBar* m_statusBar;	
private:
};


}; //end of namespace VCFBuilder

#endif //_MAINWINDOW_H__


