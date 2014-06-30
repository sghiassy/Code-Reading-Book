//FormView.h

#ifndef _FORMVIEW_H__
#define _FORMVIEW_H__



#include "Panel.h"
#include "ListViewControl.h"

#include "TextControl.h"
#include "MultilineTextControl.h"

using namespace VCF;


#define FORMVIEW_CLASSID					"4db7de12-cbc0-43c0-8d45-9e686991531f"
#define FORMVIEW_WINDOW_HANDLER				"FormViewWindowHandler"

#define SELECTED_CONTROL_MOUSEDOWNHANDLER		"SelectedControlMouseDownHandler"
#define SELECTED_CONTROL_MOUSEUPHANDLER			"SelectedControlMouseUpHandler"
#define SELECTED_CONTROL_MOUSEMOVEHANDLER		"SelectedControlMoveMouseHandler"


#define SELECTED_CONTROL_KEYPRESSEDHANDLER		"SelectedControlKeyPressedHandler"
#define SELECTED_CONTROL_POSITIONEDHANDLER		"SelectedControlPositionedHandler"
#define FORMVIEW_PROJECT_HANDLER				"ActiveProjectHandler"

namespace VCFBuilder  {

class SelectionManagerEvent;

class FormGrid;

class GridRuler;

class GridRulerBtn;

class Form;

class Project;

class ProjectEvent;

enum FormToolMode {
	FTM_SELECT = 0,
	FTM_NEW_COMPONENT
};

enum SelectionToolMode { 
	STM_NONE = 0,
	STM_DRAW_SELECTION_RECT,
	STM_TRANSLATE_CONTROL
};

/**
*Class FormView documentation
*/
class FormView : public VCF::Panel { 
public:
	BEGIN_CLASSINFO(FormView, "VCFBuilder::FormView", "VCF::Panel", FORMVIEW_CLASSID)
	END_CLASSINFO(FormView)

	FormView();

	virtual ~FormView();

	virtual void paint( GraphicsContext* ctx );

	virtual void mouseDown( MouseEvent* e );	

	void onActiveFormClose( WindowEvent* event );

	void onControlMouseDown( MouseEvent* e );

	void onControlMouseMove( MouseEvent* e );

	void onControlMouseUp( MouseEvent* e );

	void onSelectionManagerChanged( SelectionManagerEvent* e );

	void setComponentClass( Class* componentClass ) {
		m_componentClass = componentClass;
	}

	Class* getComponentClass() {
		return m_componentClass;
	}

	void setActiveForm( Frame* activeForm, Form* activeFormInst );

	void setToolMode( const FormToolMode& toolMode ) {
		m_toolMode = toolMode;
	}

	FormToolMode getToolMode() {
		return m_toolMode;
	}

	void setActiveProject( Project* activeProject );
protected:
	Point m_startDragPt;
	Rect m_selectionRect;
	bool m_canBeginDraggingSelectionRect;
	Frame* m_activeForm;
	ListViewControl* m_formComponentsView;
	Form* m_activeFormInst;
	Class* m_componentClass;

	void addNewComponent( Class* componentClass );

	void addComponentToForm( Component* componentToAdd ); 

	void removeComponentFromForm( Component* componentToRemove ); 

	void updateComponentListForSelectedComponents( Enumerator<Component*>* componentSelectionSet );
	
	void doSelectionGrippers();

	SingleSelectionGripper m_singleSelectionGripper;

	FormGrid* m_formGrid;
	
	MultilineTextControl* m_formAsTextView;

	GridRulerBtn* m_gridRulerBtn;
	GridRuler* m_verticalRuler;
	GridRuler* m_horizontalRuler;
	
	void dragSelectionRect( Control* control, Point* pt );

	void translateSelectionRect( Control* control, Point* pt );

	FormToolMode m_toolMode;
	SelectionToolMode m_selectionToolMode;

	void onControlKeyPressed( KeyboardEvent* e );

	void onControlPositioned( ControlEvent* e );

	void onControlSized( ControlEvent* e );

	void onShowFormAsVFF( MenuItemEvent* e );
	void onUpdateShowFormAsVFF( MenuItemEvent* e );

	void reloadControlHandlers( Control* control );

	void onProjectChanged( ProjectEvent* e );

	void onComponentsViewClick( MouseEvent* event );

	void onComponentEditorCommandClicked( MenuItemEvent* e );

private:
};


}; //end of namespace VCFBuilder

#endif //_FORMVIEW_H__


