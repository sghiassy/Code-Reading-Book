//Splitter.h

#ifndef _SPLITTER_H__
#define _SPLITTER_H__



using namespace VCF;


#define SPLITTER_CLASSID		"e06c4170-04d7-4ba5-84ff-d1718d7b2a9d"


namespace VCF  {

/**
*Class Splitter documentation
*/
class APPKIT_API Splitter : public CustomControl { 
public:
	BEGIN_CLASSINFO(Splitter, "VCF::Splitter", "VCF::CustomControl", SPLITTER_CLASSID)
	PROPERTY(bool,"updateAttachedControl",Splitter::getUpdateAttachedControl, Splitter::setUpdateAttachedControl,PROP_BOOL)
	END_CLASSINFO(Splitter)

	Splitter();

	virtual ~Splitter();

	void init();

	virtual void mouseDown( MouseEvent* e );

	virtual void mouseMove( MouseEvent* e );

	virtual void mouseUp( MouseEvent* e );

	virtual void paint( GraphicsContext* ctx );

	bool getUpdateAttachedControl() {
		return m_updateAttachedControl;
	}

	void setUpdateAttachedControl( const bool& updateAttachedControl ) {
		m_updateAttachedControl = updateAttachedControl;
	}
protected:
	Control* m_attachedControl;
	Point m_dragPoint;
	double m_delta;
	Light3DBorder m_bdr;
	bool m_updateAttachedControl;

	void updateAttachedControl( Point* pt );
private:
};


}; //end of namespace VCF

#endif //_SPLITTER_H__


