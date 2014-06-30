//MainUIPanel.h
#include "ApplicationKit.h"
#include "MainUIPanel.h"
#include "Splitter.h"
#include "MenuBar.h"
#include "DefaultMenuItem.h"

#include "NewProjectDlg.h"

#include "Command.h"
#include "Project.h"
#include "NewProjectCmd.h"
#include "DefaultTreeItem.h"
#include "Form.h"
#include "CPPClassInstance.h"
#include "SelectionManager.h"
#include "ToolbarButton.h"
#include "ComponentInfo.h"

#include "TreeControl.h"
#include "TableControl.h"
#include "TabbedPages.h"
#include "Panel.h"
#include "ToolbarDock.h"
#include "Toolbar.h"	

#include "Label.h"
#include "TextControl.h"
#include "ListBoxControl.h"
#include "ObjectExplorer.h"
#include "VFFInputStream.h"
#include "ComponentInstance.h"

#include "VCFBuilderUI.h"

#include "CodeEditor.h"
#include "SingleSelectionGripper.h"
#include "FormView.h"
#include "DefaultUIProject.h"

#include "DefaultTreeModel.h"
#include "ProjectTreeModel.h"

#include "CodeGenerator.h"

using namespace VCF;

using namespace VCFBuilder;


MainUIPanel::MainUIPanel()
{
	m_activeForm = NULL;
	m_selectedPaletteButton = NULL;
	m_activeFormObj = NULL;
	m_projectObjectTree = NULL;
	m_objectExplorer = NULL;
	m_formViewer = NULL;	
	m_mainDock = NULL;
	m_mainToolbar = NULL;
	m_topLevel = NULL;
	m_componentPages = NULL;
	m_paletteImageList = NULL;
	m_standardImageList = NULL;
	m_selectedPaletteButton = NULL;
	m_codePages = NULL;

	setShowBorder( false );
}

MainUIPanel::~MainUIPanel()
{

}


void MainUIPanel::newProject( MenuItemEvent* e )
{		
	NewProjectDlg newProjectDlg;	

	if ( MRT_OK == newProjectDlg.showModal() ) {
		Project* project = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();
		if ( NULL != project ) {
			VCFBuilderUIApplication::getVCFBuilderUI()->closeProject( project );
		}


		Project* newProject = new DefaultUIProject();
		
		newProject->setName( newProjectDlg.getProjectName() );
		
		newProject->setPathName( newProjectDlg.getProjectPath() + "\\" + newProjectDlg.getProjectName() );
		
		//newProject->intializeSettings();

		NewProjectCmd* newProjectCmd = new NewProjectCmd( newProject, newProjectDlg.getSelectedTemplateProjectPath() );
		UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
		cmdStack->addCommand( newProjectCmd );
	}	
}

void MainUIPanel::openProject( MenuItemEvent* e )
{	
	Project* project = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();
	if ( NULL != project ) {
		VCFBuilderUIApplication::getVCFBuilderUI()->closeProject( project );
	}

	CommonFileOpen openDlg( this );	
	
	openDlg.addFilter( "VCF Builder Project Files", "*.vcp" );	
	if ( true == openDlg.execute() ) {			
		Project* project = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();
		if ( NULL != project ) {
			VCFBuilderUIApplication::getVCFBuilderUI()->closeProject( project );
		}

		VCFBuilderUIApplication::getVCFBuilderUI()->addExistingProject( openDlg.getFileName() );			
	}	
}

void MainUIPanel::saveProject( MenuItemEvent* e )
{		
	Project* project = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();
	if ( NULL != project ) {
		CommonFileSave saveDlg( this );
		
		saveDlg.setFileName( project->getName() + ".vcp" );
		saveDlg.addFilter( "VCF Builder Project Files", "*.vcp" );	
		if ( true == saveDlg.execute() ) {			
			
			VCFBuilderUIApplication::getVCFBuilderUI()->saveProject( saveDlg.getFileName(), project );			
		}		
	}
}

void MainUIPanel::saveProjectAs( MenuItemEvent* e )
{

}

void MainUIPanel::closeProject( MenuItemEvent* e )
{
	Project* project = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();

	if ( NULL != project ) {
		VCFBuilderUIApplication::getVCFBuilderUI()->closeProject( project );
	}
}

void MainUIPanel::newForm( MenuItemEvent* e )
{

}

void MainUIPanel::saveForm( MenuItemEvent* e )
{

}

void MainUIPanel::saveFormAs( MenuItemEvent* e )
{

}

void MainUIPanel::editUndo( MenuItemEvent* e )
{
	UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
	cmdStack->undo();
}

void MainUIPanel::editRedo( MenuItemEvent* e )
{
	UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
	cmdStack->redo();
}


void MainUIPanel::cut( MenuItemEvent* e )
{

}

void MainUIPanel::copy( MenuItemEvent* e )
{

}

void MainUIPanel::paste( MenuItemEvent* e )
{

}

void MainUIPanel::selectAll( MenuItemEvent* e )
{

}

void MainUIPanel::deleteSelection( MenuItemEvent* e )
{

}

void MainUIPanel::editPreferences( MenuItemEvent* e )
{

}

void MainUIPanel::editProjectSettings( MenuItemEvent* e )
{

}

void MainUIPanel::rebuildProjectTree( Project* project )
{
	TreeModel* tm = m_projectObjectTree->getTreeModel();
	tm->empty();

	if ( NULL != project ) {
		DefaultTreeItem* item = new DefaultTreeItem( project->getName(), m_projectObjectTree, tm );
		item->setImageIndex( 0 );
		tm->addNodeItem( item );
		DefaultTreeItem* forms = new DefaultTreeItem( "Forms", m_projectObjectTree, tm );
		forms->setImageIndex( 6 );
		tm->addNodeItem( forms, item );
		Enumerator<Form*>* formEnum = project->getForms();
		while ( formEnum->hasMoreElements() ) {
			Form* form = formEnum->nextElement();
			DefaultTreeItem* formItem = new DefaultTreeItem( "", m_projectObjectTree, tm );
			formItem->setCaption( form->getName() );
			formItem->setData( (void*)form );
			formItem->setImageIndex( 3 );
			tm->addNodeItem( formItem, forms );
			
			Enumerator<ComponentInstance*>* components = form->getComponents();
			if ( NULL != components ) {
				while ( true == components->hasMoreElements() ) {
					ComponentInstance* compInst = components->nextElement();
					DefaultTreeItem* componentItem = new DefaultTreeItem( "", m_projectObjectTree, tm );
					componentItem->setCaption( compInst->getName() );
					componentItem->setData( (void*)compInst );
					componentItem->setImageIndex( 4 );
					tm->addNodeItem( componentItem, formItem );
				}
			}
		}
		
		DefaultTreeItem* classes = new DefaultTreeItem( "Classes", m_projectObjectTree, tm );
		tm->addNodeItem( classes, item );
		classes->setImageIndex( 5 );
		Enumerator<CPPClass*>* classEnum = project->getClasses();
		while ( classEnum->hasMoreElements() ) {
			CPPClass* cppClass = classEnum->nextElement();
			DefaultTreeItem* classItem = new DefaultTreeItem( "", m_projectObjectTree, tm );
			classItem->setCaption( cppClass->getName() );
			classItem->setData( (void*)cppClass );
			classItem->setImageIndex( 1 );
			tm->addNodeItem( classItem, classes );
		}
		
		DefaultTreeItem* instances = new DefaultTreeItem( "Class Instances", m_projectObjectTree, tm );
		tm->addNodeItem( instances, item );
	}
}

void MainUIPanel::onProjectItemAdded( ProjectEvent* e )
{
	ProjectObject* projObject = e->getObjectThatChanged();
	switch ( e->getItemType() ) {
		case PIT_CLASS : {
			
		}
		break;
			
		case PIT_CLASS_INST : {
			
		}
		break;
			
		case PIT_FORM : {
				
		}
		break;
			
		case PIT_COMPONENT : {
			ProjectObject* parent = projObject->getParent();
			if ( NULL == parent ) {
				throw RuntimeException( "component project object has no parent!" );
			}	
			ProjectTreeModel* tm = dynamic_cast<ProjectTreeModel*>( m_projectObjectTree->getTreeModel() );
			if ( NULL == tm ) {
				throw RuntimeException( "Project tree model is not the right class type!" );	
			}
			
			TreeItem* parentItem = tm->find( (void*)parent );
			if ( NULL != parentItem ) {
				DefaultTreeItem* componentItem = new DefaultTreeItem( "", m_projectObjectTree, tm );
				componentItem->setCaption( projObject->getName() );
				componentItem->setData( (void*)projObject );
				componentItem->setImageIndex( 4 );
				tm->addNodeItem( componentItem, parentItem );
			}
		}
		break;
	}
}

void MainUIPanel::onProjectItemRemoved( ProjectEvent* e )
{
	ProjectObject* projObject = e->getObjectThatChanged();
	switch ( e->getItemType() ) {
		case PIT_CLASS : {
			
		}
		break;
			
		case PIT_CLASS_INST : {
			
		}
		break;
			
		case PIT_FORM : {
				
		}
		break;
			
		case PIT_COMPONENT : {
			ProjectObject* parent = projObject->getParent();
			if ( NULL == parent ) {
				throw RuntimeException( "component project object has no parent!" );
			}	
			ProjectTreeModel* tm = dynamic_cast<ProjectTreeModel*>( m_projectObjectTree->getTreeModel() );
			if ( NULL == tm ) {
				throw RuntimeException( "Project tree model is not the right class type!" );	
			}
			
			TreeItem* parentItem = tm->find( (void*)parent );
			TreeItem* objItem = tm->find( (void*)projObject );
			if ( (NULL != parentItem) && (NULL != objItem) ) {
				tm->deleteNodeItem( objItem );
			}
			else {
				throw RuntimeException( "Project tree model has invalid items!" );	
			}
		}
		break;
	}
}

void MainUIPanel::onProjectItemChanged( ProjectEvent* e )
{
	ProjectObject* projObject = e->getObjectThatChanged();
	Project* projectThatChanged =  (Project*)e->getSource();		
	
	ProjectTreeModel* tm = dynamic_cast<ProjectTreeModel*>( m_projectObjectTree->getTreeModel() );
	if ( NULL == tm ) {
		throw RuntimeException( "Project tree model is not the right class type!" );	
	}

	switch ( e->getItemType() ) {
		case PIT_CLASS : {
			
		}
		break;
			
		case PIT_CLASS_INST : {
			
		}
		break;
			
		case PIT_FORM : {
			TreeItem* objItem = tm->find( (void*)projObject );
			if ( NULL != objItem ) {
				objItem->setCaption( projObject->getName() );
			}
			else {
				throw RuntimeException( "Project tree model has invalid items!" );	
			}
			//find the forms class
			Form* form = (Form*)projObject;
			objItem = tm->find( (void*)form->getFormClass() );
			if ( NULL != objItem ) {
				objItem->setCaption( form->getFormClass()->getName() );
			}
			else {
				throw RuntimeException( "Project tree model has invalid items!" );	
			}
		}
		break;
			
		case PIT_COMPONENT : {
			ProjectObject* parent = projObject->getParent();
			if ( NULL == parent ) {
				throw RuntimeException( "component project object has no parent!" );
			}

			TreeItem* objItem = tm->find( (void*)projObject );
			if ( NULL != objItem ) {
				objItem->setCaption( projObject->getName() );
			}
			else {
				throw RuntimeException( "Project tree model has invalid items!" );	
			}
		}
		break;
	}
}

void MainUIPanel::onProjectChanged( ProjectEvent* e )
{
	switch ( e->getType() ) {
		case PROJECT_ITEM_ADDED : {			
			onProjectItemAdded( e );
		}
		break;

		case PROJECT_ITEM_REMOVED : {
			onProjectItemRemoved( e );
		}
		break;

		case PROJECT_ITEM_CHANGED : {
			onProjectItemChanged( e );
		}
		break;
	}
}

void MainUIPanel::onAppProjectChanged( VCFBuilderAppEvent* e ) 
{	
	//clear the active form
	setActiveForm( NULL );
	this->m_formViewer->setActiveProject( NULL );

	VCFBuilderUIApplication* app = (VCFBuilderUIApplication*)e->getSource();
	//get the active project
	Project* openProject = app->getCurrentOpenProject();
	if ( NULL != openProject ) {
		EventHandler* pl = getEventHandler( PROJECT_HANDLER );
		openProject->addProjectChangedHandler( pl );
		
		this->m_formViewer->setActiveProject( openProject );

		setActiveForm( openProject->getMainForm() );		
	}	
	rebuildProjectTree( openProject );	
}

void MainUIPanel::afterCreate( ComponentEvent* event )
{
	Panel* left = new Panel();
	left->setWidth( 350 );

	Splitter* leftSplitter = new Splitter();
	add( leftSplitter, ALIGN_LEFT );

	add( left, ALIGN_LEFT );
	

	m_formViewer = new FormView();
	//m_formViewer->setDoubleBuffered( false );
	add( m_formViewer, ALIGN_CLIENT );	

	m_codePages = new TabbedPages();
	add( m_codePages, ALIGN_CLIENT );
	m_codePages->setVisible( false );

	left->add( new Splitter(), ALIGN_TOP );	

	m_projectObjectTree = new TreeControl();
	m_projectObjectTree->setTreeModel( new ProjectTreeModel() );
	m_projectObjectTree->setVisible( true );
	m_projectObjectTree->setHeight( 300 );	
	left->add( m_projectObjectTree, ALIGN_TOP );
	
	ImageList* projectImageList = new ImageList();
	projectImageList->setImageHeight( 16 );
	projectImageList->setImageWidth( 16 );
	projectImageList->setTransparentColor( Color::getColor("green" ) );
	
	ResourceBundle* resBundle = VCFBuilderUIApplication::getVCFBuilderUI()->getResourceBundle();

	projectImageList->addImage( resBundle->getImage( "PROJECT" ) );
	projectImageList->addImage( resBundle->getImage( "class" ) );
	projectImageList->addImage( resBundle->getImage( "member" ) );
	projectImageList->addImage( resBundle->getImage( "Form" ) );
	projectImageList->addImage( resBundle->getImage( "FormMember" ) );
	projectImageList->addImage( resBundle->getImage( "classFolder" ) );
	projectImageList->addImage( resBundle->getImage( "formFolder" ) );
	m_projectObjectTree->setImageList( projectImageList );
	m_projectObjectTree->addComponent( projectImageList );

	MouseEventHandler<MainUIPanel>* mh = 
		new MouseEventHandler<MainUIPanel>( this, MainUIPanel::onProjectObjectTreeDblClicked, "projTreeMouseHandler" );
	
	m_projectObjectTree->addMouseDoubleClickedHandler( mh );
	


	VCFBuilderAppEventHandler<MainUIPanel>* appHandler = 
		new VCFBuilderAppEventHandler<MainUIPanel>(this,MainUIPanel::onAppProjectChanged,"appHandler" );
	
	VCFBuilderUIApplication::getVCFBuilderUI()->addProjectChangedHandler( appHandler );
	

	m_objectExplorer = new ObjectExplorer();
	left->add( m_objectExplorer, ALIGN_CLIENT );	

	m_topLevel = new Panel();
	m_topLevel->setHeight( 75 );
	add( m_topLevel, ALIGN_TOP );

	Panel* dockPanel = new Panel();
	dockPanel->setWidth( 200 );
	m_topLevel->add( new Splitter(), ALIGN_LEFT );

	m_topLevel->add( dockPanel, ALIGN_LEFT );	

	m_componentPages = new TabbedPages();
	
	Enumerator<String>* componentCategories = Component::getRegisteredComponentCategories();
	while ( componentCategories->hasMoreElements() ) {
		String category = componentCategories->nextElement();
		TabPage* newPage = m_componentPages->addNewPage( category );
		Toolbar* toolbar = new Toolbar();
		Panel* pagePanel = (Panel*)newPage->getPageComponent();
		toolbar->setHandleVisible( false );
		pagePanel->add( toolbar );
		m_paletteToolbars[category] = toolbar;
	}	

	m_topLevel->add( m_componentPages, ALIGN_CLIENT );
	

	m_mainDock = new ToolbarDock();
	m_mainDock->setHeight( 35 );
	dockPanel->add( m_mainDock, ALIGN_TOP );
	
	m_mainToolbar = new Toolbar();
	m_mainToolbar->setHeight( 32 );
	m_mainDock->addToolbar( m_mainToolbar );

	m_paletteImageList = new ImageList();
	this->addComponent( m_paletteImageList );	

	initPaletteImageList();

	m_standardImageList = new ImageList();
	this->addComponent( m_standardImageList );
	m_standardImageList->setImageHeight( 20 );
	m_standardImageList->setImageWidth( 20 );
	m_standardImageList->setTransparentColor( &Color( (uchar)255, (uchar)0, (uchar)255 ) );

	Image* i = VCFBuilderUIApplication::getVCFBuilderUI()->getResourceBundle()->getImage( "POINTER" );
	m_standardImageList->addImage( i );	
	delete i;
	

	ToolbarButton* pointerBtn = new ToolbarButton();
	m_mainToolbar->addButton( pointerBtn );
	pointerBtn->setImageList( m_standardImageList );
	pointerBtn->setBtnImageIndex( 0 );	
	pointerBtn->setShowCaption(false);
	pointerBtn->setAutoSizeCaption( false );
	pointerBtn->setCaption( "Selection" );
	pointerBtn->setStateToggle( false );

	ButtonEventHandler<MainUIPanel>* bh = 
		new ButtonEventHandler<MainUIPanel>( this, MainUIPanel::onPaletteButtonClicked, PALETTE_BUTTON_HANDLER );	
	

	bh = new ButtonEventHandler<MainUIPanel>( this, MainUIPanel::onSelectBtnClicked, "SelectBtnHandler" );
	
	pointerBtn->addButtonClickHandler ( bh );
	
	//PROJECT_HANDLER
	ProjectEventHandler<MainUIPanel>* ph = 
		new ProjectEventHandler<MainUIPanel>(this,MainUIPanel::onProjectChanged, PROJECT_HANDLER );
	

	initializeComponentPalette();
	
	setVisible( true );
}

void MainUIPanel::onPaletteButtonClicked( ButtonEvent* e )
{
	ToolbarButton* button = (ToolbarButton*)e->getSource();
	m_selectedPaletteButton = button;
	if ( NULL != button ){
		//button clicked - set the component class then set the tool mode
		m_formViewer->setComponentClass( (Class*)button->getUserData() );		
		m_formViewer->setToolMode( FTM_NEW_COMPONENT );		
	}	
}

void MainUIPanel::releaseSelectedPaletteToolbarBtn()
{
	if ( NULL != m_selectedPaletteButton ) 	{
		m_selectedPaletteButton->setIsPressed( false );		
	}
	m_selectedPaletteButton = NULL;
}

void MainUIPanel::setActiveForm( Form* activeForm )
{
	if ( NULL != m_activeForm ) {
		SelectionManager::getSelectionMgr()->removeFromSelectionSet( m_activeForm );		
		m_activeForm = NULL;		
	}
	
	m_activeFormObj = activeForm;

	if ( NULL != activeForm ) {
		try {
			//load the VFF from a file
			FileStream fs( activeForm->getVFFFileName(), FS_READ );
			VFFInputStream vis( &fs );
			//read in the Frame
			vis.readComponent( (Component**)&m_activeForm );
			
			activeForm->setRootComponent( m_activeForm );

			m_formViewer->setActiveForm( m_activeForm, activeForm );						

			if ( NULL == m_activeForm ) {
				throw RuntimeException( "Unable to load form \"" + activeForm->getName() + "\" from VFF File \"" + activeForm->getVFFFileName() + "\""  );
			}
		}
		catch ( BasicException& e ) {
			throw RuntimeException( "Error loading form \"" + activeForm->getName() + "\".\nException: " + e.getMessage() );
		}
	}
	else {
		m_formViewer->setActiveForm( NULL, NULL );
	}
}

void MainUIPanel::onProjectObjectTreeDblClicked( MouseEvent* e )
{
	TreeItem* foundItem = this->m_projectObjectTree->findItem( e->getPoint() );
	if ( NULL != foundItem ) {
		setActiveForm( NULL );
		ProjectObject* itemData  = (ProjectObject*)foundItem->getData();
		
		//this is a really hacky way of doing this. Need to come up with 
		//something better
		Form* formData = dynamic_cast<Form*>(itemData);
		if ( NULL != formData ) {
			m_formViewer->setVisible( true );
			resizeChildren();
			m_codePages->setVisible( false );
			setActiveForm( formData );
		}
		else {
			CPPClass* cppClass = dynamic_cast<CPPClass*>(itemData);
			if ( NULL != cppClass ) {
				m_formViewer->setVisible( false );
				m_codePages->setVisible( true );
				TabModel* model = m_codePages->getModel();
				Enumerator<TabPage*>* pages = model->getPages();
				bool addTabPage = true;
				while ( true == pages->hasMoreElements() ) {
					TabPage* page = pages->nextElement();
					String classDecl = cppClass->getName() + " - Decl";
					String classImpl = cppClass->getName() + " - Impl";
					if ( classDecl == page->getPageName() ) {						
						addTabPage = false;
						FileStream fs( cppClass->getDeclFileName(), FS_READ );
						String tmp;
						fs >> tmp;
						Panel* pagePanel = (Panel*)page->getPageComponent();
						CodeEditor* editor = (CodeEditor*)pagePanel->findControl( "SourceEditor" );						
						editor->getTextModel()->setText( tmp );
					}
					else if ( classImpl == page->getPageName() ) {
						addTabPage = false;
						FileStream fs( cppClass->getImplFileName(), FS_READ );
						String tmp;
						fs >> tmp;
						Panel* pagePanel = (Panel*)page->getPageComponent();
						CodeEditor* editor = (CodeEditor*)pagePanel->findControl( "SourceEditor" );						
						editor->getTextModel()->setText( tmp );
					}
				}
				if ( true == addTabPage ) {
					TabPage* page = m_codePages->addNewPage( cppClass->getName() + " - Decl" );
					Panel* pagePanel = (Panel*)page->getPageComponent();
					CodeEditor* editor = new CodeEditor();
					editor->setName( "SourceEditor" );
					pagePanel->add( editor, ALIGN_CLIENT );
					editor->setVisible( true );
					editor->setFocus( false );
					
					FileStream fs( cppClass->getDeclFileName(), FS_READ );
					String tmp;
					fs >> tmp;
					fs.close();
					editor->getTextModel()->setText( tmp );

					page = m_codePages->addNewPage( cppClass->getName() + " - Impl" );
					pagePanel = (Panel*)page->getPageComponent();
					editor = new CodeEditor();
					editor->setName( "SourceEditor" );
					pagePanel->add( editor, ALIGN_CLIENT );
					editor->setVisible( true );
					editor->setFocus( true );
					fs.open( cppClass->getImplFileName(), FS_READ );
					tmp = "";
					fs >> tmp;
					editor->getTextModel()->setText( tmp );
				}
				resizeChildren();
			}
		}
	}
}

void MainUIPanel::initializeComponentPalette()
{
	EventHandler* bh = getEventHandler( PALETTE_BUTTON_HANDLER );
	Enumerator<String>* componentCategories = Component::getRegisteredComponentCategories();
	bool pointerBtn = true;
	if ( NULL != componentCategories ){		
		
		while ( true == componentCategories->hasMoreElements() ){
			String category = componentCategories->nextElement();
			Toolbar* paletteToolbar = m_paletteToolbars[category];
			Enumerator<Class*>* componentClasses = Component::getComponentClassesForCategory( category );
			if ( NULL != componentClasses ){
				while ( true == componentClasses->hasMoreElements() ){
					Class* compClass = componentClasses->nextElement();
					if ( NULL != compClass ){
						String compClassName = compClass->getClassName();
						VCF::ComponentInfo* compInfo = UIToolkit::getDefaultUIToolkit()->getComponentInfo(compClass);
						
						ToolbarButton* button = new ToolbarButton();						
						paletteToolbar->addButton( button );
						button->setShowCaption( false );
						button->setAutoSizeCaption( false );
						button->setCaption( compClassName );
						button->setToolTip( compClassName + " component" );
						button->setUserData( (void*)compClass );
						button->setBtnImageIndex( 0 );
						button->setWidth( button->getPreferredWidth()*1.5 );
						if ( NULL != compInfo ) {
							int compImageIndex = m_paletteImageListIndexMap[compClass->getID()];							
							button->setBtnImageIndex( compImageIndex );
						}
						button->setImageList( this->m_paletteImageList );

						if ( NULL != bh ) {
							button->addButtonClickHandler( bh );
						}						
					}
				}
			}
		}
	}
}

void MainUIPanel::onSelectBtnClicked( ButtonEvent* e )
{

}

void MainUIPanel::initPaletteImageList()
{
	m_paletteImageList->setImageHeight( 25 );
	m_paletteImageList->setImageWidth( 25 );	
	m_paletteImageList->setTransparentColor( &Color( (uchar)255, (uchar)0, (uchar)255 ) );
	Image* paletteImage = VCFBuilderUIApplication::getVCFBuilderUI()->getResourceBundle()->getImage( "DEFAULTCOMPONENTIMAGE" );
	this->m_paletteImageList->addImage( paletteImage );
	delete paletteImage;
	VCF::ComponentInfo* compInfo = NULL;
	
	ResourceBundle* rezBundle = VCFBuilderUIApplication::getVCFBuilderUI()->getResourceBundle();

	paletteImage = rezBundle->getImage( "commandButton" );	
	compInfo = new VCF::ComponentInfo( COMMANDBUTTON_CLASSID, "", "", "", "", paletteImage );

	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( COMMANDBUTTON_CLASSID, compInfo );
	compInfo->addHeader( "CommandButton.h" );
	compInfo->addNamespace( "VCF" );
	this->m_paletteImageList->addImage( paletteImage );
	m_paletteImageListIndexMap[COMMANDBUTTON_CLASSID] = 1;	

	paletteImage = rezBundle->getImage( "label" );	
	compInfo = new VCF::ComponentInfo( LABEL_CLASSID, "", "", "", "", paletteImage );
	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( LABEL_CLASSID, compInfo );
	compInfo->addHeader( "Label.h" );
	compInfo->addNamespace( "VCF" );
	this->m_paletteImageList->addImage( paletteImage );
	m_paletteImageListIndexMap[LABEL_CLASSID] = 2;
	
	
	paletteImage = rezBundle->getImage( "menu" );	
	compInfo = new VCF::ComponentInfo( MENUBAR_CLASSID, "", "", "", "", paletteImage );
	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( MENUBAR_CLASSID, compInfo );
	compInfo->addHeader( "MenuBar.h" );
	compInfo->addNamespace( "VCF" );
	this->m_paletteImageList->addImage( paletteImage );
	m_paletteImageListIndexMap[MENUBAR_CLASSID] = 3;
	

	paletteImage = rezBundle->getImage( "listBox" );
	compInfo = new VCF::ComponentInfo( LISTBOXCONTROL_CLASSID, "", "", "", "", paletteImage );
	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( LISTBOXCONTROL_CLASSID, compInfo );
	compInfo->addHeader( "ListBoxControl.h" );
	compInfo->addNamespace( "VCF" );
	this->m_paletteImageList->addImage( paletteImage );
	m_paletteImageListIndexMap[LISTBOXCONTROL_CLASSID] = 4;
	

	paletteImage = rezBundle->getImage( "panel" );	
	compInfo = new VCF::ComponentInfo( PANEL_CLASSID, "", "", "", "", paletteImage );
	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( PANEL_CLASSID, compInfo );
	this->m_paletteImageList->addImage( paletteImage );
	compInfo->addHeader( "Panel.h" );
	compInfo->addNamespace( "VCF" );
	m_paletteImageListIndexMap[PANEL_CLASSID] = 5;
	

	paletteImage = rezBundle->getImage( "popup" );	
	compInfo = new VCF::ComponentInfo( POPUPMENU_CLASSID, "", "", "", "", paletteImage );
	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( POPUPMENU_CLASSID, compInfo );
	this->m_paletteImageList->addImage( paletteImage );
	compInfo->addHeader( "PopupMenu.h" );
	compInfo->addNamespace( "VCF" );
	m_paletteImageListIndexMap[POPUPMENU_CLASSID] = 6;
	

	paletteImage = rezBundle->getImage( "textControl" );	
	compInfo = new VCF::ComponentInfo( TEXTCONTROL_CLASSID, "", "", "", "", paletteImage );
	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( TEXTCONTROL_CLASSID, compInfo );
	this->m_paletteImageList->addImage( paletteImage );
	compInfo->addHeader( "TextControl.h" );
	compInfo->addNamespace( "VCF" );
	m_paletteImageListIndexMap[TEXTCONTROL_CLASSID] = 7;	

	paletteImage = rezBundle->getImage( "richTextControl" );	
	compInfo = new VCF::ComponentInfo( MULTILINETEXTCONTROL_CLASSID, "", "", "", "", paletteImage );
	UIToolkit::getDefaultUIToolkit()->registerComponentInfo( MULTILINETEXTCONTROL_CLASSID, compInfo );
	this->m_paletteImageList->addImage( paletteImage );
	compInfo->addHeader( "MultilineTextControl.h" );
	compInfo->addNamespace( "VCF" );
	m_paletteImageListIndexMap[MULTILINETEXTCONTROL_CLASSID] = 8;	
	
}