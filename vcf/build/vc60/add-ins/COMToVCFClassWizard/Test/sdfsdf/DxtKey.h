//DxtKey.h

#ifndef DEXTERLIB_DXTKEY_H__
#define DEXTERLIB_DXTKEY_H__
#include "FoundationKit.h"

namespace DexterLib  {

#define DEXTERLIB_DXTKEY_CLASSID 		"C5B19592-145E-11D3-9F04-006008039E37"
/**
*class DxtKey
*UUID: C5B19592-145E-11D3-9F04-006008039E37
*ProgID: 
*/
class DxtKey : public VCF::Object, public IDxtKey {
public:
	BEGIN_CLASSINFO( DxtKey, "DexterLib::DxtKey", "VCF::Object", DEXTERLIB_DXTKEY_CLASSID );
	END_CLASSINFO( DxtKey );

	DxtKey();

	virtual ~DxtKey();

	virtual long getCapabilities(  );//[id(10000)]

	virtual float getProgress(  );//[id(10001)]

	virtual void setProgress( float Val );//[id(10001)]

	virtual float getStepResolution(  );//[id(10002)]

	virtual float getDuration(  );//[id(10003)]

	virtual void setDuration( float Val );//[id(10003)]

	virtual int getKeyType(  );//[id(1)]

	virtual void setKeyType( int Val );//[id(1)]

	virtual int getHue(  );//[id(2)]

	virtual void setHue( int Val );//[id(2)]

	virtual int getLuminance(  );//[id(3)]

	virtual void setLuminance( int Val );//[id(3)]

	virtual unsigned long getRGB(  );//[id(4)]

	virtual void setRGB( unsigned long Val );//[id(4)]

	virtual int getSimilarity(  );//[id(5)]

	virtual void setSimilarity( int Val );//[id(5)]

	virtual long getInvert(  );//[id(6)]

	virtual void setInvert( long Val );//[id(6)]

};

}  //end of namespace DexterLib

#endif //Adding method 17 of 18 for interface IDxtKey

