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

//DefaultPropertyEditors.h

#ifndef _DEFAULT_PROPERTY_EDITORS_H__ 
#define _DEFAULT_PROPERTY_EDITORS_H__ 



namespace VCF
{

class Control;

class ItemEvent;

class ComboBoxAdapter;

class APPKIT_API AbstractPropertyEditor : public ObjectWithEvents, public PropertyEditor {
public:
	AbstractPropertyEditor();

	virtual ~AbstractPropertyEditor();

	virtual Control* getCustomEditor();
	
	virtual void paintValue( GraphicsContext* context, const Rect& bounds );

	virtual VariantData* getValue();

	virtual void setSource( Object* source );

	virtual void setProperty( Property* property );

	virtual void setValue( VariantData* value );

	/**
	*is it OK to call getCustomEditor ?
	*/
	virtual bool hasCustomEditor();
	
	/**
	*is OK to call paintValue ?
	*/
	virtual bool canPaintValue();

	virtual String getValueAsText();

	virtual void setValueAsText( const String& textValue );

	/**
	*called by an IDE to get the C++ string representation of 
	the property to insert into C++ code.
	*/
	virtual String getCodeString();
	
protected:
	Property* m_property;
	Object* m_source;
	VariantData m_data;
private:
	
};


class APPKIT_API IntegerPropertyEditor : public AbstractPropertyEditor {
public:
	IntegerPropertyEditor(){};

	virtual ~IntegerPropertyEditor(){};	

private:
	
};


class APPKIT_API DoublePropertyEditor : public AbstractPropertyEditor {
public:
	DoublePropertyEditor(){};

	virtual ~DoublePropertyEditor(){};	

private:
	
};

class APPKIT_API StringPropertyEditor : public AbstractPropertyEditor {
public:
	StringPropertyEditor(){};

	virtual ~StringPropertyEditor(){};	

private:
	
};


class ComboBoxControl;

class APPKIT_API BoolPropertyEditor : public AbstractPropertyEditor {
public:
	BoolPropertyEditor();

	virtual ~BoolPropertyEditor();	

	virtual bool hasCustomEditor(){
		return true;	
	};

	virtual Control* getCustomEditor();

	void onSelectionChange( ItemEvent* event );
private:
	ComboBoxControl* m_combo;
	ComboBoxItemEventHandler<BoolPropertyEditor>* m_comboBoxHandler;
};

class APPKIT_API EnumPropertyEditor : public AbstractPropertyEditor{
public:
	EnumPropertyEditor();

	virtual ~EnumPropertyEditor();
	
	virtual bool hasCustomEditor(){
		return true;	
	};

	virtual Control* getCustomEditor();

	void onSelectionChange( ItemEvent* event );
private:
	ComboBoxControl* m_combo;
	ComboBoxItemEventHandler<EnumPropertyEditor>* m_comboBoxHandler;
};



class APPKIT_API ColorPropertyEditor : public AbstractPropertyEditor {
public:
	ColorPropertyEditor();

	virtual ~ColorPropertyEditor();
	
	virtual bool hasCustomEditor(){
		return true;	
	};

	virtual bool canPaintValue(){
		return true;
	}

	virtual void paintValue( GraphicsContext* context, const Rect& bounds );

	virtual Control* getCustomEditor();

	void showColorEditor( VariantData* data );
private:
	
};

class APPKIT_API FontPropertyEditor : public AbstractPropertyEditor {
public:
	FontPropertyEditor();

	virtual ~FontPropertyEditor();
	
	virtual bool hasCustomEditor(){
		return true;	
	};

	virtual bool canPaintValue(){
		return true;
	}

	virtual void paintValue( GraphicsContext* context, const Rect& bounds );

	virtual Control* getCustomEditor();

	void showFontEditor( VariantData* data );
private:
	
};


class APPKIT_API DefaultMenuItemPropertyEditor : public AbstractPropertyEditor {
public:
	DefaultMenuItemPropertyEditor();

	virtual ~DefaultMenuItemPropertyEditor();
	
	virtual bool hasCustomEditor(){
		return true;	
	};	

	virtual Control* getCustomEditor();

	void showDefaultMenuItemEditor( VariantData* data );
private:
	
};

class APPKIT_API DefaultListModelPropertyEditor : public AbstractPropertyEditor {
public:
	DefaultListModelPropertyEditor();

	virtual ~DefaultListModelPropertyEditor();
	
	virtual bool hasCustomEditor(){
		return true;	
	};	

	virtual Control* getCustomEditor();

	void showDefaultListModelEditor( VariantData* data );
private:
	
};




template <class T> class ModalPropertyEditorControl : public Panel {
public:
	typedef void (T::*ShowEditorDialog)( VariantData* data );

	ModalPropertyEditorControl( ShowEditorDialog showEditorMethod, VariantData* data, T* source ){
		m_showEditorMethod = showEditorMethod;
		m_data = data;
		m_source = source;

		CommandButton* btn = new CommandButton();
		
		ButtonEventHandler<ModalPropertyEditorControl>* handler = 
			new ButtonEventHandler<ModalPropertyEditorControl>( this, ModalPropertyEditorControl<T>::onBtnClick, "ButtonClickHandler"  );
		
		btn->addButtonClickHandler( handler );	

		btn->setBounds( &Rect(0,0,16,16) );
		btn->setCaption( "..." );
		this->add( btn, ALIGN_RIGHT );		
		
		this->setColor( Color::getColor("white") );
		this->setBorder( NULL );
	}

	virtual ~ModalPropertyEditorControl(){};	

	void onBtnClick( ButtonEvent* event ){
		(m_source->*m_showEditorMethod)( m_data );
	}		
private:
	ShowEditorDialog m_showEditorMethod;
	VariantData* m_data;
	T* m_source;
};


};


#endif