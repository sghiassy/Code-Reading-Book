//NewProjectDlg.h

#ifndef _NEWPROJECTDLG_H__
#define _NEWPROJECTDLG_H__



#include "ClassInfo.h"


using namespace VCF;


#define NEWPROJECTDLG_CLASSID		"83d06114-2d88-4d25-b7c3-2f0234bc96f4"


namespace VCF {
	class TreeControl;
	class ListViewControl;
	class Label;
};

namespace VCFBuilder  {

class ProjectData {
public:
	String m_name;
	String m_filename;
	String m_tooltip;
};

class ProjectListing {
public:
	std::vector<ProjectData> m_projects;	
};



/**
*Class NewProjectDlg documentation
*/
class NewProjectDlg : public VCF::Dialog { 
public:
	BEGIN_CLASSINFO(NewProjectDlg, "VCFBuilder::NewProjectDlg", "VCF::Dialog", NEWPROJECTDLG_CLASSID)
	END_CLASSINFO(NewProjectDlg)

	NewProjectDlg();

	virtual ~NewProjectDlg();

	void onProjectPathTextChanged( TextEvent* event );

	void onProjectNameTextChanged( TextEvent* event );

	String getProjectPath() {
		return m_projectPath;
	}

	String getProjectName() {
		return m_projectName;
	}

	String getSelectedTemplateProjectPath();
protected:
	String m_projectPath;
	String m_projectName;

	TreeControl* m_projectTypesTree;
	ListViewControl* m_projectTemplates;
	Label* m_projectDesciption;
	Label* m_projectLocation;

	void populateProjectTypes();

	void readProjectDir( String directoryName, TreeItem* parentItem );
	
	void onProjectsTypesTreeClicked( MouseEvent* e );

	void onProjectTemplateItemSelected( ItemEvent* e );

	std::vector<ProjectListing*> m_projectListings;
private:
};


}; //end of namespace VCFBuilder

#endif //_NEWPROJECTDLG_H__



