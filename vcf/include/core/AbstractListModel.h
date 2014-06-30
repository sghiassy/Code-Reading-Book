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



#ifndef ABSTRACTLISTMODEL_H
#define ABSTRACTLISTMODEL_H


namespace VCF{

class Stream;


/**
*The AbstractListModel is basic implementation of ListModel. Deriving classes can use it as 
*starting point in their own implementation. See ListModel for more information on the methods
*implemented here.
*
*@version 1.0
*@author Jim Crafton
*/

#define ABSTRACTLISTMODEL_CLASSID		"ED88C0A7-26AB-11d4-B539-00C04F0196DA"

class APPKIT_API AbstractListModel : public ListModel, public Persistable {
public:
	BEGIN_ABSTRACT_CLASSINFO(AbstractListModel, "VCF::AbstractListModel", "VCF::ListModel", ABSTRACTLISTMODEL_CLASSID)		
	END_CLASSINFO(AbstractListModel)

	AbstractListModel();

	virtual ~AbstractListModel();

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

    /**
     * validate the model. 
     * The implementation for this can vary widely, or even be nonexistant for model's that do not require validation.
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
	
	virtual void removeListModelHandler(EventHandler * handler) {
		ADD_EVENT_IMPL(ModelEmptied) 
	}
    
	virtual void addListModelHandler(EventHandler * handler) {
		REMOVE_EVENT_IMPL(ModelEmptied) 
	}

	/**
	*ListModel Handlers
	*/
	EVENT_HANDLER_LIST(ContentsChanged)
	EVENT_HANDLER_LIST(ItemAdded)
	EVENT_HANDLER_LIST(ItemDeleted)

	virtual void addContentsChangedHandler(EventHandler * handler) {
		ADD_EVENT_IMPL(ContentsChanged) 
	}

	virtual void removeContentsChangedHandler(EventHandler * handler) {
		REMOVE_EVENT_IMPL(ContentsChanged) 
	}

	virtual void addItemAddedHandler(EventHandler * handler) {
		ADD_EVENT_IMPL(ItemAdded) 
	}

	virtual void removeItemAddedHandler(EventHandler * handler) {
		REMOVE_EVENT_IMPL(ItemAdded) 
	}

	virtual void addItemDeletedHandler(EventHandler * handler) {
		ADD_EVENT_IMPL(ItemDeleted) 
	}

	virtual void removeItemDeletedHandler(EventHandler * handler) {
		REMOVE_EVENT_IMPL(ItemDeleted) 
	}

	FIRE_EVENT(ContentsChanged,ListModelEvent)
	FIRE_EVENT(ItemAdded,ListModelEvent)
	FIRE_EVENT(ItemDeleted,ListModelEvent)

    virtual void deleteItem(ListItem * item);

    virtual void deleteItem(const unsigned long & index);

    virtual void insertItem(const unsigned long & index, ListItem * item);

    virtual void addItem(ListItem * item);

	virtual ListItem* getItemFromIndex( const unsigned long& index );

	virtual Enumerator<ListItem*>* getItems();

	virtual unsigned long getCount();

	/**
	*Write the object to the specified output stream
	*/
    virtual void saveToStream( OutputStream * stream );
	
	/**
	**Read the object from the specified input stream
	*/
    virtual void loadFromStream( InputStream * stream );	
private:    

    /**
    * @supplierCardinality 0..* 
   */
	std::vector<ListItem*> m_listItems;
	EnumeratorContainer<std::vector<ListItem*>, ListItem*> m_listContainer;
};

};

#endif //ABSTRACTLISTMODEL_H