//Light3DBorder.h

#ifndef _LIGHT3DBORDER_H__
#define _LIGHT3DBORDER_H__



#define LIGHT3DBORDER_CLASSID		"70ec3332-f241-4f39-8383-9b1ac02945dc"


namespace VCF  {

/**
*Class Light3DBorder documentation
*/
class APPKIT_API Light3DBorder : public Border { 
public:
	BEGIN_CLASSINFO(Light3DBorder, "VCF::Light3DBorder", "VCF::Border", LIGHT3DBORDER_CLASSID)
	END_CLASSINFO(Light3DBorder)

	Light3DBorder();

	virtual ~Light3DBorder();

	virtual void paint( Control* control, GraphicsContext* context );

	virtual void paint( Rect* bounds, GraphicsContext* context );

	/**
	*returns a client rect in which contained components may live.
	*Thus if a particular implementation required a 3 pixel
	*border at the edges to draw itself, then it would retrieve the
	*component's border and then reduce it by 3 pixels. This new
	*value is the available area that the component could use to
	*place child components
	*
	*@param Control* - component that the border belongs to 
	*
	*@return Rect* the client rectangle 
	*/
	virtual Rect* getClientRect( Control* component );

	void setInverted( const bool& inverted ) {
		m_inverted = inverted;
	}
protected:

private:
	bool m_inverted;
};


}; //end of namespace VCF

#endif //_LIGHT3DBORDER_H__


