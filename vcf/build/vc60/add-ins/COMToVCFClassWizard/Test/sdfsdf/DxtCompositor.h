//DxtCompositor.h

#ifndef DEXTERLIB_DXTCOMPOSITOR_H__
#define DEXTERLIB_DXTCOMPOSITOR_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_DXTCOMPOSITOR_CLASSID 		"BB44391D-6ABD-422F-9E2E-385C9DFF51FC"
/**
*class DxtCompositor
*UUID: BB44391D-6ABD-422F-9E2E-385C9DFF51FC
*ProgID: 
*/
class DxtCompositor : public VCF::Object, public IDxtCompositor {
public:
	BEGIN_CLASSINFO( DxtCompositor, "DexterLib::DxtCompositor", "VCF::Object", DEXTERLIB_DXTCOMPOSITOR_CLASSID );
	END_CLASSINFO( DxtCompositor );

	DxtCompositor();

	virtual ~DxtCompositor();

	virtual long getCapabilities(  );//[id(10000)]

	virtual float getProgress(  );//[id(10001)]

	virtual void setProgress( float Val );//[id(10001)]

	virtual float getStepResolution(  );//[id(10002)]

	virtual float getDuration(  );//[id(10003)]

	virtual void setDuration( float Val );//[id(10003)]

	virtual long getOffsetX(  );//[id(1)]

	virtual void setOffsetX( long Val );//[id(1)]

	virtual long getOffsetY(  );//[id(2)]

	virtual void setOffsetY( long Val );//[id(2)]

	virtual long getWidth(  );//[id(3)]

	virtual void setWidth( long Val );//[id(3)]

	virtual long getHeight(  );//[id(4)]

	virtual void setHeight( long Val );//[id(4)]

	virtual long getSrcOffsetX(  );//[id(5)]

	virtual void setSrcOffsetX( long Val );//[id(5)]

	virtual long getSrcOffsetY(  );//[id(6)]

	virtual void setSrcOffsetY( long Val );//[id(6)]

	virtual long getSrcWidth(  );//[id(7)]

	virtual void setSrcWidth( long Val );//[id(7)]

	virtual long getSrcHeight(  );//[id(8)]

	virtual void setSrcHeight( long Val );//[id(8)]

};

}  //end of namespace DexterLib

#endif //Adding method 21 of 22 for interface IDxtCompositor

