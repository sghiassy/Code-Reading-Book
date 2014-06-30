//UIForm.h

#ifndef _UIFORM_H__
#define _UIFORM_H__


using namespace VCF;


#define UIFORM_CLASSID		"018d97ef-a049-4b2e-a829-1922e727e896"


namespace VCFBuilder  {

/**
*Class UIForm documentation
*/
class UIForm : public Form { 
public:	
	UIForm( ProjectObject* parent, Project* owningProject );

	virtual ~UIForm();

	virtual void setName( const String& name );

	Frame* getFrameControl() {
		return m_frameControl;
	}

	void setFrameControl( Frame* frameControl ) {
		m_frameControl = frameControl;
		Form::setRootComponent( m_frameControl );
	}

	virtual void setRootComponent( Component* rootComponent ) {		
		m_frameControl = dynamic_cast<Frame*>(rootComponent);
		Form::setRootComponent( m_frameControl );
	}

	CPPClass* getFormClass() {
		return m_formCPPClass;
	}

protected:
	CPPClass* m_formCPPClass;	

	Frame* m_frameControl;
};


}; //end of namespace VCFBuilder

#endif //_UIFORM_H__


