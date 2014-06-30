//NewProjectDlg.h
#include "ApplicationKit.h"
#include "NewProjectDlg.h"
#include "Label.h"
#include "TextControl.h"
#include "VCFBuilderUI.h"
#include "TreeControl.h"
#include "ListViewControl.h"
#include "Label.h"
#include "Light3DBorder.h"
#include "DefaultTreeItem.h"
#include "XMLParser.h"
#include "DefaultListItem.h"

using namespace VCF;

using namespace VCFBuilder;


NewProjectDlg::NewProjectDlg()
{
	m_projectName = "Project1";
	m_projectPath = VCFBuilderUIApplication::getVCFBuilderUI()->getDefaultProjectPath();

	setHeight( 355 );
	setWidth( 550 );
	setCaption( "New VCF Builder Project" );

	double midx = getWidth() / 2.0;
	double qtrx = getWidth() / 4.0;
	double left = 10;
	double right = getWidth() - 10;


	Label* projectsLabel = new Label();
	projectsLabel->setBounds( &Rect( left, 5, midx, 28 ) );
	projectsLabel->setCaption( "Project Types" );
	add( projectsLabel );
	projectsLabel->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );

	m_projectTypesTree = new TreeControl();
	m_projectTypesTree->setBounds( &Rect( left, 30, midx, 155 ) );
	add( m_projectTypesTree );
	m_projectTypesTree->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );
	m_projectTypesTree->setVisible( true );

	m_projectTypesTree->addMouseClickedHandler( 
							new MouseEventHandler<NewProjectDlg>( this,
																	onProjectsTypesTreeClicked,
																	"ProjectsTypesTreeClickedHandler" ) );


	Label* projectTemplatesLabel = new Label();
	projectTemplatesLabel->setCaption( "Templates" );
	projectTemplatesLabel->setBounds( &Rect( midx+ 5, 5, right, 28 ) );
	add( projectTemplatesLabel );
	projectTemplatesLabel->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );

	m_projectTemplates = new ListViewControl();
	m_projectTemplates->setBounds( &Rect( midx+ 5, 30, right, 155 ) );
	add( m_projectTemplates );
	m_projectTemplates->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );
	m_projectTemplates->setVisible( true );

	m_projectTemplates->addItemSelectionChangedHandler( 
							new ItemEventHandler<NewProjectDlg>( this, 
																	onProjectTemplateItemSelected,
																	"ProjectTemplateItemSelectedHandler" ) );

	m_projectDesciption = new Label();
	m_projectDesciption->setBounds( &Rect( left, 160, right, 185 ) );
	add( m_projectDesciption );
	m_projectDesciption->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );
	Light3DBorder* border = new Light3DBorder();
	border->setInverted( true );
	m_projectDesciption->setBorder( border );	
	m_projectDesciption->setCaption( "Select a project template to create a new project" );

	Label* label1 = new Label();
	label1->setCaption( "Name:" );
	double top = 190;
	label1->setBounds( &Rect(left, top, qtrx, top + label1->getPreferredHeight() ) );
	label1->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );
	add( label1 );

	TextControl* projName = new TextControl();	
	projName->getTextModel()->setText( m_projectName );
	projName->setBounds( &Rect(qtrx + 8, top, right, top + label1->getPreferredHeight() ) );
	projName->setVisible( true );
	projName->setToolTip( "Project Name" );
	add( projName );


	top += label1->getPreferredHeight() + 8;

	Label* label2 = new Label();
	label2->setCaption( "Directory Path:" );
	label2->setBounds( &Rect(left, top, qtrx, top + label2->getPreferredHeight() ) );
	label2->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );
	add( label2 );
	
	TextControl* projPath = new TextControl();	
	projPath->getTextModel()->setText( m_projectPath );
	projPath->setBounds( &Rect(qtrx + 8, top, right - 30, top + label2->getPreferredHeight() ) );
	projPath->setVisible( true );
	projPath->setToolTip( "Project Directory Path" );
	add( projPath );

	CommandButton* browseBtn = new CommandButton();
	browseBtn->setBounds( &Rect(right - 25, top, right, top + label2->getPreferredHeight() ) );
	browseBtn->setCaption( "..." );
	browseBtn->setToolTip( "Browse for Project Directory Path" );
	add( browseBtn );

	top += label2->getPreferredHeight() + 8;

	TextModelEventHandler<NewProjectDlg>* tmHandler = 
		new TextModelEventHandler<NewProjectDlg>( this,NewProjectDlg::onProjectPathTextChanged,"projPathHandler" );	
	projPath->getTextModel()->addTextModelChangedHandler( tmHandler );

	tmHandler = 
		new TextModelEventHandler<NewProjectDlg>( this,NewProjectDlg::onProjectNameTextChanged,"projNameHandler"  );	
	projName->getTextModel()->addTextModelChangedHandler( tmHandler );


	m_projectLocation = new Label();
	m_projectLocation->setBounds( &Rect( left, top, right, top + m_projectLocation->getPreferredHeight() ) );
	m_projectLocation->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );
	add( m_projectLocation );

	top += m_projectLocation->getPreferredHeight() + 8;

	populateProjectTypes();

	CommandButton* okBtn = new CommandButton();
	double x = right - (okBtn->getPreferredWidth() * 2 + 10);
	okBtn->setBounds( &Rect( x, top, x + okBtn->getPreferredWidth(), top + okBtn->getPreferredHeight() ) );	
	okBtn->setCaption( "OK" );
	okBtn->setCommandType( BC_OK );
	add( okBtn );
	okBtn->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );


	x += okBtn->getPreferredWidth() + 10;
	CommandButton* cancelBtn = new CommandButton();
	cancelBtn->setBounds( &Rect(x, top, x + cancelBtn->getPreferredWidth(), top + cancelBtn->getPreferredHeight() ) );
	cancelBtn->setCaption( "Cancel" );
	cancelBtn->setCommandType( BC_CANCEL );
	add( cancelBtn );
	cancelBtn->setAnchor( ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT );

}

NewProjectDlg::~NewProjectDlg()
{
	std::vector<ProjectListing*>::iterator it = m_projectListings.begin();
	while ( it != m_projectListings.end() ) {
		ProjectListing* pl = *it;
		delete pl;
		pl = NULL;
		it ++;
	}
	m_projectListings.clear();
}	

String NewProjectDlg::getSelectedTemplateProjectPath()
{
	String result;
	
	Item* item = m_projectTemplates->getSelectedItem();
	if ( NULL != item ) {
		ProjectData* data = (ProjectData*)item->getData();
		if ( NULL != data ) {
			result = data->m_filename;
		}
	}

	return result;
}

void NewProjectDlg::onProjectTemplateItemSelected( ItemEvent* e )
{
	m_projectDesciption->setCaption( "Select a project template to create a new project" );
	
	Item* item = m_projectTemplates->getSelectedItem();
	ProjectData* data = (ProjectData*)item->getData();
	if ( NULL != data ) {
		if ( false == data->m_tooltip.empty() ) {
			m_projectDesciption->setCaption( data->m_tooltip );		
		}
	}
	m_projectDesciption->repaint();
}

void NewProjectDlg::onProjectsTypesTreeClicked( MouseEvent* e )
{
	m_projectDesciption->setCaption( "Select a project template to create a new project" );
	m_projectDesciption->repaint();
	ListModel* lm = m_projectTemplates->getListModel();
	lm->empty();
	TreeItem* foundItem = m_projectTypesTree->findItem( e->getPoint() );
	if ( NULL != foundItem ) {
		ProjectListing* projectListing = (ProjectListing*)foundItem->getData();
		if ( NULL != projectListing ) {			
			std::vector<ProjectData>::iterator it = projectListing->m_projects.begin();
			while ( it != projectListing->m_projects.end() ) {
				ProjectData& data = *it;
				DefaultListItem* project = new DefaultListItem();
				project->setCaption( data.m_name );
				project->setData( (void*)&data );
				lm->addItem( project );
				it ++;
			}
			
		}
	}
}

void NewProjectDlg::onProjectPathTextChanged( TextEvent* event )
{
	m_projectPath = event->getTextModel()->getText();
	String tmp = m_projectPath;
	if ( tmp[tmp.size()-1] != '\\' ) {
		tmp += "\\";
	}
	m_projectLocation->setCaption( "Project created at: " + tmp + m_projectName + ".vcp" );
	m_projectLocation->repaint();
}

void NewProjectDlg::onProjectNameTextChanged( TextEvent* event )
{
	m_projectName = event->getTextModel()->getText();
	String tmp = m_projectPath;
	if ( tmp[tmp.size()-1] != '\\' ) {
		tmp += "\\";
	}
	m_projectLocation->setCaption( "Project created at: " + tmp + m_projectName + ".vcp" );
	m_projectLocation->repaint();
}

void NewProjectDlg::readProjectDir( String directoryName, TreeItem* parentItem )
{
	TreeModel* projectTypesTreeModel = m_projectTypesTree->getTreeModel();
	
	if ( directoryName[directoryName.size()-1] == '\\' ){
		directoryName.erase( directoryName.size()-1, 1 );
	}

	String directoryShortName;
	int lastPos = String::npos;
	int pos = directoryName.find( "\\" );
	while ( String::npos != pos ) {
		lastPos = pos;
		pos = directoryName.find( "\\", pos +1 );
	}
	
	if ( String::npos != lastPos ) {
		directoryShortName = directoryName.substr( lastPos+1, directoryName.size() );
	}

	if ( directoryName[directoryName.size()-1] != '\\' ){
		directoryName += "\\";
	}

	XMLParser parser;

	FileStream fs( directoryName + directoryShortName + ".vcfdir", FS_READ );
	parser.parse( &fs );
	Enumerator<XMLNode*>* parsedNodes = parser.getParsedNodes();
	ProjectListing* projectListing = new ProjectListing ();
	parentItem->setData( (void*)projectListing );

	while ( parsedNodes->hasMoreElements() ) {
		XMLNode* node = parsedNodes->nextElement();		
		if ( node->getName() == "project" ) {
			XMLAttr* attr = node->getAttrByName( "name" );
			if ( NULL != attr ) {
				ProjectData data;
				data.m_name = "Unknown Project";
				data.m_filename = directoryName + attr->getValue();
				attr = node->getAttrByName( "displayName" );
				if ( NULL != attr ) {
					data.m_name = attr->getValue();
				}

				attr = node->getAttrByName( "tooltip" );
				if ( NULL != attr ) {
					data.m_tooltip = attr->getValue();
				}
				
				projectListing->m_projects.push_back( data );
			}
		}
		else if ( node->getName() == "projectdir" ) {
			XMLAttr* attr = node->getAttrByName( "name" );
			if ( NULL != attr ) {
				DefaultTreeItem* projectDirItem = new DefaultTreeItem( attr->getValue() );
				projectTypesTreeModel->addNodeItem( projectDirItem, parentItem );

				readProjectDir( directoryName + attr->getValue(), projectDirItem );
			}
		}
	}
}

void NewProjectDlg::populateProjectTypes()
{
	TreeModel* projectTypesTreeModel = m_projectTypesTree->getTreeModel();
	DefaultTreeItem* vcfProjects = new DefaultTreeItem("VCF Builder Projects");
	projectTypesTreeModel->addNodeItem( vcfProjects, NULL );

	String objectRepos = VCFBuilderUIApplication::getVCFBuilderUI()->getObjectRepositoryPath();

	readProjectDir( objectRepos, vcfProjects );
	
}