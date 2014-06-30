//DxtAlphaSetter.h

#ifndef DEXTERLIB_DXTALPHASETTER_H__
#define DEXTERLIB_DXTALPHASETTER_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_DXTALPHASETTER_CLASSID 		"506D89AE-909A-44F7-9444-ABD575896E35"
/**
*class DxtAlphaSetter
*UUID: 506D89AE-909A-44F7-9444-ABD575896E35
*ProgID: 
*/
class DxtAlphaSetter : public VCF::Object, public IDxtAlphaSetter {
public:
	BEGIN_CLASSINFO( DxtAlphaSetter, "DexterLib::DxtAlphaSetter", "VCF::Object", DEXTERLIB_DXTALPHASETTER_CLASSID );
	END_CLASSINFO( DxtAlphaSetter );

	DxtAlphaSetter();

	virtual ~DxtAlphaSetter();

	virtual long getCapabilities(  );//[id(10000)]

	virtual float getProgress(  );//[id(10001)]

	virtual void setProgress( float Val );//[id(10001)]

	virtual float getStepResolution(  );//[id(10002)]

	virtual float getDuration(  );//[id(10003)]

	virtual void setDuration( float Val );//[id(10003)]

	virtual long getAlpha(  );//[id(1)]

	virtual void setAlpha( long Val );//[id(1)]

	virtual double getAlphaRamp(  );//[id(2)]

	virtual void setAlphaRamp( double Val );//[id(2)]

};

}  //end of namespace DexterLib

#endif //Adding method 9 of 10 for interface IDxtAlphaSetter

