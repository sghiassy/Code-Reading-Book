//DxtJpeg.h

#ifndef DEXTERLIB_DXTJPEG_H__
#define DEXTERLIB_DXTJPEG_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_DXTJPEG_CLASSID 		"DE75D012-7A65-11D2-8CEA-00A0C9441E20"
/**
*class DxtJpeg
*UUID: DE75D012-7A65-11D2-8CEA-00A0C9441E20
*ProgID: 
*/
class DxtJpeg : public VCF::Object, public IDxtJpeg {
public:
	BEGIN_CLASSINFO( DxtJpeg, "DexterLib::DxtJpeg", "VCF::Object", DEXTERLIB_DXTJPEG_CLASSID );
	END_CLASSINFO( DxtJpeg );

	DxtJpeg();

	virtual ~DxtJpeg();

	virtual long getCapabilities(  );//[id(10000)]

	virtual float getProgress(  );//[id(10001)]

	virtual void setProgress( float Val );//[id(10001)]

	virtual float getStepResolution(  );//[id(10002)]

	virtual float getDuration(  );//[id(10003)]

	virtual void setDuration( float Val );//[id(10003)]

	virtual long getMaskNum(  );//[id(1)]

	virtual void setMaskNum( long Val );//[id(1)]

	virtual BSTR getMaskName(  );//[id(2)]

	virtual void setMaskName( BSTR Val );//[id(2)]

	virtual double getScaleX(  );//[id(3)]

	virtual void setScaleX( double Val );//[id(3)]

	virtual double getScaleY(  );//[id(4)]

	virtual void setScaleY( double Val );//[id(4)]

	virtual long getOffsetX(  );//[id(5)]

	virtual void setOffsetX( long Val );//[id(5)]

	virtual long getOffsetY(  );//[id(6)]

	virtual void setOffsetY( long Val );//[id(6)]

	virtual long getReplicateX(  );//[id(7)]

	virtual void setReplicateX( long Val );//[id(7)]

	virtual long getReplicateY(  );//[id(8)]

	virtual void setReplicateY( long Val );//[id(8)]

	virtual long getBorderColor(  );//[id(9)]

	virtual void setBorderColor( long Val );//[id(9)]

	virtual long getBorderWidth(  );//[id(10)]

	virtual void setBorderWidth( long Val );//[id(10)]

	virtual long getBorderSoftness(  );//[id(11)]

	virtual void setBorderSoftness( long Val );//[id(11)]

	virtual void ApplyChanges(  );//[id(1610809366)]

	virtual void LoadDefSettings(  );//[id(1610809367)]

};

}  //end of namespace DexterLib

#endif //Adding method 29 of 30 for interface IDxtJpeg

