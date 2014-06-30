/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/



#ifndef ABSTRACTTREEMODEL_H
#define ABSTRACTTREEMODEL_H



namespace VCF{

#define ABSTRACTTREEMODEL_CLASSID		"3126B227-2819-11d4-B53A-00C04F0196DA"


/**
*AbstractTreeModel is a basic implementation of Treemodel. It used for
*deriving classes to have a starting point for implementing TreeModel. 
*For more information on the methods implemented here please see TreeModel.
*
*@version 1.0
*@author Jim Crafton
*/
class APPKIT_API AbstractTreeModel : public TreeModel {
public:

	BEGIN_CLASSINFO(AbstractTreeModel, "VCF::AbstractTreeModel", "VCF::TreeModel", ABSTRACTTREEMODEL_CLASSID)	
	END_CLASSINFO(AbstractTreeModel)

    AbstractTreeModel();

	virtual ~AbstractTreeModel();    

	void init();

	EVENT_HANDLER_LIST(ModelEmptied)
	EVENT_HANDLER_LIST(ModelValidate)
	

    virtual void addModelValidationHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelValidate) 
	}
	
	virtual void removeModelValidationHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelValidate) 
	}

	virtual void addModelHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelEmptied) 
	}

	virtual void removeModelHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelEmptied) 
	}

	FIRE_EVENT(ModelValidate,ValidationEvent)

	FIRE_EVENT(ModelEmptied,ModelEvent)


	EVENT_HANDLER_LIST(RootNodeChanged)
	EVENT_HANDLER_LIST(NodeAdded)
	EVENT_HANDLER_LIST(NodeDeleted)

	virtual void addTreeRootNodeChangedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(RootNodeChanged)
	}

	virtual void removeTreeRootNodeChangedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(RootNodeChanged)
	}

	virtual void addTreeNodeAddedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(NodeAdded)
	}

	virtual void removeTreeNodeAddedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(NodeAdded)
	}

	virtual void addTreeNodeDeletedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(NodeDeleted)
	}

	virtual void removeTreeNodeDeletedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(NodeDeleted)
	}
	
	FIRE_EVENT(RootNodeChanged,TreeModelEvent)
	FIRE_EVENT(NodeAdded,TreeModelEvent)
	FIRE_EVENT(NodeDeleted,TreeModelEvent)

    /**
     * validate the model. 
     * The implementation for this can vary widely, or even be nonexistant for model's that do not 
	 *require validation.
     * The basic idea is to call all the listeners in the list , passing in a local variable to the
     * onModelValidate() methods of the listener's. The variable is initialized to true, and if it is
     * still true at the end of the listener iterations, then it is safe to apply the changes to the
     * model, other wise the changes are removed. 
     */
    virtual void validate();

    /**
     * clears out the model's data 
     */
    virtual void empty();
    
	virtual Enumerator<TreeItem*>* getRootItems();

    virtual void insertNodeItem(TreeItem * node, TreeItem * nodeToInsertAfter);

    virtual void deleteNodeItem(TreeItem * nodeToDelete);

    virtual void addNodeItem( TreeItem * node, TreeItem * nodeParent=NULL );

    virtual void sort();
	
	virtual void onItemPaint( ItemEvent* event );

    virtual void onItemChanged( ItemEvent* event );

    virtual void onItemSelected( ItemEvent* event );

	virtual void onItemAdded( ItemEvent* event );
	
	virtual void onItemDeleted( ItemEvent* event );
protected:
	std::vector<TreeItem*> m_rootNodes;
	EnumeratorContainer<std::vector<TreeItem*>,TreeItem*> m_treeItemContainer;
private:    	
	
};

};
#endif //ABSTRACTTREEMODEL_H