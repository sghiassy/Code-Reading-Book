//ProjectTreeModel.h

#ifndef _PROJECTTREEMODEL_H__
#define _PROJECTTREEMODEL_H__



namespace VCFBuilder  {

/**
*Class ProjectTreeModel documentation
*/
class ProjectTreeModel : public VCF::DefaultTreeModel { 
public:
	ProjectTreeModel();

	virtual ~ProjectTreeModel();

	TreeItem* find( void* userDataKey );


	virtual void addNodeItem( VCF::TreeItem * node, VCF::TreeItem * nodeParent=NULL );

	virtual void insertNodeItem( VCF::TreeItem * node, VCF::TreeItem * nodeToInsertAfter);

	virtual void deleteNodeItem( VCF::TreeItem * nodeToDelete);

protected:
	std::map<long, TreeItem*> m_treeItemMap;

};


}; //end of namespace VCFBuilder

#endif //_PROJECTTREEMODEL_H__


