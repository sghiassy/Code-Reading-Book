//MainUIPanel.h

#ifndef _MAINUIPANEL_H__
#define _MAINUIPANEL_H__

#include "VCFBuilderUIConfig.h"
#include "Panel.h"

namespace VCF {
	
	class TreeControl;	
	class TabbedPages;
	class ToolbarDock;
	class Toolbar;
	class ToolbarButton;
	
};

using namespace VCF;

namespace VCFBuilder  {

class FormView;

#define MAINUIPANEL_CLASSID		"c9e23e4f-61e4-4ac9-a31d-5e17b2f2b0e4"

#define	PALETTE_BUTTON_HANDLER	"PaletteButtonHandler"
#define PROJECT_HANDLER			"CurrentProjectHandler"

class Form;
class VCFBuilderAppEvent;
class ProjectEvent;
class Project;
class ObjectExplorer;

/**
*Class MainUIPanel documentation
*/
class VCFBUILDER_API MainUIPanel : public VCF::Panel { 
public:
	BEGIN_CLASSINFO(MainUIPanel, "MainUIPanel", "VCF::Panel", MAINUIPANEL_CLASSID)
	END_CLASSINFO(MainUIPanel)

	MainUIPanel();

	virtual ~MainUIPanel();

	void newProject( VCF::MenuItemEvent* e );

	void openProject( VCF::MenuItemEvent* e );

	void saveProject( VCF::MenuItemEvent* e );

	void saveProjectAs( VCF::MenuItemEvent* e );

	void closeProject( VCF::MenuItemEvent* e );

	void newForm( VCF::MenuItemEvent* e );

	void saveForm( VCF::MenuItemEvent* e );

	void saveFormAs( VCF::MenuItemEvent* e );

	void editUndo( VCF::MenuItemEvent* e );	

	void editRedo( VCF::MenuItemEvent* e );

	void cut( VCF::MenuItemEvent* e );

	void copy( VCF::MenuItemEvent* e );

	void paste( VCF::MenuItemEvent* e );

	void selectAll( VCF::MenuItemEvent* e );

	void deleteSelection( VCF::MenuItemEvent* e );

	void editPreferences( VCF::MenuItemEvent* e );

	void editProjectSettings( VCF::MenuItemEvent* e );	

	virtual void afterCreate( ComponentEvent* event );

	void onAppProjectChanged( VCFBuilderAppEvent* e );

	/**
	sets the current active form object. This causes a new 
	Frame to be displayed within the FormView's form grid.
	The new Frame is loaded from the VFF file name specified
	in the Form's getVFFFileName() method.
	*/
	void setActiveForm( Form* activeForm );

	Form* getActiveForm() {
		return m_activeFormObj;
	}

	void onProjectObjectTreeDblClicked( MouseEvent* e );

	void onPaletteButtonClicked( ButtonEvent* e );

	void onSelectBtnClicked( ButtonEvent* e );

	void releaseSelectedPaletteToolbarBtn();

	Frame* getActiveFrameControl() {
		return m_activeForm;
	}

	FormView* getFormViewer() {
		return m_formViewer;
	}

	ObjectExplorer* getObjectExplorer() {
		return m_objectExplorer;
	}
protected:
	VCF::TreeControl* m_projectObjectTree;

	ObjectExplorer* m_objectExplorer;	
	
	FormView* m_formViewer;	

	ToolbarDock* m_mainDock;
	Toolbar* m_mainToolbar;
	Panel* m_topLevel;

	TabbedPages* m_componentPages;

	Frame* m_activeForm;
	Form* m_activeFormObj;

	ImageList* m_paletteImageList;
	ImageList* m_standardImageList;
	std::map<String,Toolbar*> m_paletteToolbars;
	void initializeComponentPalette();	
	
	void initPaletteImageList();

	std::map<String,long> m_paletteImageListIndexMap;

	ToolbarButton* m_selectedPaletteButton;

	void onProjectChanged( ProjectEvent* e );

	void rebuildProjectTree( Project* project );
	
	TabbedPages* m_codePages;

	void onProjectItemAdded( ProjectEvent* e );

	void onProjectItemRemoved( ProjectEvent* e );

	void onProjectItemChanged( ProjectEvent* e );
protected:

private:
};


}; //end of namespace VCFBuilder

#endif //_MAINUIPANEL_H__


