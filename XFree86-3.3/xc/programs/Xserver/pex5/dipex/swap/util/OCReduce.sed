## 
# $XConsortium: OCReduce.sed,v 5.2 94/04/17 20:36:37 rws Exp $
## 
###########################################################################
## Copyright 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.
## 
##                         All Rights Reserved
## 
## Permission to use, copy, modify, and distribute this software and its 
## documentation for any purpose and without fee is hereby granted, 
## provided that the above copyright notice appear in all copies and that
## both that copyright notice and this permission notice appear in 
## supporting documentation, and that the names of Sun Microsystems
## and the X Consortium not be used in advertising or publicity 
## pertaining to distribution of the software without specific, written 
## prior permission.  
## 
## SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
## INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
## SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
## DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
## WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
## ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
## SOFTWARE.
## 
###########################################################################
## Usage:
##	rm -f temp.dat
##	echo "STUB_NAME " <stub_name> | cat > temp.dat
##	awk -f OCTables.awk temp.dat <pex-include-path>/PEX.h | awk -f OCReduce.awk | sed -f OCReduce.sed > <output_file>
##	
s/SWAP_OC_PREFIX(MarkerType)/SwapPEXMarkerType/g
s/SWAP_OC_PREFIX(MarkerScale)/SwapPEXMarkerScale/g
s/SWAP_OC_PREFIX(MarkerColourIndex)/SwapPEXMarkerColourIndex/g
s/SWAP_OC_PREFIX(MarkerBundleIndex)/SwapPEXMarkerBundleIndex/g
s/SWAP_OC_PREFIX(AtextStyle)/SwapPEXAtextStyle/g
s/SWAP_OC_PREFIX(TextPrecision)/SwapPEXTextPrecision/g
s/SWAP_OC_PREFIX(CharExpansion)/SwapPEXCharExpansion/g
s/SWAP_OC_PREFIX(CharSpacing)/SwapPEXCharSpacing/g
s/SWAP_OC_PREFIX(CharHeight)/SwapPEXCharHeight/g
s/SWAP_OC_PREFIX(CharUpVector)/SwapPEXCharUpVector/g
s/SWAP_OC_PREFIX(TextPath)/SwapPEXTextPath/g
s/SWAP_OC_PREFIX(TextAlignment)/SwapPEXTextAlignment/g
s/SWAP_OC_PREFIX(LineType)/SwapPEXLineType/g
s/SWAP_OC_PREFIX(LineWidth)/SwapPEXLineWidth/g
s/SWAP_OC_PREFIX(LineColourIndex)/SwapPEXLineColourIndex/g
s/SWAP_OC_PREFIX(CurveApproximation)/SwapPEXCurveApproximation/g
s/SWAP_OC_PREFIX(PolylineInterp)/SwapPEXPolylineInterp/g
s/SWAP_OC_PREFIX(InteriorStyle)/SwapPEXInteriorStyle/g
s/SWAP_OC_PREFIX(SurfaceColourIndex)/SwapPEXSurfaceColourIndex/g
s/SWAP_OC_PREFIX(SurfaceReflModel)/SwapPEXSurfaceReflModel/g
s/SWAP_OC_PREFIX(SurfaceInterp)/SwapPEXSurfaceInterp/g
s/SWAP_OC_PREFIX(SurfaceApproximation)/SwapPEXSurfaceApproximation/g
s/SWAP_OC_PREFIX(CullingMode)/SwapPEXCullingMode/g
s/SWAP_OC_PREFIX(DistinguishFlag)/SwapPEXDistinguishFlag/g
s/SWAP_OC_PREFIX(PatternSize)/SwapPEXPatternSize/g
s/SWAP_OC_PREFIX(PatternRefPt)/SwapPEXPatternRefPt/g
s/SWAP_OC_PREFIX(PatternAttr)/SwapPEXPatternAttr/g
s/SWAP_OC_PREFIX(SurfaceEdgeFlag)/SwapPEXSurfaceEdgeFlag/g
s/SWAP_OC_PREFIX(SurfaceEdgeType)/SwapPEXSurfaceEdgeType/g
s/SWAP_OC_PREFIX(SurfaceEdgeWidth)/SwapPEXSurfaceEdgeWidth/g
s/SWAP_OC_PREFIX(SetAsfValues)/SwapPEXSetAsfValues/g
s/SWAP_OC_PREFIX(LocalTransform)/SwapPEXLocalTransform/g
s/SWAP_OC_PREFIX(LocalTransform2D)/SwapPEXLocalTransform2D/g
s/SWAP_OC_PREFIX(GlobalTransform)/SwapPEXGlobalTransform/g
s/SWAP_OC_PREFIX(GlobalTransform2D)/SwapPEXGlobalTransform2D/g
s/SWAP_OC_PREFIX(ModelClip)/SwapPEXModelClip/g
s/SWAP_OC_PREFIX(RestoreModelClip)/SwapPEXRestoreModelClip/g
s/SWAP_OC_PREFIX(LightState)/SwapPEXLightState/g
s/SWAP_OC_PREFIX(PickId)/SwapPEXPickId/g
s/SWAP_OC_PREFIX(HlhsrIdentifier)/SwapPEXHlhsrIdentifier/g
s/SWAP_OC_PREFIX(ExecuteStructure)/SwapPEXExecuteStructure/g
s/SWAP_OC_PREFIX(Label)/SwapPEXLabel/g
s/SWAP_OC_PREFIX(ApplicationData)/SwapPEXApplicationData/g
s/SWAP_OC_PREFIX(Gse)/SwapPEXGse/g
s/SWAP_OC_PREFIX(RenderingColourModel)/SwapPEXRenderingColourModel/g
s/SWAP_OC_PREFIX(OCUnused)/SwapPEXOCUnused/g
