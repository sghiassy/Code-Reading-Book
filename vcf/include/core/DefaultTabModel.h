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

#ifndef _DEFAULTTABMODEL_H__
#define _DEFAULTTABMODEL_H__


namespace VCF {

#define DEFAULTTABMODEL_CLASSID		"86F02174-3E7F-11d4-8EA7-00207811CFAB"

class APPKIT_API DefaultTabModel : public TabModel {
public:
	BEGIN_CLASSINFO(DefaultTabModel, "VCF::DefaultTabModel", "VCF::TabModel", DEFAULTTABMODEL_CLASSID )
	END_CLASSINFO(DefaultTabModel)

	DefaultTabModel();

	virtual ~DefaultTabModel();

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

	EVENT_HANDLER_LIST(TabPageAdded)
	EVENT_HANDLER_LIST(TabPageRemoved)
	EVENT_HANDLER_LIST(TabPageSelected)

	virtual void addTabPageAddedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TabPageAdded)
	}
    
	virtual void removeTabPageAddedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TabPageAdded)
	}

	virtual void addTabPageRemovedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TabPageRemoved)
	}
    
	virtual void removeTabPageRemovedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TabPageRemoved)
	}

	virtual void addTabPageSelectedHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(TabPageSelected)
	}
    
	virtual void removeTabPageSelectedHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(TabPageSelected)
	}

	FIRE_EVENT(TabPageAdded,TabModelEvent)
	FIRE_EVENT(TabPageRemoved,TabModelEvent)
	FIRE_EVENT(TabPageSelected,TabModelEvent)
	

	virtual void addTabPage( TabPage* page );

	virtual void insertTabPage( const ulong32& index, TabPage* page );
	
	virtual void deleteTabPage( TabPage* page );

	virtual void deleteTabPage( const ulong32& index );

	virtual void clearTabPages();

	virtual TabPage* getPageAt( const ulong32& index );

	virtual TabPage* getSelectedPage();

	virtual void setSelectedPage( TabPage* page );

	virtual void setSelectedPage( const ulong32& index );

	virtual Enumerator<TabPage*>* getPages();
	
    /**
     * validate the model. 
     * The implementation for this can vary widely, or even be nonexistant for model's that do not require validation.
     * The basic idea is to call all the listeners in the list , passing in a local variable to the
     * onModelValidate() methods of the listener's. The variable is initialized to true, and if it is
     * still true at the end of the listener iterations, then it is safe to apply the changes to the
     * model, other wise the changes are removed. 
     */
    virtual void validate(){};

    /**
     * clears out the model's data 
     */
    virtual void empty(){};
private:
	std::vector<TabPage*> m_pages;
	EnumeratorContainer<std::vector<TabPage*>, TabPage*> m_container;
};

}; //end of namespace VCF

#endif //_DEFAULTTABMODEL_H__
