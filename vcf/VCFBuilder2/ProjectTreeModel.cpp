//ProjectTreeModel.h
#include "ApplicationKit.h"
#include "DefaultTreeModel.h"
#include "ProjectTreeModel.h"

using namespace VCF;

using namespace VCFBuilder;


ProjectTreeModel::ProjectTreeModel()
{

}

ProjectTreeModel::~ProjectTreeModel()
{

}

TreeItem* ProjectTreeModel::find( void* userDataKey )
{
	TreeItem* result = NULL;
	
	std::map<long, TreeItem*>::iterator found = m_treeItemMap.find( (long)userDataKey );
	if ( found != m_treeItemMap.end() ) {
		result = found->second;
	}
	
	return result;
}

void ProjectTreeModel::addNodeItem( TreeItem * node, TreeItem * nodeParent )
{
	DefaultTreeModel::addNodeItem( node, nodeParent );
	if ( NULL != node->getData() ) {
		m_treeItemMap[(long)node->getData()] = node;
	}
}

void ProjectTreeModel::insertNodeItem(TreeItem * node, TreeItem * nodeToInsertAfter)
{
	DefaultTreeModel::insertNodeItem( node, nodeToInsertAfter );
	if ( NULL != node->getData() ) {
		m_treeItemMap[(long)node->getData()] = node;
	}
}

void ProjectTreeModel::deleteNodeItem(TreeItem * nodeToDelete)
{
	if ( NULL != nodeToDelete->getData() ) {
		std::map<long, TreeItem*>::iterator found = m_treeItemMap.find( (long)nodeToDelete->getData() );
		if ( found != m_treeItemMap.end() ) {
			m_treeItemMap.erase( found );
		}		
	}
	DefaultTreeModel::deleteNodeItem( nodeToDelete );
}