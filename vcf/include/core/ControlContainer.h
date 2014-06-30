//ControlContainer.h

#ifndef _CONTROLCONTAINER_H__
#define _CONTROLCONTAINER_H__



using namespace VCF;


#define CONTROLCONTAINER_CLASSID		"708b26ee-c180-4e73-89f2-f624d5593d8e"


namespace VCF  {

/**
*Class ControlContainer documentation
*/
class APPKIT_API ControlContainer : public VCF::CustomControl, public AbstractContainer { 
public:
	BEGIN_CLASSINFO(ControlContainer, "VCF::ControlContainer", "VCF::CustomControl", CONTROLCONTAINER_CLASSID)
	PROPERTY(double, "bottomBorderHeight", ControlContainer::getBottomBorderHeight, ControlContainer::setBottomBorderHeight, PROP_DOUBLE);
	PROPERTY(double, "topBorderHeight", ControlContainer::getTopBorderHeight, ControlContainer::setTopBorderHeight, PROP_DOUBLE);
	PROPERTY(double, "rightBorderWidth", ControlContainer::getRightBorderWidth, ControlContainer::setRightBorderWidth, PROP_DOUBLE);
	PROPERTY(double, "leftBorderWidth", ControlContainer::getLeftBorderWidth, ControlContainer::setLeftBorderWidth, PROP_DOUBLE);
	PROPERTY(double, "borderSize", ControlContainer::getBorderSize, ControlContainer::setBorderSize, PROP_DOUBLE);
	END_CLASSINFO(ControlContainer)

	ControlContainer( const bool& heavyWeight=true );

	virtual ~ControlContainer();

	void init();

	double getBottomBorderHeight();

	void setBottomBorderHeight( const double& bottomBorderHeight );

	double getTopBorderHeight();

	void setTopBorderHeight( const double& topBorderHeight );

	double getRightBorderWidth();

	void setRightBorderWidth( const double& rightBorderWidth );

	double getLeftBorderWidth();

	void setLeftBorderWidth( const double& leftBorderWidth );

	double getBorderSize();

	void setBorderSize( const double& borderSize );

	virtual void paint( GraphicsContext* context );

	virtual void resizeChildren();
protected:

private:
	double m_bottomBorderHeight;
	double m_topBorderHeight;
	double m_rightBorderWidth;
	double m_leftBorderWidth;	
};


}; //end of namespace VCF

#endif //_CONTROLCONTAINER_H__


