//DexterLib.cpp

#include "DexterLib.h"
#include "DexterLibInterfaces.h"
#include "AMTimeline.h"
#include "AMTimelineObj.h"
#include "AMTimelineSrc.h"
#include "AMTimelineTrack.h"
#include "AMTimelineComp.h"
#include "AMTimelineGroup.h"
#include "AMTimelineTrans.h"
#include "AMTimelineEffect.h"
#include "RenderEngine.h"
#include "SmartRenderEngine.h"
#include "AudMixer.h"
#include "Xml2Dex.h"
#include "MediaLocator.h"
#include "PropertySetter.h"
#include "MediaDet.h"
#include "SampleGrabber.h"
#include "NullRenderer.h"
#include "DxtCompositor.h"
#include "DxtAlphaSetter.h"
#include "DxtJpeg.h"
#include "ColorSource.h"
#include "DxtKey.h"


using namespace VCF;
using namespace DexterLib;


void registerDexterLibClasses()
{
	REGISTER_CLASSINFO( AMTimeline );
	REGISTER_CLASSINFO( AMTimelineObj );
	REGISTER_CLASSINFO( AMTimelineSrc );
	REGISTER_CLASSINFO( AMTimelineTrack );
	REGISTER_CLASSINFO( AMTimelineComp );
	REGISTER_CLASSINFO( AMTimelineGroup );
	REGISTER_CLASSINFO( AMTimelineTrans );
	REGISTER_CLASSINFO( AMTimelineEffect );
	REGISTER_CLASSINFO( RenderEngine );
	REGISTER_CLASSINFO( SmartRenderEngine );
	REGISTER_CLASSINFO( AudMixer );
	REGISTER_CLASSINFO( Xml2Dex );
	REGISTER_CLASSINFO( MediaLocator );
	REGISTER_CLASSINFO( PropertySetter );
	REGISTER_CLASSINFO( MediaDet );
	REGISTER_CLASSINFO( SampleGrabber );
	REGISTER_CLASSINFO( NullRenderer );
	REGISTER_CLASSINFO( DxtCompositor );
	REGISTER_CLASSINFO( DxtAlphaSetter );
	REGISTER_CLASSINFO( DxtJpeg );
	REGISTER_CLASSINFO( ColorSource );
	REGISTER_CLASSINFO( DxtKey );
}

