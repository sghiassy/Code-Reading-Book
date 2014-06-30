//
// Some codeparts of this file are (c) copyright 1992-1999 Silicon Graphics, Inc.
// All Rights Reserved.
//
///////////////////////////////////////////////////////////////////////////////
// This file is freely downloadable (glext.h) from (the SGI contexts): 
//      http://www.berkelium.com/OpenGL/examples/glext.h
//
//////////////////////////////////////////////////////////////////////
// Part of DemoGL Demo System sourcecode. See version information
//////////////////////////////////////////////////////////////////////
// COPYRIGHTS:
// Copyright (c)1999-2001 Solutions Design. All rights reserved.
// Central DemoGL Website: www.demogl.com.
// 
// Released under the following license: (BSD)
// -------------------------------------------
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met: 
//
// 1) Redistributions of source code must retain the above copyright notice, this list of 
//    conditions and the following disclaimer. 
// 2) Redistributions in binary form must reproduce the above copyright notice, this list of 
//    conditions and the following disclaimer in the documentation and/or other materials 
//    provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY SOLUTIONS DESIGN ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SOLUTIONS DESIGN OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
//
// The views and conclusions contained in the software and documentation are those of the authors 
// and should not be interpreted as representing official policies, either expressed or implied, 
// of Solutions Design. 
//
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- SGI
//		- nVidia
//		- ATI
//		- 3dfx
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: Added all known extension definitions and presets. 
// v1.2: Added more extensions.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////
// Last changed on: 03-may-2001.
//////////////////////////////////////////////////////////////////////
#ifndef DEMOGL_GLEXT_H
#define DEMOGL_GLEXT_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////

//////////////////////////
// Define OpenGL 1.2 constants. Not needed if you link to OpenGL 1.2 headers. 
// Not available at the moment for win32.
//////////////////////////
#ifndef GL_VERSION_1_2
#define GL_CONSTANT_COLOR					0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR			0x8002
#define GL_CONSTANT_ALPHA					0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA			0x8004
#define GL_BLEND_COLOR						0x8005
#define GL_FUNC_ADD							0x8006
#define GL_MIN								0x8007
#define GL_MAX								0x8008
#define GL_BLEND_EQUATION					0x8009
#define GL_FUNC_SUBTRACT					0x800A
#define GL_FUNC_REVERSE_SUBTRACT			0x800B
#define GL_CONVOLUTION_1D					0x8010
#define GL_CONVOLUTION_2D					0x8011
#define GL_SEPARABLE_2D						0x8012
#define GL_CONVOLUTION_BORDER_MODE			0x8013
#define GL_CONVOLUTION_FILTER_SCALE			0x8014
#define GL_CONVOLUTION_FILTER_BIAS			0x8015
#define GL_REDUCE							0x8016
#define GL_CONVOLUTION_FORMAT				0x8017
#define GL_CONVOLUTION_WIDTH				0x8018
#define GL_CONVOLUTION_HEIGHT				0x8019
#define GL_MAX_CONVOLUTION_WIDTH			0x801A
#define GL_MAX_CONVOLUTION_HEIGHT			0x801B
#define GL_POST_CONVOLUTION_RED_SCALE		0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE		0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE		0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE		0x801F
#define GL_POST_CONVOLUTION_RED_BIAS		0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS		0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS		0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS		0x8023
#define GL_HISTOGRAM						0x8024
#define GL_PROXY_HISTOGRAM					0x8025
#define GL_HISTOGRAM_WIDTH					0x8026
#define GL_HISTOGRAM_FORMAT					0x8027
#define GL_HISTOGRAM_RED_SIZE				0x8028
#define GL_HISTOGRAM_GREEN_SIZE				0x8029
#define GL_HISTOGRAM_BLUE_SIZE				0x802A
#define GL_HISTOGRAM_ALPHA_SIZE				0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE			0x802C
#define GL_HISTOGRAM_SINK					0x802D
#define GL_MINMAX							0x802E
#define GL_MINMAX_FORMAT					0x802F
#define GL_MINMAX_SINK						0x8030
#define GL_TABLE_TOO_LARGE					0x8031
#define GL_UNSIGNED_BYTE_3_3_2				0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4			0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1			0x8034
#define GL_UNSIGNED_INT_8_8_8_8				0x8035
#define GL_UNSIGNED_INT_10_10_10_2			0x8036
#define GL_RESCALE_NORMAL					0x803A
#define GL_UNSIGNED_BYTE_2_3_3_REV			0x8362
#define GL_UNSIGNED_SHORT_5_6_5				0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV			0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV		0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV		0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV			0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV		0x8368
#define GL_COLOR_MATRIX						0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH			0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH		0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE		0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE	0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE		0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE	0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS		0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS		0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS		0x80BA
#define GL_COLOR_TABLE						0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE		0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE	0x80D2
#define GL_PROXY_COLOR_TABLE				0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE 0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE 0x80D5
#define GL_COLOR_TABLE_SCALE				0x80D6
#define GL_COLOR_TABLE_BIAS					0x80D7
#define GL_COLOR_TABLE_FORMAT				0x80D8
#define GL_COLOR_TABLE_WIDTH				0x80D9
#define GL_COLOR_TABLE_RED_SIZE				0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE			0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE			0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE			0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE		0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE		0x80DF
#define GL_CLAMP_TO_EDGE					0x812F
#define GL_TEXTURE_MIN_LOD					0x813A
#define GL_TEXTURE_MAX_LOD					0x813B
#define GL_TEXTURE_BASE_LEVEL				0x813C
#define GL_TEXTURE_MAX_LEVEL				0x813D
#endif // GL_VERSION_1_2


//////////////////////////
// Extension specific constants
//////////////////////////

///////////////////////////// 3DFX ///////////////////////////////////

// 3DFX_texture_compression_FXT1
#define GL_COMPRESSED_RGB_FXT1_3DFX			0x86B0
#define GL_COMPRESSED_RGBA_FXT1_3DFX		0x86B1

// 3DFX_multisample
#define GL_MULTISAMPLE_3DFX					0x86B2
#define GL_SAMPLE_BUFFERS_3DFX				0x86B3
#define GL_SAMPLES_3DFX						0x86B4
#define GL_MULTISAMPLE_BIT_3DFX				0x20000000


///////////////////////////// ARB ///////////////////////////////////

// ARB_multitexture 
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4
#define GL_TEXTURE5_ARB                     0x84C5
#define GL_TEXTURE6_ARB                     0x84C6
#define GL_TEXTURE7_ARB                     0x84C7
#define GL_TEXTURE8_ARB                     0x84C8
#define GL_TEXTURE9_ARB                     0x84C9
#define GL_TEXTURE10_ARB                    0x84CA
#define GL_TEXTURE11_ARB                    0x84CB
#define GL_TEXTURE12_ARB                    0x84CC
#define GL_TEXTURE13_ARB                    0x84CD
#define GL_TEXTURE14_ARB                    0x84CE
#define GL_TEXTURE15_ARB                    0x84CF
#define GL_TEXTURE16_ARB                    0x84D0
#define GL_TEXTURE17_ARB                    0x84D1
#define GL_TEXTURE18_ARB                    0x84D2
#define GL_TEXTURE19_ARB                    0x84D3
#define GL_TEXTURE20_ARB                    0x84D4
#define GL_TEXTURE21_ARB                    0x84D5
#define GL_TEXTURE22_ARB                    0x84D6
#define GL_TEXTURE23_ARB                    0x84D7
#define GL_TEXTURE24_ARB                    0x84D8
#define GL_TEXTURE25_ARB                    0x84D9
#define GL_TEXTURE26_ARB                    0x84DA
#define GL_TEXTURE27_ARB                    0x84DB
#define GL_TEXTURE28_ARB                    0x84DC
#define GL_TEXTURE29_ARB                    0x84DD
#define GL_TEXTURE30_ARB                    0x84DE
#define GL_TEXTURE31_ARB                    0x84DF

// ARB_multisample
#define GL_MULTISAMPLE_ARB					0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB		0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_ARB			0x809F
#define GL_SAMPLE_COVERAGE_ARB				0x80A0
#define GL_SAMPLE_BUFFERS_ARB				0x80A8
#define GL_SAMPLES_ARB						0x80A9
#define GL_SAMPLE_COVERAGE_VALUE_ARB		0x80AA
#define GL_SAMPLE_COVERAGE_INVERT_ARB		0x80AB
#define GL_MULTISAMPLE_BIT_ARB				0x20000000

// ARB_texture_border_clamp
#define GL_CLAMP_TO_BORDER_ARB				0x812D

// ARB_texture_compression
#define GL_COMPRESSED_ALPHA_ARB				0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB			0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB	0x84EB
#define GL_COMPRESSED_INTENSITY_ARB			0x84EC
#define GL_COMPRESSED_RGB_ARB				0x84ED
#define GL_COMPRESSED_RGBA_ARB				0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB		0x84EF
#define GL_TEXTURE_IMAGE_SIZE_ARB			0x86A0
#define GL_TEXTURE_COMPRESSED_ARB			0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A3

// ARB_texture_cube_map
#define GL_NORMAL_MAP_ARB					0x8511	
#define GL_REFLECTION_MAP_ARB				0x8512
#define GL_TEXTURE_CUBE_MAP_ARB				0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB		0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB	0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB	0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB	0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB	0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB		0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB	0x851C

// ARB_texture_env_combine
#define GL_COMBINE_ARB						0x8570
#define GL_COMBINE_RGB_ARB					0x8571
#define GL_COMBINE_ALPHA_ARB				0x8572
#define GL_SOURCE0_RGB_ARB					0x8580
#define GL_SOURCE1_RGB_ARB					0x8581
#define GL_SOURCE2_RGB_ARB					0x8582
#define GL_SOURCE0_ALPHA_ARB				0x8588
#define GL_SOURCE1_ALPHA_ARB				0x8589
#define GL_SOURCE2_ALPHA_ARB				0x858A
#define GL_OPERAND0_RGB_ARB					0x8590
#define GL_OPERAND1_RGB_ARB					0x8591
#define GL_OPERAND2_RGB_ARB					0x8592
#define GL_OPERAND0_ALPHA_ARB				0x8598
#define GL_OPERAND1_ALPHA_ARB				0x8599
#define GL_OPERAND2_ALPHA_ARB				0x859A
#define GL_RGB_SCALE_ARB					0x8573
#define GL_ADD_SIGNED_ARB					0x8574
#define GL_INTERPOLATE_ARB					0x8575
#define GL_CONSTANT_ARB						0x8576
#define GL_PRIMARY_COLOR_ARB				0x8577
#define GL_PREVIOUS_ARB						0x8578
#define GL_SUBTRACT_ARB						0x84E7

// ARB_texture_env_dot3
#define GL_DOT3_RGB_ARB						0x86AE
#define GL_DOT3_RGBA_ARB					0x86AF

// ARB_transpose_matrix
#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB	0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB	0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB		0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX_ARB		0x84E6

// ARB_vertex_blend
#define GL_MAX_VERTEX_UNITS_ARB				0x86A4
#define GL_ACTIVE_VERTEX_UNITS_ARB			0x86A5
#define GL_WEIGHT_SUM_UNITY_ARB				0x86A6		
#define GL_VERTEX_BLEND_ARB					0x86A7
#define GL_CURRENT_WEIGHT_ARB				0x86A8
#define GL_WEIGHT_ARRAY_TYPE_ARB			0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_ARB			0x86AA
#define GL_WEIGHT_ARRAY_SIZE_ARB			0x86AB
#define GL_WEIGHT_ARRAY_POINTER_ARB			0x86AC
#define GL_WEIGHT_ARRAY_ARB					0x86AD
#define GL_MODELVIEW0_ARB					0x1700
#define GL_MODELVIEW1_ARB					0x850a
#define GL_MODELVIEW2_ARB					0x8722
#define GL_MODELVIEW3_ARB					0x8723
#define GL_MODELVIEW4_ARB					0x8724
#define GL_MODELVIEW5_ARB					0x8725
#define GL_MODELVIEW6_ARB					0x8726
#define GL_MODELVIEW7_ARB					0x8727
#define GL_MODELVIEW8_ARB					0x8728
#define GL_MODELVIEW9_ARB					0x8729
#define GL_MODELVIEW10_ARB					0x872A
#define GL_MODELVIEW11_ARB					0x872B
#define GL_MODELVIEW12_ARB					0x872C
#define GL_MODELVIEW13_ARB					0x872D
#define GL_MODELVIEW14_ARB					0x872E
#define GL_MODELVIEW15_ARB					0x872F
#define GL_MODELVIEW16_ARB					0x8730
#define GL_MODELVIEW17_ARB					0x8731
#define GL_MODELVIEW18_ARB					0x8732
#define GL_MODELVIEW19_ARB					0x8733
#define GL_MODELVIEW20_ARB					0x8734
#define GL_MODELVIEW21_ARB					0x8735
#define GL_MODELVIEW22_ARB					0x8736
#define GL_MODELVIEW23_ARB					0x8737
#define GL_MODELVIEW24_ARB					0x8738
#define GL_MODELVIEW25_ARB					0x8739
#define GL_MODELVIEW26_ARB					0x873A
#define GL_MODELVIEW27_ARB					0x873B
#define GL_MODELVIEW28_ARB					0x873C
#define GL_MODELVIEW29_ARB					0x873D
#define GL_MODELVIEW30_ARB					0x873E
#define GL_MODELVIEW31_ARB					0x873F

///////////////////////////// ATI(X) ///////////////////////////////////
// ATIX_envmap_bumpmap
#define GL_BUMP_ROT_MATRIX_ATIX				0x60A0
#define GL_BUMP_ROT_MATRIX_SIZE_ATIX		0x60A1
#define GL_BUMP_NUM_TEX_UNITS_ATIX			0x60A2
#define GL_BUMP_TEX_UNITS_ATIX				0x60A3
#define GL_DUDV_ATIX						0x60A4
#define GL_DU8DV8_ATIX						0x60A5
#define GL_BUMP_ENVMAP_ATIX					0x60A6
#define GL_BUMP_TARGET_ATIX					0x60A7

// ATIX_pn_triangles
#define GL_PN_TRIANGLES_ATIX						0x6090
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATIX	0x6091
#define GL_PN_TRIANGLES_POINT_MODE_ATIX				0x6092
#define GL_PN_TRIANGLES_NORMAL_MODE_ATIX			0x6093
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATIX		0x6094
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATIX		0x6095
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATIX		0x6096
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATIX     0x6097
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATIX  0x6098

// ATIX_texture_env_combine3
#define GL_MODULATE_ADD_ATIX				0x8744
#define GL_MODULATE_SIGNED_ADD_ATIX			0x8745
#define GL_MODULATE_SUBTRACT_ATIX			0x8746

// ATIX_texture_env_dot3
#define GL_DOT3_RGB_ATIX					0x6070
#define GL_DOT3_RGBA_ATIX					0x6071

// ATIX_texture_env_route
#define GL_SECONDARY_COLOR_ATIX				0x8747
#define GL_TEXTURE_OUTPUT_RGB_ATIX			0x8748
#define GL_TEXTURE_OUTPUT_ALPHA_ATIX		0x8749

// ATI_texture_mirror_once
#define GL_MIRROR_CLAMP_ATI					0x6080
#define GL_MIRROR_CLAMP_TO_EDGE_ATI			0x6081

// ATI_vertex_blend 
#define GL_ACTIVE_VERTEX_UNIT_ATI			0x6020
#define GL_CLIENT_ACTIVE_VERTEX_UNIT_ATI	0x6021
#define GL_MAX_VERTEX_UNITS_ATI				0x6022
#define GL_WEIGHT_ARRAY_ATI					0x6023
#define GL_VERTEX_BLENDING_ATI				0x6024
#define GL_CURRENT_WEIGHT_ATI				0x6025
#define GL_WEIGHT_ARRAY_TYPE_ATI			0x6026
#define GL_WEIGHT_ARRAY_STRIDE_ATI			0x6027
#define GL_VERTEX_UNIT0_ATI					0x6000
#define GL_VERTEX_UNIT1_ATI					0x6001
#define GL_VERTEX_UNIT2_ATI					0x6002
#define GL_VERTEX_UNIT3_ATI					0x6003
#define GL_VERTEX_UNIT4_ATI					0x6004
#define GL_VERTEX_UNIT5_ATI					0x6005
#define GL_VERTEX_UNIT6_ATI					0x6006
#define GL_VERTEX_UNIT7_ATI					0x6007
#define GL_VERTEX_UNIT8_ATI					0x6008
#define GL_VERTEX_UNIT9_ATI					0x6009
#define GL_VERTEX_UNIT10_ATI				0x600A
#define GL_VERTEX_UNIT11_ATI				0x600B
#define GL_VERTEX_UNIT12_ATI				0x600C
#define GL_VERTEX_UNIT13_ATI				0x600D
#define GL_VERTEX_UNIT14_ATI				0x600E
#define GL_VERTEX_UNIT15_ATI				0x600F
#define GL_VERTEX_UNIT16_ATI				0x6010
#define GL_VERTEX_UNIT17_ATI				0x6011
#define GL_VERTEX_UNIT18_ATI				0x6012
#define GL_VERTEX_UNIT19_ATI				0x6013
#define GL_VERTEX_UNIT20_ATI				0x6014
#define GL_VERTEX_UNIT21_ATI				0x6015
#define GL_VERTEX_UNIT22_ATI				0x6016
#define GL_VERTEX_UNIT23_ATI				0x6017
#define GL_VERTEX_UNIT24_ATI				0x6018
#define GL_VERTEX_UNIT25_ATI				0x6019
#define GL_VERTEX_UNIT26_ATI				0x601A
#define GL_VERTEX_UNIT27_ATI				0x601B
#define GL_VERTEX_UNIT28_ATI				0x601C
#define GL_VERTEX_UNIT29_ATI				0x601D
#define GL_VERTEX_UNIT30_ATI				0x601E
#define GL_VERTEX_UNIT31_ATI				0x601F

// ATI_vertex_streams 
#define GL_MAX_VERTEX_STREAMS_ATI			0x6050
#define GL_VERTEX_SOURCE_ATI				0x6051
#define GL_VERTEX_STREAM0_ATI				0x6030
#define GL_VERTEX_STREAM1_ATI				0x6031
#define GL_VERTEX_STREAM2_ATI				0x6032
#define GL_VERTEX_STREAM3_ATI				0x6033
#define GL_VERTEX_STREAM4_ATI				0x6034
#define GL_VERTEX_STREAM5_ATI				0x6035
#define GL_VERTEX_STREAM6_ATI				0x6036
#define GL_VERTEX_STREAM7_ATI				0x6037
#define GL_VERTEX_STREAM8_ATI				0x6038
#define GL_VERTEX_STREAM9_ATI				0x6039
#define GL_VERTEX_STREAM10_ATI				0x603A
#define GL_VERTEX_STREAM11_ATI				0x603B
#define GL_VERTEX_STREAM12_ATI				0x603C
#define GL_VERTEX_STREAM13_ATI				0x603D
#define GL_VERTEX_STREAM14_ATI				0x603E
#define GL_VERTEX_STREAM15_ATI				0x603F
#define GL_VERTEX_STREAM16_ATI				0x6040
#define GL_VERTEX_STREAM17_ATI				0x6041
#define GL_VERTEX_STREAM18_ATI				0x6042
#define GL_VERTEX_STREAM19_ATI				0x6043
#define GL_VERTEX_STREAM20_ATI				0x6044
#define GL_VERTEX_STREAM21_ATI				0x6045
#define GL_VERTEX_STREAM22_ATI				0x6046
#define GL_VERTEX_STREAM23_ATI				0x6047
#define GL_VERTEX_STREAM24_ATI				0x6048
#define GL_VERTEX_STREAM25_ATI				0x6049
#define GL_VERTEX_STREAM26_ATI				0x604A
#define GL_VERTEX_STREAM27_ATI				0x604B
#define GL_VERTEX_STREAM28_ATI				0x604C
#define GL_VERTEX_STREAM29_ATI				0x604D
#define GL_VERTEX_STREAM30_ATI				0x604E
#define GL_VERTEX_STREAM31_ATI				0x604F

///////////////////////////// EXT ///////////////////////////////////

// EXT_abgr 
#define GL_ABGR_EXT                         0x8000

// EXT_bgra 
#define GL_BGR_EXT                          0x80E0
#define GL_BGRA_EXT                         0x80E1

// EXT_blend_color 
#define	GL_CONSTANT_COLOR_EXT				0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT		0x8002
#define	GL_CONSTANT_ALPHA_EXT				0x8003
#define	GL_ONE_MINUS_CONSTANT_ALPHA_EXT		0x8004
#define	GL_BLEND_COLOR_EXT					0x8005

// EXT_blend_func_separate
#define GL_BLEND_DST_RGB_EXT				0x80C8
#define GL_BLEND_SRC_RGB_EXT				0x80C9
#define GL_BLEND_DST_ALPHA_EXT				0x80CA
#define GL_BLEND_SRC_ALPHA_EXT				0x80CB

// EXT_blend_minmax 
#define	GL_FUNC_ADD_EXT						0x8006
#define	GL_MIN_EXT							0x8007
#define GL_MAX_EXT							0x8008
#define GL_BLEND_EQUATION_EXT				0x8009

// EXT_blend_subtract 
#define	GL_FUNC_SUBTRACT_EXT				0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT		0x800B

// EXT_clip_volume_hint 
#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT    0x80F0

// EXT_compiled_vertex_array 
#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9

// EXT_convolution
#define GL_CONVOLUTION_1D_EXT				0x8010
#define GL_CONVOLUTION_2D_EXT				0x8011
#define GL_SEPARABLE_2D_EXT					0x8012
#define GL_CONVOLUTION_BORDER_MODE_EXT		0x8013
#define GL_CONVOLUTION_FILTER_SCALE_EXT		0x8014
#define GL_CONVOLUTION_FILTER_BIAS_EXT		0x8015
#define GL_REDUCE_EXT						0x8016
#define GL_CONVOLUTION_FORMAT_EXT			0x8017
#define GL_CONVOLUTION_WIDTH_EXT			0x8018
#define GL_CONVOLUTION_HEIGHT_EXT			0x8019
#define GL_MAX_CONVOLUTION_WIDTH_EXT		0x801A
#define GL_MAX_CONVOLUTION_HEIGHT_EXT		0x801B
#define GL_POST_CONVOLUTION_RED_SCALE_EXT	0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE_EXT 0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE_EXT	0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE_EXT 0x801F
#define GL_POST_CONVOLUTION_RED_BIAS_EXT	0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS_EXT	0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS_EXT	0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS_EXT	0x8023

// EXT_coordinate_frame
#define GL_TANGENT_ARRAY_EXT				0x8439
#define GL_BINORMAL_ARRAY_EXT				0x843A
#define GL_CURRENT_TANGENT_EXT				0x843B
#define GL_CURRENT_BINORMAL_EXT				0x843C
#define GL_TANGENT_ARRAY_TYPE_EXT			0x843E
#define GL_TANGENT_ARRAY_STRIDE_EXT			0x843F
#define GL_BINORMAL_ARRAY_TYPE_EXT			0x8440
#define GL_BINORMAL_ARRAY_STRIDE_EXT		0x8441
#define GL_TANGENT_ARRAY_POINTER_EXT		0x8442
#define GL_BINORMAL_ARRAY_POINTER_EXT		0x8443
#define GL_MAP1_TANGENT_EXT					0x8444
#define GL_MAP2_TANGENT_EXT					0x8445
#define GL_MAP1_BINORMAL_EXT				0x8446
#define GL_MAP2_BINORMAL_EXT				0x8447

// EXT_cull_vertex 
#define GL_CULL_VERTEX_EXT                  0x81AA
#define GL_CULL_VERTEX_EYE_POSITION_EXT     0x81AB
#define GL_CULL_VERTEX_OBJECT_POSITION_EXT  0x81AC

// EXT_draw_range_elements
#define GL_MAX_ELEMENTS_VERTICES_EXT		0x80E8
#define GL_MAX_ELEMENTS_INDICES_EXT			0x80E9

// EXT_fog_coord
#define	GL_FOG_COORDINATE_SOURCE_EXT		0x8450
#define	GL_FOG_COORDINATE_EXT				0x8451
#define	GL_FRAGMENT_DEPTH_EXT				0x8452
#define	GL_CURRENT_FOG_COORDINATE_EXT		0x8453
#define	GL_FOG_COORDINATE_ARRAY_TYPE_EXT	0x8454
#define	GL_FOG_COORDINATE_ARRAY_STRIDE_EXT	0x8455
#define	GL_FOG_COORDINATE_ARRAY_POINTER_EXT 0x8456
#define	GL_FOG_COORDINATE_ARRAY_EXT			0x8457

// EXT_histogram
#define GL_HISTOGRAM_EXT					0x8024
#define GL_PROXY_HISTOGRAM_EXT				0x8025
#define GL_HISTOGRAM_WIDTH_EXT				0x8026
#define GL_HISTOGRAM_FORMAT_EXT				0x8027
#define GL_HISTOGRAM_RED_SIZE_EXT			0x8028
#define GL_HISTOGRAM_GREEN_SIZE_EXT			0x8029
#define GL_HISTOGRAM_BLUE_SIZE_EXT			0x802A
#define GL_HISTOGRAM_ALPHA_SIZE_EXT			0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE_EXT		0x802C
#define GL_HISTOGRAM_SINK_EXT				0x802D
#define GL_MINMAX_EXT						0x802E
#define GL_MINMAX_FORMAT_EXT				0x802F
#define GL_MINMAX_SINK_EXT					0x8030
#define GL_TABLE_TOO_LARGE_EXT				0x8031

// EXT_index_array_formats
#define GL_IUI_V2F_EXT						0x81AD
#define GL_IUI_V3F_EXT						0x81AE
#define GL_IUI_N3F_V2F_EXT					0x81AF
#define GL_IUI_N3F_V3F_EXT					0x81B0
#define GL_T2F_IUI_V2F_EXT					0x81B1
#define GL_T2F_IUI_V3F_EXT					0x81B2
#define GL_T2F_IUI_N3F_V2F_EXT				0x81B3
#define GL_T2F_IUI_N3F_V3F_EXT				0x81B4

// EXT_index_func
#define GL_INDEX_TEST_EXT					0x81B5
#define GL_INDEX_TEST_FUNC_EXT				0x81B6
#define GL_INDEX_TEST_REF_EXT				0x81B7

// EXT_index_material
#define GL_INDEX_MATERIAL_EXT				0x81B8
#define GL_INDEX_MATERIAL_PARAMETER_EXT		0x81B9
#define GL_INDEX_MATERIAL_FACE_EXT			0x81BA

// EXT_light_max_exponent
#define	GL_MAX_SHININESS_EXT				0x8507
#define	GL_MAX_SPOT_EXPONENT_EXT			0x8508

// EXT_light_texture
#define GL_FRAGMENT_MATERIAL_EXT			0x8349
#define GL_FRAGMENT_NORMAL_EXT				0x834A
#define GL_FRAGMENT_COLOR_EXT				0x834C
#define GL_ATTENUATION_EXT					0x834D
#define GL_SHADOW_ATTENUATION_EXT			0x834E
#define GL_TEXTURE_APPLICATION_MODE_EXT		0x834F
#define GL_TEXTURE_LIGHT_EXT				0x8350
#define GL_TEXTURE_MATERIAL_FACE_EXT		0x8351
#define GL_TEXTURE_MATERIAL_PARAMETER_EXT	0x8352

// EXT_multisample
#define GL_MULTISAMPLE_EXT					0x809D
#define GL_SAMPLE_ALPHA_TO_MASK_EXT			0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_EXT			0x809F
#define GL_SAMPLE_MASK_EXT					0x80A0
#define GL_1PASS_EXT						0x80A1
#define GL_2PASS_0_EXT						0x80A2
#define GL_2PASS_1_EXT						0x80A3
#define GL_4PASS_0_EXT						0x80A4
#define GL_4PASS_1_EXT						0x80A5
#define GL_4PASS_2_EXT						0x80A6
#define GL_4PASS_3_EXT						0x80A7
#define GL_SAMPLE_BUFFERS_EXT				0x80A8
#define GL_SAMPLES_EXT						0x80A9
#define GL_SAMPLE_MASK_VALUE_EXT			0x80AA
#define GL_SAMPLE_MASK_INVERT_EXT			0x80AB
#define GL_SAMPLE_PATTERN_EXT				0x80AC

// EXT_packed_pixels 
#define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036

// EXT_paletted_texture 
#define	GL_COLOR_INDEX1_EXT					0x80E2
#define	GL_COLOR_INDEX2_EXT					0x80E3
#define	GL_COLOR_INDEX4_EXT					0x80E4
#define	GL_COLOR_INDEX8_EXT					0x80E5
#define	GL_COLOR_INDEX12_EXT				0x80E6
#define	GL_COLOR_INDEX16_EXT				0x80E7
#define	GL_COLOR_TABLE_FORMAT_EXT			0x80D8
#define	GL_COLOR_TABLE_WIDTH_EXT			0x80D9
#define	GL_COLOR_TABLE_RED_SIZE_EXT			0x80DA
#define	GL_COLOR_TABLE_GREEN_SIZE_EXT		0x80DB
#define	GL_COLOR_TABLE_BLUE_SIZE_EXT		0x80DC
#define	GL_COLOR_TABLE_ALPHA_SIZE_EXT		0x80DD
#define	GL_COLOR_TABLE_LUMINANCE_SIZE_EXT	0x80DE
#define	GL_COLOR_TABLE_INTENSITY_SIZE_EXT	0x80DF
#define	GL_TEXTURE_INDEX_SIZE_EXT			0x80ED

// EXT_pixel_transform
#define GL_PIXEL_TRANSFORM_2D_EXT			0x8330
#define GL_PIXEL_MAG_FILTER_EXT				0x8331
#define GL_PIXEL_MIN_FILTER_EXT				0x8332
#define GL_PIXEL_CUBIC_WEIGHT_EXT			0x8333
#define GL_CUBIC_EXT						0x8334
#define GL_AVERAGE_EXT						0x8335
#define GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT 0x8336
#define GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT 0x8337
#define GL_PIXEL_TRANSFORM_2D_MATRIX_EXT	0x8338

// EXT_point_parameters 
#define GL_POINT_SIZE_MIN_EXT               0x8126
#define GL_POINT_SIZE_MAX_EXT               0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT    0x8128
#define GL_DISTANCE_ATTENUATION_EXT         0x8129

// EXT_polygon_offset
#define GL_POLYGON_OFFSET_EXT				0x8037
#define GL_POLYGON_OFFSET_FACTOR_EXT		0x8038
#define GL_POLYGON_OFFSET_BIAS_EXT			0x8039

// EXT_rescale_normal 
#define	GL_RESCALE_NORMAL_EXT				0x803A

// EXT_secondary_color
#define	GL_COLOR_SUM_EXT					0x8458
#define	GL_CURRENT_SECONDARY_COLOR_EXT		0x8459
#define	GL_SECONDARY_COLOR_ARRAY_SIZE_EXT	0x845A
#define	GL_SECONDARY_COLOR_ARRAY_TYPE_EXT	0x845B
#define	GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT 0x845C
#define	GL_SECONDARY_COLOR_ARRAY_POINTER_EXT 0x845D
#define	GL_SECONDARY_COLOR_ARRAY_EXT		0x845E

// EXT_separate_specular_color
#define	GL_LIGHT_MODEL_COLOR_CONTROL_EXT	0x81F8
#define	GL_SINGLE_COLOR_EXT					0x81F9
#define	GL_SEPARATE_SPECULAR_COLOR_EXT		0x81FA

// EXT_shared_texture_palette 
#define	GL_SHARED_TEXTURE_PALETTE_EXT		0x81FB

// EXT_stencil_wrap 
#define GL_INCR_WRAP_EXT                    0x8507
#define GL_DECR_WRAP_EXT                    0x8508

// EXT_texgen_reflection
#define GL_NORMAL_MAP_EXT                   0x8511
#define GL_REFLECTION_MAP_EXT               0x8512

// EXT_texture
#define GL_ALPHA4_EXT						0x803B
#define GL_ALPHA8_EXT						0x803C
#define GL_ALPHA12_EXT						0x803D
#define GL_ALPHA16_EXT						0x803E
#define GL_LUMINANCE4_EXT					0x803F
#define GL_LUMINANCE8_EXT					0x8040
#define GL_LUMINANCE12_EXT					0x8041
#define GL_LUMINANCE16_EXT					0x8042
#define GL_LUMINANCE4_ALPHA4_EXT			0x8043
#define GL_LUMINANCE6_ALPHA2_EXT			0x8044
#define GL_LUMINANCE8_ALPHA8_EXT			0x8045
#define GL_LUMINANCE12_ALPHA4_EXT			0x8046
#define GL_LUMINANCE12_ALPHA12_EXT			0x8047
#define GL_LUMINANCE16_ALPHA16_EXT			0x8048
#define GL_INTENSITY_EXT					0x8049
#define GL_INTENSITY4_EXT					0x804A
#define GL_INTENSITY8_EXT					0x804B
#define GL_INTENSITY12_EXT					0x804C
#define GL_INTENSITY16_EXT					0x804D
#define GL_RGB2_EXT							0x804E
#define GL_RGB4_EXT							0x804F
#define GL_RGB5_EXT							0x8050
#define GL_RGB8_EXT							0x8051
#define GL_RGB10_EXT						0x8052
#define GL_RGB12_EXT						0x8053
#define GL_RGB16_EXT						0x8054
#define GL_RGBA2_EXT						0x8055
#define GL_RGBA4_EXT						0x8056
#define GL_RGB5_A1_EXT						0x8057
#define GL_RGBA8_EXT						0x8058
#define GL_RGB10_A2_EXT						0x8059
#define GL_RGBA12_EXT						0x805A
#define GL_RGBA16_EXT						0x805B
#define GL_TEXTURE_RED_SIZE_EXT				0x805C
#define GL_TEXTURE_GREEN_SIZE_EXT			0x805D
#define GL_TEXTURE_BLUE_SIZE_EXT			0x805E
#define GL_TEXTURE_ALPHA_SIZE_EXT			0x805F
#define GL_TEXTURE_LUMINANCE_SIZE_EXT		0x8060
#define GL_TEXTURE_INTENSITY_SIZE_EXT		0x8061
#define GL_REPLACE_EXT						0x8062
#define GL_PROXY_TEXTURE_1D_EXT				0x8063
#define GL_PROXY_TEXTURE_2D_EXT				0x8064
#define GL_TEXTURE_TOO_LARGE_EXT			0x8065

// EXT_texture3D
#define GL_TEXTURE_BINDING_3D_EXT		    0x806A
#define GL_PACK_SKIP_IMAGES					0x806B
#define GL_PACK_SKIP_IMAGES_EXT				0x806B
#define GL_PACK_IMAGE_HEIGHT				0x806C
#define GL_PACK_IMAGE_HEIGHT_EXT			0x806C
#define GL_UNPACK_SKIP_IMAGES				0x806D
#define GL_UNPACK_SKIP_IMAGES_EXT			0x806D
#define GL_UNPACK_IMAGE_HEIGHT				0x806E
#define GL_UNPACK_IMAGE_HEIGHT_EXT			0x806E
#define GL_TEXTURE_3D						0x806F
#define GL_TEXTURE_3D_EXT					0x806F
#define GL_PROXY_TEXTURE_3D					0x8070
#define GL_PROXY_TEXTURE_3D_EXT				0x8070
#define GL_TEXTURE_DEPTH					0x8071
#define GL_TEXTURE_DEPTH_EXT				0x8071
#define GL_TEXTURE_WRAP_R					0x8072
#define GL_TEXTURE_WRAP_R_EXT				0x8072
#define GL_MAX_3D_TEXTURE_SIZE				0x8073
#define GL_MAX_3D_TEXTURE_SIZE_EXT			0x8073

// EXT_texture_compression_s3tc & NV_texgen_compression_vtc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT		0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT	0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT	0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT	0x83F3

// EXT_texture_cube_map 
#define	GL_NORMAL_MAP_EXT					0x8511
#define	GL_REFLECTION_MAP_EXT				0x8512
#define	GL_TEXTURE_CUBE_MAP_EXT				0x8513
#define	GL_TEXTURE_BINDING_CUBE_MAP_EXT		0x8514
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT	0x8515
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT	0x8516
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT	0x8517
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT	0x8518
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT	0x8519
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT	0x851a
#define	GL_PROXY_TEXTURE_CUBE_MAP_EXT		0x851b
#define	GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT	0x851c

// EXT_texture_edge_clamp
#define GL_CLAMP_TO_EDGE_EXT                0x812F

// EXT_texture_env_combine 
#define GL_COMBINE_EXT                      0x8570
#define GL_COMBINE_RGB_EXT                  0x8571
#define GL_COMBINE_ALPHA_EXT                0x8572
#define GL_RGB_SCALE_EXT                    0x8573
#define GL_ADD_SIGNED_EXT                   0x8574
#define GL_INTERPOLATE_EXT                  0x8575
#define GL_CONSTANT_EXT                     0x8576
#define GL_PRIMARY_COLOR_EXT                0x8577
#define GL_PREVIOUS_EXT                     0x8578
#define GL_SOURCE0_RGB_EXT                  0x8580
#define GL_SOURCE1_RGB_EXT                  0x8581
#define GL_SOURCE2_RGB_EXT                  0x8582
#define GL_SOURCE0_ALPHA_EXT                0x8588
#define GL_SOURCE1_ALPHA_EXT                0x8589
#define GL_SOURCE2_ALPHA_EXT                0x858A
#define GL_OPERAND0_RGB_EXT                 0x8590
#define GL_OPERAND1_RGB_EXT                 0x8591
#define GL_OPERAND2_RGB_EXT                 0x8592
#define GL_OPERAND0_ALPHA_EXT               0x8598
#define GL_OPERAND1_ALPHA_EXT               0x8599
#define GL_OPERAND2_ALPHA_EXT               0x859A

// EXT_texture_env_dot3
#define GL_DOT3_RGB_EXT						0x8740
#define GL_DOT3_RGBA_EXT					0x8741

// EXT_texture_filter_anisotropic 
#define	GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84fe
#define	GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84ff

// EXT_texture_lod_bias 
#define	GL_TEXTURE_FILTER_CONTROL_EXT		0x8500
#define	GL_TEXTURE_LOD_BIAS_EXT				0x8501
#define	GL_MAX_TEXTURE_LOD_BIAS_EXT			0x84fd

// EXT_texture_object (Obsolete in OpenGL 1.1+)
#define	GL_TEXTURE_PRIORITY_EXT				0x8066
#define	GL_TEXTURE_RESIDENT_EXT				0x8067
#define	GL_TEXTURE_1D_BINDING_EXT			0x8068
#define	GL_TEXTURE_2D_BINDING_EXT			0x8069
#define	GL_TEXTURE_3D_BINDING_EXT			0x806A

// EXT_texture_perturb_normal
#define GL_PERTURB_EXT						0x85AE
#define GL_TEXTURE_NORMAL_EXT				0x85AF

// EXT_vertex_array 
#define GL_VERTEX_ARRAY_EXT                 0x8074
#define GL_NORMAL_ARRAY_EXT                 0x8075
#define GL_COLOR_ARRAY_EXT                  0x8076
#define GL_INDEX_ARRAY_EXT                  0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT          0x8078
#define GL_EDGE_FLAG_ARRAY_EXT              0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT            0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT            0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT          0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT           0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT            0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT          0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT           0x8080
#define GL_COLOR_ARRAY_SIZE_EXT             0x8081
#define GL_COLOR_ARRAY_TYPE_EXT             0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT           0x8083
#define GL_COLOR_ARRAY_COUNT_EXT            0x8084
#define GL_INDEX_ARRAY_TYPE_EXT             0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT           0x8086
#define GL_INDEX_ARRAY_COUNT_EXT            0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT     0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT     0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT   0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT    0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT       0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT        0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT         0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT         0x808F
#define GL_COLOR_ARRAY_POINTER_EXT          0x8090
#define GL_INDEX_ARRAY_POINTER_EXT          0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT  0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT      0x8093

// EXT_vertex_weighting
#define	GL_VERTEX_WEIGHTING_EXT				0x8509
#define	GL_MODELVIEW0_EXT					0x1700
#define	GL_MODELVIEW1_EXT					0x850a
#define	GL_CURRENT_VERTEX_WEIGHT_EXT		0x850b
#define	GL_VERTEX_WEIGHT_ARRAY_EXT			0x850c
#define	GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT		0x850d
#define	GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT		0x850e
#define	GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT	0x850f
#define	GL_MODELVIEW0_STACK_DEPTH_EXT		0x0BA3
#define	GL_MODELVIEW1_STACK_DEPTH_EXT		0x8502
#define	GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT	0x8510


///////////////////////////// IBM ///////////////////////////////////

// IBM_texture_mirrored_repeat
#define GL_MIRRORED_REPEAT_IBM				0x8370


///////////////////////////// KTX ///////////////////////////////////

// KTX_buffer_region
#define GL_KTX_FRONT_REGION					0x0
#define GL_KTX_BACK_REGION					0x1
#define GL_KTX_Z_REGION						0x2
#define GL_KTX_STENCIL_REGION				0x3


///////////////////////////// NV ///////////////////////////////////
// NV_evaluators
#define GL_EVAL_2D_NV                     0x86C0
#define GL_EVAL_TRIANGULAR_2D_NV          0x86C1
#define GL_MAP_TESSELLATION_NV            0x86C2
#define GL_MAP_ATTRIB_U_ORDER_NV          0x86C3
#define GL_MAP_ATTRIB_V_ORDER_NV          0x86C4
#define GL_EVAL_FRACTIONAL_TESSELLATION_NV 0x86C5
#define GL_EVAL_VERTEX_ATTRIB0_NV         0x86C6
#define GL_EVAL_VERTEX_ATTRIB1_NV         0x86C7
#define GL_EVAL_VERTEX_ATTRIB2_NV         0x86C8
#define GL_EVAL_VERTEX_ATTRIB3_NV         0x86C9
#define GL_EVAL_VERTEX_ATTRIB4_NV         0x86CA
#define GL_EVAL_VERTEX_ATTRIB5_NV         0x86CB
#define GL_EVAL_VERTEX_ATTRIB6_NV         0x86CC
#define GL_EVAL_VERTEX_ATTRIB7_NV         0x86CD
#define GL_EVAL_VERTEX_ATTRIB8_NV         0x86CE
#define GL_EVAL_VERTEX_ATTRIB9_NV         0x86CF
#define GL_EVAL_VERTEX_ATTRIB10_NV        0x86D0
#define GL_EVAL_VERTEX_ATTRIB11_NV        0x86D1
#define GL_EVAL_VERTEX_ATTRIB12_NV        0x86D2
#define GL_EVAL_VERTEX_ATTRIB13_NV        0x86D3
#define GL_EVAL_VERTEX_ATTRIB14_NV        0x86D4
#define GL_EVAL_VERTEX_ATTRIB15_NV        0x86D5
#define GL_MAX_MAP_TESSELLATION_NV        0x86D6
#define GL_MAX_RATIONAL_EVAL_ORDER_NV     0x86D7

// NV_fence
#define GL_ALL_COMPLETED_NV					0x84F2
#define GL_FENCE_STATUS_NV					0x84F3
#define GL_FENCE_CONDITION_NV				0x84F4

// NV_fog_distance
#define	GL_FOG_DISTANCE_MODE_NV				0x855a
#define	GL_EYE_RADIAL_NV					0x855b
#define	GL_EYE_PLANE_ABSOLUTE_NV			0x855c

// NV_light_max_exponent
#define GL_MAX_SHININESS_NV					0x8504
#define GL_MAX_SPOT_EXPONENT_NV				0x8505

// NV_packed_depth_stencil
#define GL_DEPTH_STENCIL_NV					0x84F9
#define GL_UNSIGNED_INT_24_8_NV				0x84FA

// NV_register_combiners 
#define	GL_REGISTER_COMBINERS_NV			0x8522
#define	GL_COMBINER0_NV						0x8550
#define	GL_COMBINER1_NV						0x8551
#define	GL_COMBINER2_NV						0x8552
#define	GL_COMBINER3_NV						0x8553
#define	GL_COMBINER4_NV						0x8554
#define	GL_COMBINER5_NV						0x8555
#define	GL_COMBINER6_NV						0x8556
#define	GL_COMBINER7_NV						0x8557
#define	GL_VARIABLE_A_NV					0x8523
#define	GL_VARIABLE_B_NV					0x8524
#define	GL_VARIABLE_C_NV					0x8525
#define	GL_VARIABLE_D_NV					0x8526
#define	GL_VARIABLE_E_NV					0x8527
#define	GL_VARIABLE_F_NV					0x8528
#define	GL_VARIABLE_G_NV					0x8529
#define	GL_CONSTANT_COLOR0_NV				0x852a
#define	GL_CONSTANT_COLOR1_NV				0x852b
#define	GL_PRIMARY_COLOR_NV					0x852c
#define	GL_SECONDARY_COLOR_NV				0x852d
#define	GL_SPARE0_NV						0x852e
#define	GL_SPARE1_NV						0x852f
#define	GL_UNSIGNED_IDENTITY_NV				0x8536
#define	GL_UNSIGNED_INVERT_NV				0x8537
#define	GL_EXPAND_NORMAL_NV					0x8538
#define	GL_EXPAND_NEGATE_NV					0x8539
#define	GL_HALF_BIAS_NORMAL_NV				0x853a
#define	GL_HALF_BIAS_NEGATE_NV				0x853b
#define	GL_SIGNED_IDENTITY_NV				0x853c
#define	GL_SIGNED_NEGATE_NV					0x853d
#define	GL_E_TIMES_F_NV						0x8531
#define	GL_SPARE0_PLUS_SECONDARY_COLOR_NV	0x8532
#define	GL_SCALE_BY_TWO_NV					0x853e
#define	GL_SCALE_BY_FOUR_NV					0x853f
#define	GL_SCALE_BY_ONE_HALF_NV				0x8540
#define	GL_BIAS_BY_NEGATIVE_ONE_HALF_NV		0x8541
#define	GL_DISCARD_NV						0x8530
#define	GL_COMBINER_INPUT_NV				0x8542
#define	GL_COMBINER_MAPPING_NV				0x8543
#define	GL_COMBINER_COMPONENT_USAGE_NV		0x8544
#define	GL_COMBINER_AB_DOT_PRODUCT_NV		0x8545
#define	GL_COMBINER_CD_DOT_PRODUCT_NV		0x8546
#define	GL_COMBINER_MUX_SUM_NV				0x8547
#define	GL_COMBINER_SCALE_NV				0x8548
#define	GL_COMBINER_BIAS_NV					0x8549
#define	GL_COMBINER_AB_OUTPUT_NV			0x854a
#define	GL_COMBINER_CD_OUTPUT_NV			0x854b
#define	GL_COMBINER_SUM_OUTPUT_NV			0x854c
#define	GL_NUM_GENERAL_COMBINERS_NV			0x854e
#define	GL_COLOR_SUM_CLAMP_NV				0x854f
#define	GL_MAX_GENERAL_COMBINERS_NV			0x854d

// NV_register_combiners2
#define GL_PER_STAGE_CONSTANTS_NV			0x8535

// NV_texgen_emboss 
#define	GL_EMBOSS_MAP_NV					0x855f
#define	GL_EMBOSS_LIGHT_NV					0x855d
#define	GL_EMBOSS_CONSTANT_NV				0x855e

// NV_texgen_reflection 
#define GL_NORMAL_MAP_NV                    0x8511
#define GL_REFLECTION_MAP_NV                0x8512

// NV_texture_env_combine4 
#define GL_COMBINE4_NV                      0x8503
#define GL_SOURCE3_RGB_NV                   0x8583
#define GL_SOURCE3_ALPHA_NV                 0x858B
#define GL_OPERAND3_RGB_NV                  0x8593
#define GL_OPERAND3_ALPHA_NV                0x859B

// NV_texture_rectangle
#define GL_TEXTURE_RECTANGLE_NV           0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_NV   0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_NV     0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_NV  0x84F8

// NV_texture_shader & NV_texture_shader2
#define GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV 0x86D9
#define GL_UNSIGNED_INT_S8_S8_8_8_NV      0x86DA
#define GL_UNSIGNED_INT_S8_S8_8_8_REV_NV  0x86DB
#define GL_DSDT_MAG_INTENSITY_NV          0x86DC
#define GL_SHADER_CONSISTENT_NV           0x86DD
#define GL_TEXTURE_SHADER_NV              0x86DE
#define GL_SHADER_OPERATION_NV            0x86DF
#define GL_CULL_MODES_NV                  0x86E0
#define GL_OFFSET_TEXTURE_2D_MATRIX_NV    0x86E1
#define GL_OFFSET_TEXTURE_2D_SCALE_NV     0x86E2
#define GL_OFFSET_TEXTURE_2D_BIAS_NV      0x86E3
#define GL_PREVIOUS_TEXTURE_INPUT_NV      0x86E4
#define GL_CONST_EYE_NV                   0x86E5
#define GL_PASS_THROUGH_NV                0x86E6
#define GL_CULL_FRAGMENT_NV               0x86E7
#define GL_OFFSET_TEXTURE_2D_NV           0x86E8
#define GL_DEPENDENT_AR_TEXTURE_2D_NV     0x86E9
#define GL_DEPENDENT_GB_TEXTURE_2D_NV     0x86EA
#define GL_DOT_PRODUCT_NV                 0x86EC
#define GL_DOT_PRODUCT_DEPTH_REPLACE_NV   0x86ED
#define GL_DOT_PRODUCT_TEXTURE_2D_NV      0x86EE
#define GL_DOT_PRODUCT_TEXTURE_3D_NV      0x86EF
#define GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV 0x86F0
#define GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV 0x86F1
#define GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV 0x86F2
#define GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV 0x86F3
#define GL_HILO_NV                        0x86F4
#define GL_DSDT_NV                        0x86F5
#define GL_DSDT_MAG_NV                    0x86F6
#define GL_DSDT_MAG_VIB_NV                0x86F7
#define GL_HILO16_NV                      0x86F8
#define GL_SIGNED_HILO_NV                 0x86F9
#define GL_SIGNED_HILO16_NV               0x86FA
#define GL_SIGNED_RGBA_NV                 0x86FB
#define GL_SIGNED_RGBA8_NV                0x86FC
#define GL_SIGNED_RGB_NV                  0x86FE
#define GL_SIGNED_RGB8_NV                 0x86FF
#define GL_SIGNED_LUMINANCE_NV            0x8701
#define GL_SIGNED_LUMINANCE8_NV           0x8702
#define GL_SIGNED_LUMINANCE_ALPHA_NV      0x8703
#define GL_SIGNED_LUMINANCE8_ALPHA8_NV    0x8704
#define GL_SIGNED_ALPHA_NV                0x8705
#define GL_SIGNED_ALPHA8_NV               0x8706
#define GL_SIGNED_INTENSITY_NV            0x8707
#define GL_SIGNED_INTENSITY8_NV           0x8708
#define GL_DSDT8_NV                       0x8709
#define GL_DSDT8_MAG8_NV                  0x870A
#define GL_DSDT8_MAG8_INTENSITY8_NV       0x870B
#define GL_SIGNED_RGB_UNSIGNED_ALPHA_NV   0x870C
#define GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV 0x870D
#define GL_HI_SCALE_NV                    0x870E
#define GL_LO_SCALE_NV                    0x870F
#define GL_DS_SCALE_NV                    0x8710
#define GL_DT_SCALE_NV                    0x8711
#define GL_MAGNITUDE_SCALE_NV             0x8712
#define GL_VIBRANCE_SCALE_NV              0x8713
#define GL_HI_BIAS_NV                     0x8714
#define GL_LO_BIAS_NV                     0x8715
#define GL_DS_BIAS_NV                     0x8716
#define GL_DT_BIAS_NV                     0x8717
#define GL_MAGNITUDE_BIAS_NV              0x8718
#define GL_VIBRANCE_BIAS_NV               0x8719
#define GL_TEXTURE_BORDER_VALUES_NV       0x871A
#define GL_TEXTURE_HI_SIZE_NV             0x871B
#define GL_TEXTURE_LO_SIZE_NV             0x871C
#define GL_TEXTURE_DS_SIZE_NV             0x871D
#define GL_TEXTURE_DT_SIZE_NV             0x871E
#define GL_TEXTURE_MAG_SIZE_NV            0x871F

// NV_vertex_array_range 
#define	GL_VERTEX_ARRAY_RANGE_NV			0x851d
#define	GL_VERTEX_ARRAY_RANGE_LENGTH_NV		0x851e
#define	GL_VERTEX_ARRAY_RANGE_VALID_NV		0x851f
#define	GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
#define	GL_VERTEX_ARRAY_RANGE_POINTER_NV	0x8521

// NV_vertex_program
#define GL_VERTEX_PROGRAM_NV              0x8620
#define GL_VERTEX_STATE_PROGRAM_NV        0x8621
#define GL_ATTRIB_ARRAY_SIZE_NV           0x8623
#define GL_ATTRIB_ARRAY_STRIDE_NV         0x8624
#define GL_ATTRIB_ARRAY_TYPE_NV           0x8625
#define GL_CURRENT_ATTRIB_NV              0x8626
#define GL_PROGRAM_LENGTH_NV              0x8627
#define GL_PROGRAM_STRING_NV              0x8628
#define GL_MODELVIEW_PROJECTION_NV        0x8629
#define GL_IDENTITY_NV                    0x862A
#define GL_INVERSE_NV                     0x862B
#define GL_TRANSPOSE_NV                   0x862C
#define GL_INVERSE_TRANSPOSE_NV           0x862D
#define GL_MAX_TRACK_MATRIX_STACK_DEPTH_NV 0x862E
#define GL_MAX_TRACK_MATRICES_NV          0x862F
#define GL_MATRIX0_NV                     0x8630
#define GL_MATRIX1_NV                     0x8631
#define GL_MATRIX2_NV                     0x8632
#define GL_MATRIX3_NV                     0x8633
#define GL_MATRIX4_NV                     0x8634
#define GL_MATRIX5_NV                     0x8635
#define GL_MATRIX6_NV                     0x8636
#define GL_MATRIX7_NV                     0x8637
#define GL_CURRENT_MATRIX_STACK_DEPTH_NV  0x8640
#define GL_CURRENT_MATRIX_NV              0x8641
#define GL_VERTEX_PROGRAM_POINT_SIZE_NV   0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_NV     0x8643
#define GL_PROGRAM_PARAMETER_NV           0x8644
#define GL_ATTRIB_ARRAY_POINTER_NV        0x8645
#define GL_PROGRAM_TARGET_NV              0x8646
#define GL_PROGRAM_RESIDENT_NV            0x8647
#define GL_TRACK_MATRIX_NV                0x8648
#define GL_TRACK_MATRIX_TRANSFORM_NV      0x8649
#define GL_VERTEX_PROGRAM_BINDING_NV      0x864A
#define GL_PROGRAM_ERROR_POSITION_NV      0x864B
#define GL_VERTEX_ATTRIB_ARRAY0_NV        0x8650
#define GL_VERTEX_ATTRIB_ARRAY1_NV        0x8651
#define GL_VERTEX_ATTRIB_ARRAY2_NV        0x8652
#define GL_VERTEX_ATTRIB_ARRAY3_NV        0x8653
#define GL_VERTEX_ATTRIB_ARRAY4_NV        0x8654
#define GL_VERTEX_ATTRIB_ARRAY5_NV        0x8655
#define GL_VERTEX_ATTRIB_ARRAY6_NV        0x8656
#define GL_VERTEX_ATTRIB_ARRAY7_NV        0x8657
#define GL_VERTEX_ATTRIB_ARRAY8_NV        0x8658
#define GL_VERTEX_ATTRIB_ARRAY9_NV        0x8659
#define GL_VERTEX_ATTRIB_ARRAY10_NV       0x865A
#define GL_VERTEX_ATTRIB_ARRAY11_NV       0x865B
#define GL_VERTEX_ATTRIB_ARRAY12_NV       0x865C
#define GL_VERTEX_ATTRIB_ARRAY13_NV       0x865D
#define GL_VERTEX_ATTRIB_ARRAY14_NV       0x865E
#define GL_VERTEX_ATTRIB_ARRAY15_NV       0x865F
#define GL_MAP1_VERTEX_ATTRIB0_4_NV       0x8660
#define GL_MAP1_VERTEX_ATTRIB1_4_NV       0x8661
#define GL_MAP1_VERTEX_ATTRIB2_4_NV       0x8662
#define GL_MAP1_VERTEX_ATTRIB3_4_NV       0x8663
#define GL_MAP1_VERTEX_ATTRIB4_4_NV       0x8664
#define GL_MAP1_VERTEX_ATTRIB5_4_NV       0x8665
#define GL_MAP1_VERTEX_ATTRIB6_4_NV       0x8666
#define GL_MAP1_VERTEX_ATTRIB7_4_NV       0x8667
#define GL_MAP1_VERTEX_ATTRIB8_4_NV       0x8668
#define GL_MAP1_VERTEX_ATTRIB9_4_NV       0x8669
#define GL_MAP1_VERTEX_ATTRIB10_4_NV      0x866A
#define GL_MAP1_VERTEX_ATTRIB11_4_NV      0x866B
#define GL_MAP1_VERTEX_ATTRIB12_4_NV      0x866C
#define GL_MAP1_VERTEX_ATTRIB13_4_NV      0x866D
#define GL_MAP1_VERTEX_ATTRIB14_4_NV      0x866E
#define GL_MAP1_VERTEX_ATTRIB15_4_NV      0x866F
#define GL_MAP2_VERTEX_ATTRIB0_4_NV       0x8670
#define GL_MAP2_VERTEX_ATTRIB1_4_NV       0x8671
#define GL_MAP2_VERTEX_ATTRIB2_4_NV       0x8672
#define GL_MAP2_VERTEX_ATTRIB3_4_NV       0x8673
#define GL_MAP2_VERTEX_ATTRIB4_4_NV       0x8674
#define GL_MAP2_VERTEX_ATTRIB5_4_NV       0x8675
#define GL_MAP2_VERTEX_ATTRIB6_4_NV       0x8676
#define GL_MAP2_VERTEX_ATTRIB7_4_NV       0x8677
#define GL_MAP2_VERTEX_ATTRIB8_4_NV       0x8678
#define GL_MAP2_VERTEX_ATTRIB9_4_NV       0x8679
#define GL_MAP2_VERTEX_ATTRIB10_4_NV      0x867A
#define GL_MAP2_VERTEX_ATTRIB11_4_NV      0x867B
#define GL_MAP2_VERTEX_ATTRIB12_4_NV      0x867C
#define GL_MAP2_VERTEX_ATTRIB13_4_NV      0x867D
#define GL_MAP2_VERTEX_ATTRIB14_4_NV      0x867E
#define GL_MAP2_VERTEX_ATTRIB15_4_NV      0x867F


///////////////////////////// SGI(S) ///////////////////////////////////

// SGI_color_matrix
#define GL_COLOR_MATRIX_SGI					0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH_SGI		0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI	0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE_SGI	0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI 0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI 0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI 0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS_SGI	0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI 0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI	0x80BA
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI 0x80BB

// SGI_color_table
#define GL_COLOR_TABLE_SGI					0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE_SGI 0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI 0x80D2
#define GL_PROXY_COLOR_TABLE_SGI			0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI 0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI 0x80D5
#define GL_COLOR_TABLE_SCALE_SGI			0x80D6
#define GL_COLOR_TABLE_BIAS_SGI				0x80D7
#define GL_COLOR_TABLE_FORMAT_SGI			0x80D8
#define GL_COLOR_TABLE_WIDTH_SGI			0x80D9
#define GL_COLOR_TABLE_RED_SIZE_SGI			0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_SGI		0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_SGI		0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_SGI		0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_SGI	0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_SGI	0x80DF

// SGIS_detail_texture
#define GL_DETAIL_TEXTURE_2D_SGIS			0x8095
#define GL_DETAIL_TEXTURE_2D_BINDING_SGIS	0x8096
#define GL_LINEAR_DETAIL_SGIS				0x8097
#define GL_LINEAR_DETAIL_ALPHA_SGIS			0x8098
#define GL_LINEAR_DETAIL_COLOR_SGIS			0x8099
#define GL_DETAIL_TEXTURE_LEVEL_SGIS		0x809A
#define GL_DETAIL_TEXTURE_MODE_SGIS			0x809B
#define GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS	0x809C

// SGIS_fog_function
#define GL_FOG_FUNC_SGIS					0x812A
#define GL_FOG_FUNC_POINTS_SGIS				0x812B
#define GL_MAX_FOG_FUNC_POINTS_SGIS			0x812C

// SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP_SGIS				0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS		0x8192

// SGIS_multisample
#define GL_MULTISAMPLE_SGIS					0x809D
#define GL_SAMPLE_ALPHA_TO_MASK_SGIS		0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_SGIS			0x809F
#define GL_SAMPLE_MASK_SGIS					0x80A0
#define GL_1PASS_SGIS						0x80A1
#define GL_2PASS_0_SGIS						0x80A2
#define GL_2PASS_1_SGIS						0x80A3
#define GL_4PASS_0_SGIS						0x80A4
#define GL_4PASS_1_SGIS						0x80A5
#define GL_4PASS_2_SGIS						0x80A6
#define GL_4PASS_3_SGIS						0x80A7
#define GL_SAMPLE_BUFFERS_SGIS				0x80A8
#define GL_SAMPLES_SGIS						0x80A9
#define GL_SAMPLE_MASK_VALUE_SGIS			0x80AA
#define GL_SAMPLE_MASK_INVERT_SGIS			0x80AB
#define GL_SAMPLE_PATTERN_SGIS				0x80AC

// SGIS_multitexture
#define TEXTURE0_SGIS                       0x835E
#define TEXTURE1_SGIS                       0x835F
#define GL_SELECTED_TEXTURE_SGIS            0x83C0
#define GL_SELECTED_TEXTURE_COORD_SET_SGIS  0x83C1
#define GL_SELECTED_TEXTURE_TRANSFORM_SGIS  0x83C2
#define GL_MAX_TEXTURES_SGIS                0x83C3
#define GL_MAX_TEXTURE_COORD_SETS_SGIS      0x83C4
#define GL_TEXTURE_ENV_COORD_SET_SGIS       0x83C5
#define GL_TEXTURE0_SGIS                    0x83C6
#define GL_TEXTURE1_SGIS                    0x83C7
#define GL_TEXTURE2_SGIS                    0x83C8
#define GL_TEXTURE3_SGIS                    0x83C9
#define GL_TEXTURE4_SGIS                    0x83CA
#define GL_TEXTURE5_SGIS                    0x83CB
#define GL_TEXTURE6_SGIS                    0x83CC
#define GL_TEXTURE7_SGIS                    0x83CD
#define GL_TEXTURE8_SGIS                    0x83CE
#define GL_TEXTURE9_SGIS                    0x83CF
#define GL_TEXTURE10_SGIS                   0x83D0
#define GL_TEXTURE11_SGIS                   0x83D1
#define GL_TEXTURE12_SGIS                   0x83D2
#define GL_TEXTURE13_SGIS                   0x83D3
#define GL_TEXTURE14_SGIS                   0x83D4
#define GL_TEXTURE15_SGIS                   0x83D5
#define GL_TEXTURE16_SGIS                   0x83D6
#define GL_TEXTURE17_SGIS                   0x83D7
#define GL_TEXTURE18_SGIS                   0x83D8
#define GL_TEXTURE19_SGIS                   0x83D9
#define GL_TEXTURE20_SGIS                   0x83DA
#define GL_TEXTURE21_SGIS                   0x83DB
#define GL_TEXTURE22_SGIS                   0x83DC
#define GL_TEXTURE23_SGIS                   0x83DD
#define GL_TEXTURE24_SGIS                   0x83DE
#define GL_TEXTURE25_SGIS                   0x83DF
#define GL_TEXTURE26_SGIS                   0x83E0
#define GL_TEXTURE27_SGIS                   0x83E1
#define GL_TEXTURE28_SGIS                   0x83E2
#define GL_TEXTURE29_SGIS                   0x83E3
#define GL_TEXTURE30_SGIS                   0x83E4
#define GL_TEXTURE31_SGIS                   0x83E5

// SGIS_pixel_texture
#define GL_PIXEL_TEXTURE_SGIS				0x8353
#define GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS	0x8354
#define GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS 0x8355
#define GL_PIXEL_GROUP_COLOR_SGIS			0x8356

// SGIS_point_line_texgen
#define GL_EYE_DISTANCE_TO_POINT_SGIS		0x81F0
#define GL_OBJECT_DISTANCE_TO_POINT_SGIS	0x81F1
#define GL_EYE_DISTANCE_TO_LINE_SGIS		0x81F2
#define GL_OBJECT_DISTANCE_TO_LINE_SGIS		0x81F3
#define GL_EYE_POINT_SGIS					0x81F4
#define GL_OBJECT_POINT_SGIS				0x81F5
#define GL_EYE_LINE_SGIS					0x81F6
#define GL_OBJECT_LINE_SGIS					0x81F7

// SGIS_point_parameters
#define GL_POINT_SIZE_MIN_EXT				0x8126
#define GL_POINT_SIZE_MIN_SGIS				0x8126
#define GL_POINT_SIZE_MAX_EXT				0x8127
#define GL_POINT_SIZE_MAX_SGIS				0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT	0x8128
#define GL_POINT_FADE_THRESHOLD_SIZE_SGIS	0x8128
#define GL_DISTANCE_ATTENUATION_EXT			0x8129
#define GL_DISTANCE_ATTENUATION_SGIS		0x8129

// SGIS_sharpen_texture
#define GL_LINEAR_SHARPEN_SGIS				0x80AD
#define GL_LINEAR_SHARPEN_ALPHA_SGIS		0x80AE
#define GL_LINEAR_SHARPEN_COLOR_SGIS		0x80AF
#define GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS 0x80B0

// SGI_texture_edge_clamp
#define GL_CLAMP_TO_EDGE_SGI                0x812F

// SGIS_texture4D
#define GL_PACK_SKIP_VOLUMES_SGIS			0x8130
#define GL_PACK_IMAGE_DEPTH_SGIS			0x8131
#define GL_UNPACK_SKIP_VOLUMES_SGIS			0x8132
#define GL_UNPACK_IMAGE_DEPTH_SGIS			0x8133
#define GL_TEXTURE_4D_SGIS					0x8134
#define GL_PROXY_TEXTURE_4D_SGIS			0x8135
#define GL_TEXTURE_4DSIZE_SGIS				0x8136
#define GL_TEXTURE_WRAP_Q_SGIS				0x8137
#define GL_MAX_4D_TEXTURE_SIZE_SGIS			0x8138
#define GL_TEXTURE_4D_BINDING_SGIS			0x814F

// SGIS_texture_color_mask
#define GL_TEXTURE_COLOR_WRITEMASK_SGIS   0x81EF

// SGI_texture_color_table
#define GL_TEXTURE_COLOR_TABLE_SGI			0x80BC
#define GL_PROXY_TEXTURE_COLOR_TABLE_SGI	0x80BD

// SGIS_texture_lod 
#define	GL_TEXTURE_MIN_LOD_SGIS				0x813A
#define	GL_TEXTURE_MAX_LOD_SGIS				0x813B
#define	GL_TEXTURE_BASE_LEVEL_SGIS			0x813C
#define	GL_TEXTURE_MAX_LEVEL_SGIS			0x813D

// SGIS_texture_filter4
#define GL_FILTER4_SGIS						0x8146
#define GL_TEXTURE_FILTER4_SIZE_SGIS		0x8147

// SGIS_texture_select
#define GL_DUAL_ALPHA4_SGIS					0x8110
#define GL_DUAL_ALPHA8_SGIS					0x8111
#define GL_DUAL_ALPHA12_SGIS				0x8112
#define GL_DUAL_ALPHA16_SGIS				0x8113
#define GL_DUAL_LUMINANCE4_SGIS				0x8114
#define GL_DUAL_LUMINANCE8_SGIS				0x8115
#define GL_DUAL_LUMINANCE12_SGIS			0x8116
#define GL_DUAL_LUMINANCE16_SGIS			0x8117
#define GL_DUAL_INTENSITY4_SGIS				0x8118
#define GL_DUAL_INTENSITY8_SGIS				0x8119
#define GL_DUAL_INTENSITY12_SGIS			0x811A
#define GL_DUAL_INTENSITY16_SGIS			0x811B
#define GL_DUAL_LUMINANCE_ALPHA4_SGIS		0x811C
#define GL_DUAL_LUMINANCE_ALPHA8_SGIS		0x811D
#define GL_QUAD_ALPHA4_SGIS					0x811E
#define GL_QUAD_ALPHA8_SGIS					0x811F
#define GL_QUAD_LUMINANCE4_SGIS				0x8120
#define GL_QUAD_LUMINANCE8_SGIS				0x8121
#define GL_QUAD_INTENSITY4_SGIS				0x8122
#define GL_QUAD_INTENSITY8_SGIS				0x8123
#define GL_DUAL_TEXTURE_SELECT_SGIS			0x8124
#define GL_QUAD_TEXTURE_SELECT_SGIS			0x8125


///////////////////////////// S3 ///////////////////////////////////

// S3_s3tc
#define GL_RGB_S3TC                         0x83A0
#define GL_RGB4_S3TC                        0x83A1
#define GL_RGBA_S3TC                        0x83A2
#define GL_RGBA4_S3TC                       0x83A3


///////////////////////////// WIN/WGL ///////////////////////////////////

// WGL_ARB_buffer_region
#define WGL_FRONT_COLOR_BUFFER_BIT_ARB		0x00000001
#define WGL_BACK_COLOR_BUFFER_BIT_ARB		0x00000002
#define WGL_DEPTH_BUFFER_BIT_ARB			0x00000004
#define WGL_STENCIL_BUFFER_BIT_ARB			0x00000008

// WGL_ARB_pbuffer
#define WGL_DRAW_TO_PBUFFER_ARB             0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB          0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB           0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB          0x2030
#define WGL_PBUFFER_LARGEST_ARB             0x2033
#define WGL_PBUFFER_WIDTH_ARB               0x2034
#define WGL_PBUFFER_HEIGHT_ARB              0x2035
#define WGL_PBUFFER_LOST_ARB				0x2036

// WGL_ARB_pixel_format
#define WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
#define WGL_DRAW_TO_WINDOW_ARB				0x2001
#define WGL_DRAW_TO_BITMAP_ARB				0x2002
#define WGL_ACCELERATION_ARB				0x2003
#define WGL_NEED_PALETTE_ARB				0x2004
#define	WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
#define	WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
#define	WGL_SWAP_METHOD_ARB					0x2007
#define	WGL_NUMBER_OVERLAYS_ARB				0x2008
#define	WGL_NUMBER_UNDERLAYS_ARB			0x2009
#define WGL_TRANSPARENT_ARB					0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
#define WGL_SHARE_DEPTH_ARB					0x200C
#define WGL_SHARE_STENCIL_ARB				0x200D
#define WGL_SHARE_ACCUM_ARB					0x200E
#define WGL_SUPPORT_GDI_ARB					0x200F
#define WGL_SUPPORT_OPENGL_ARB				0x2010
#define WGL_DOUBLE_BUFFER_ARB				0x2011
#define WGL_STEREO_ARB						0x2012
#define WGL_PIXEL_TYPE_ARB					0x2013
#define WGL_COLOR_BITS_ARB					0x2014
#define WGL_RED_BITS_ARB					0x2015
#define WGL_RED_SHIFT_ARB					0x2016
#define WGL_GREEN_BITS_ARB					0x2017
#define WGL_GREEN_SHIFT_ARB					0x2018
#define WGL_BLUE_BITS_ARB					0x2019
#define WGL_BLUE_SHIFT_ARB					0x201A
#define WGL_ALPHA_BITS_ARB					0x201B
#define WGL_ALPHA_SHIFT_ARB					0x201C
#define WGL_ACCUM_BITS_ARB					0x201D
#define WGL_ACCUM_RED_BITS_ARB				0x201E
#define WGL_ACCUM_GREEN_BITS_ARB			0x201F
#define WGL_ACCUM_BLUE_BITS_ARB				0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB			0x2021
#define WGL_DEPTH_BITS_ARB					0x2022
#define WGL_STENCIL_BITS_ARB				0x2023
#define WGL_AUX_BUFFERS_ARB					0x2024
#define WGL_NO_ACCELERATION_ARB				0x2025
#define WGL_GENERIC_ACCELERATION_ARB		0x2026
#define WGL_FULL_ACCELERATION_ARB			0x2027
#define WGL_SWAP_EXCHANGE_ARB				0x2028
#define WGL_SWAP_COPY_ARB					0x2029
#define WGL_SWAP_UNDEFINED_ARB				0x202A
#define WGL_TYPE_RGBA_ARB					0x202B
#define WGL_TYPE_COLORINDEX_ARB				0x202C

// WIN_phong_shading
#define GL_PHONG_WIN						0x80EA
#define GL_PHONG_HINT_WIN					0x80EB

// WIN_specular_fog
#define GL_FOG_SPECULAR_TEXTURE_WIN			0x80EC

////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//                          
//						FUNCION DEFINITIONS
//
////////////////////////////////////////////////////////////////////////

///////////////////////////// 3DFX ///////////////////////////////////

// 3DFX_tbuffer
typedef void (APIENTRY * PFNGLTBUFFERMASK3DFXPROC) (GLuint mask);

///////////////////////////// ARB ///////////////////////////////////

// ARB_multisample
typedef void (APIENTRY * PFNGLSAMPLECOVERAGEARBPROC) (GLclampf value, GLboolean invert);
typedef void (APIENTRY * PFNGLSAMPLEPASSARBPROC) (GLenum pass);

// ARB_multitexture 
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum target, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum target, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum target, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum target, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum target, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum target);

// ARB_texture_compression
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (GLenum target, GLint level, void *img);

// ARB_transpose_matrix
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFARBPROC) (const GLfloat *m);
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDARBPROC) (const GLdouble *m);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFARBPROC) (const GLfloat *m);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDARBPROC) (const GLdouble *m);

// ARB_vertex_blend
typedef void (APIENTRY *PFNGLWEIGHTBVARBPROC)(GLint size, GLbyte *weights);
typedef void (APIENTRY *PFNGLWEIGHTSVARBPROC)(GLint size, GLshort *weights);
typedef void (APIENTRY *PFNGLWEIGHTIVARBPROC)(GLint size, GLint *weights);
typedef void (APIENTRY *PFNGLWEIGHTFVARBPROC)(GLint size, GLfloat *weights);
typedef void (APIENTRY *PFNGLWEIGHTDVARBPROC)(GLint size, GLdouble *weights);
typedef void (APIENTRY *PFNGLWEIGHTUBVARBPROC)(GLint size, GLubyte *weights);
typedef void (APIENTRY *PFNGLWEIGHTUSVARBPROC)(GLint size, GLushort *weights);
typedef void (APIENTRY *PFNGLWEIGHTUIVARBPROC)(GLint size, GLuint *weights);
typedef void (APIENTRY *PFNGLWEIGHTPOINTERARBPROC)(GLint size, GLenum type, GLsizei stride, GLvoid *pointer);
typedef void (APIENTRY *PFNGLVERTEXBLENDARBPROC)(GLint count);

///////////////////////////// ATI(X) ///////////////////////////////////

// ATIX_envmap_bumpmap
typedef void (APIENTRY * PFNGLTEXBUMPPARAMETERIVATIXPROC) (GLenum pname, GLint *param);
typedef void (APIENTRY * PFNGLTEXBUMPPARAMETERFVATIXPROC) (GLenum pname, GLfloat *param);
typedef void (APIENTRY * PFNGLGETTEXBUMPPARAMETERIVATIXPROC) (GLenum pname, GLint *param);
typedef void (APIENTRY * PFNGLGETTEXBUMPPARAMETERFVATIXPROC) (GLenum pname, GLfloat *param);

// ATIX_pn_triangles
typedef void (APIENTRY *PFNGLPNTRIANGLESIATIXPROC)(GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLPNTRIANGLESFATIXPROC)(GLenum pname, GLfloat param);

// ATI_vertex_blend
typedef void (APIENTRY * PFNGLACTIVEVERTEXUNITATIPROC) (GLenum unit);
typedef void (APIENTRY * PFNGLCLIENTACTIVEVERTEXUNITATIPROC) (GLenum unit);
typedef void (APIENTRY * PFNGLWEIGHTPOINTERATIPROC) (GLenum type, GLsizei stride, void *pointer);
typedef void (APIENTRY * PFNGLWEIGHTFATIPROC) (GLenum unit, GLfloat weight);
typedef void (APIENTRY * PFNGLWEIGHTDATIPROC) (GLenum unit, GLdouble weight);
typedef void (APIENTRY * PFNGLWEIGHTFVATIPROC) (GLenum unit, const GLfloat *v);
typedef void (APIENTRY * PFNGLWEIGHTDVATIPROC) (GLenum unit, const GLdouble *v);

// ATI_vertex_streams
typedef void (APIENTRY * PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC) (GLenum stream);
typedef void (APIENTRY * PFNGLVERTEXBLENDENVIATIPROC) (GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLVERTEXBLENDENVFATIPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2SATIPROC) (GLenum stream, GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2IATIPROC) (GLenum stream, GLint x, GLint y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2FATIPROC) (GLenum stream, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2DATIPROC) (GLenum stream, GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2DVATIPROC) (GLenum stream, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4IATIPROC) (GLenum stream, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4DVATIPROC) (GLenum stream, const GLdouble *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3BATIPROC) (GLenum stream, GLbyte x, GLbyte y, GLbyte z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3BVATIPROC) (GLenum stream, const GLbyte *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);


///////////////////////////// EXT ///////////////////////////////////

// EXT_blend_color 
typedef void (APIENTRY * PFNGLBLENDCOLOREXTPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

// EXT_blend_func_separate
typedef void (APIENTRY * PFNGLBLENDFUNCSEPARATEEXTPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

// EXT_blend_minmax 
typedef void (APIENTRY * PFNGLBLENDEQUATIONEXTPROC) (GLenum mode);

// EXT_compiled_vertex_array
typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

// EXT_copy_texture
typedef void (APIENTRY * PFNGLCOPYTEXIMAGE1DEXTPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRY * PFNGLCOPYTEXIMAGE2DEXTPROC) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE1DEXTPROC) (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE2DEXTPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE3DEXTPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

// EXT_convolution
typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER1DEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER2DEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFEXTPROC) (GLenum target, GLenum pname, GLfloat params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFVEXTPROC) (GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIEXTPROC) (GLenum target, GLenum pname, GLint params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIVEXTPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONFILTEREXTPROC) (GLenum target, GLenum format, GLenum type, GLvoid *image);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETSEPARABLEFILTEREXTPROC) (GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
typedef void (APIENTRY * PFNGLSEPARABLEFILTER2DEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);

// EXT_coordinate_frame
typedef void (APIENTRY * PFNGLTANGENT3BEXTPROC) (GLbyte tx, GLbyte ty, GLbyte tz);
typedef void (APIENTRY * PFNGLTANGENT3BVEXTPROC) (const GLbyte *v);
typedef void (APIENTRY * PFNGLTANGENT3DEXTPROC) (GLdouble tx, GLdouble ty, GLdouble tz);
typedef void (APIENTRY * PFNGLTANGENT3DVEXTPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLTANGENT3FEXTPROC) (GLfloat tx, GLfloat ty, GLfloat tz);
typedef void (APIENTRY * PFNGLTANGENT3FVEXTPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLTANGENT3IEXTPROC) (GLint tx, GLint ty, GLint tz);
typedef void (APIENTRY * PFNGLTANGENT3IVEXTPROC) (const GLint *v);
typedef void (APIENTRY * PFNGLTANGENT3SEXTPROC) (GLshort tx, GLshort ty, GLshort tz);
typedef void (APIENTRY * PFNGLTANGENT3SVEXTPROC) (const GLshort *v);
typedef void (APIENTRY * PFNGLBINORMAL3BEXTPROC) (GLbyte bx, GLbyte by, GLbyte bz);
typedef void (APIENTRY * PFNGLBINORMAL3BVEXTPROC) (const GLbyte *v);
typedef void (APIENTRY * PFNGLBINORMAL3DEXTPROC) (GLdouble bx, GLdouble by, GLdouble bz);
typedef void (APIENTRY * PFNGLBINORMAL3DVEXTPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLBINORMAL3FEXTPROC) (GLfloat bx, GLfloat by, GLfloat bz);
typedef void (APIENTRY * PFNGLBINORMAL3FVEXTPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLBINORMAL3IEXTPROC) (GLint bx, GLint by, GLint bz);
typedef void (APIENTRY * PFNGLBINORMAL3IVEXTPROC) (const GLint *v);
typedef void (APIENTRY * PFNGLBINORMAL3SEXTPROC) (GLshort bx, GLshort by, GLshort bz);
typedef void (APIENTRY * PFNGLBINORMAL3SVEXTPROC) (const GLshort *v);
typedef void (APIENTRY * PFNGLTANGENTPOINTEREXTPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLBINORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);

// EXT_cull_vertex
typedef void (APIENTRY * PFNGLCULLPARAMETERDVEXTPROC) (GLenum pname, GLdouble* params);
typedef void (APIENTRY * PFNGLCULLPARAMETERFVEXTPROC) (GLenum pname, GLfloat* params);

// EXT_draw_range_elements
typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTSEXTPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);

// EXT_fog_coord
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLFOGCOORDDEXTPROC) (GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (GLfloat *v);
typedef void (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (GLdouble *v);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, GLvoid *pointer);

// EXT_histogram
typedef void (APIENTRY * PFNGLGETHISTOGRAMEXTPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERFVEXTPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERIVEXTPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETMINMAXEXTPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERFVEXTPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERIVEXTPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLHISTOGRAMEXTPROC) (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
typedef void (APIENTRY * PFNGLMINMAXEXTPROC) (GLenum target, GLenum internalformat, GLboolean sink);
typedef void (APIENTRY * PFNGLRESETHISTOGRAMEXTPROC) (GLenum target);
typedef void (APIENTRY * PFNGLRESETMINMAXEXTPROC) (GLenum target);

// EXT_index_func
typedef void (APIENTRY * PFNGLINDEXFUNCEXTPROC) (GLenum func, GLclampf ref);

// EXT_index_material
typedef void (APIENTRY * PFNGLINDEXMATERIALEXTPROC) (GLenum face, GLenum mode);

// EXT_light_texture
typedef void (APIENTRY * PFNGLAPPLYTEXTUREEXTPROC) (GLenum mode);
typedef void (APIENTRY * PFNGLTEXTURELIGHTEXTPROC) (GLenum pname);
typedef void (APIENTRY * PFNGLTEXTUREMATERIALEXTPROC) (GLenum face, GLenum mode);

// EXT_multi_draw_arrays
typedef void (APIENTRY * PFNGLMULTIDRAWARRAYSEXTPROC) (GLenum mode, GLint *first, GLsizei *count, GLsizei primcount);
typedef void (APIENTRY * PFNGLMULTIDRAWELEMENTSEXTPROC) (GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount);

// EXT_multisample
typedef void (APIENTRY * PFNGLSAMPLEMASKEXTPROC) (GLclampf value, GLboolean invert);
typedef void (APIENTRY * PFNGLSAMPLEPATTERNEXTPROC) (GLenum pattern);

// EXT_paletted_texture 
typedef void (APIENTRY * PFNGLCOLORTABLEEXTPROC) (GLenum target,GLenum internalFormat,GLsizei width,GLenum format,GLenum type,const GLvoid *data);
typedef void (APIENTRY * PFNGLCOLORSUBTABLEEXTPROCCOLORSUBTABLEEXT) (GLenum target,GLsizei start,GLsizei count,GLenum format,GLenum type,const GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOLORTABLEEXTPROC) (GLenum target,GLenum format,GLenum type,GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVEXTPROC) (GLenum target,GLenum pname,GLint *params);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVEXTPROC) (GLenum target,GLenum pname,GLfloat *params);

// EXT_pixel_transform
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERIEXTPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERFEXTPROC) (GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC) (GLenum target, GLenum pname, const GLfloat *params);

// EXT_point_parameters
typedef void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFSGISPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVSGISPROC) (GLenum pname, const GLfloat *params);

// EXT_polygon_offset
typedef void (APIENTRY * PFNGLPOLYGONOFFSETEXTPROC) (GLfloat factor, GLfloat bias);

// EXT_secondary_color
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BEXTPROC) (GLbyte red, GLbyte green, GLbyte blue);		// Normal formulation variants
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SEXTPROC) (GLshort red, GLshort green, GLshort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IEXTPROC) (GLint red, GLint green, GLint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FEXTPROC) (GLfloat red, GLfloat green, GLfloat blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DEXTPROC) (GLdouble red, GLdouble green, GLdouble blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBEXTPROC) (GLubyte red, GLubyte green, GLubyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USEXTPROC) (GLushort red, GLushort green, GLushort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIEXTPROC) (GLuint red, GLuint green, GLuint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BVEXTPROC) (const GLbyte *v);		// vector variants
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SVEXTPROC) (const GLshort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IVEXTPROC) (const GLint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FVEXTPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DVEXTPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBVEXTPROC) (const GLubyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USVEXTPROC) (const GLushort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIVEXTPROC) (const GLuint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLvoid *pointer);

// EXT_subtexture
typedef void (APIENTRY * PFNGLTEXSUBIMAGE1DEXTPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE2DEXTPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);

// EXT_texture3D
typedef void (APIENTRY * PFNGLTEXIMAGE3DEXTPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE3DEXTPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);

// EXT_texture_object (obsolete in OpenGL 1.1+)
typedef void (APIENTRY * PFNGLGENTEXTURESEXTPROC) (GLsizei n,GLuint* textures);
typedef void (APIENTRY * PFNGLDELETETEXTURESEXTPROC) (GLsizei n,const GLuint* textures);
typedef void (APIENTRY * PFNGLBINDTEXTUREEXTPROC) (GLenum target,GLuint texture);
typedef void (APIENTRY * PFNGLPRIORITIZETEXTURESEXTPROC) (GLsizei n,const GLuint* textures,const GLfloat* priorities);
typedef void (APIENTRY * PFNGLARETEXTURESRESIDENTEXTPROC) (GLsizei n,const GLuint* textures, GLboolean* residences);
typedef void (APIENTRY * PFNGLISTEXTUREEXTPROC) (GLuint texture);

// EXT_texture_perturb_normal
typedef void (APIENTRY * PFNGLTEXTURENORMALEXTPROC) (GLenum mode);

// EXT_vertex_array 
typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);

// EXT_vertex_weighting
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFEXTPROC) (GLfloat weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFVEXTPROC) (GLfloat *weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLvoid *pointer);


///////////////////////////// KTX ///////////////////////////////////

// KTX_buffer_region
typedef GLuint (APIENTRY * PFNGLBUFFERREGIONENABLEDEXTPROC) ( void );
typedef GLuint (APIENTRY * PFNGLNEWBUFFERREGIONEXTPROC) ( GLenum region );
typedef void (APIENTRY * PFNGLDELETEBUFFERREGIONEXTPROC) ( GLenum region );
typedef void (APIENTRY * PFNGLREADBUFFERREGIONEXTPROC) ( GLuint region, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY * PFNGLDRAWBUFFERREGIONEXTPROC) ( GLuint region, GLint x, GLint y, GLsizei width, GLsizei height, GLint xDest, GLint yDest);


///////////////////////////// NV ///////////////////////////////////

// NV_evaluators
typedef void (APIENTRY * PFNGLMAPCONTROLPOINTSNVPROC) (GLenum target, GLuint index, GLenum type, GLsizei ustride, GLsizei vstride, GLint uorder, GLint vorder, GLboolean packed, const GLvoid *points);
typedef void (APIENTRY * PFNGLMAPPARAMETERIVNVPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLMAPPARAMETERFVNVPROC) (GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLGETMAPCONTROLPOINTSNVPROC) (GLenum target, GLuint index, GLenum type, GLsizei ustride, GLsizei vstride, GLboolean packed, GLvoid *points);
typedef void (APIENTRY * PFNGLGETMAPPARAMETERIVNVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETMAPPARAMETERFVNVPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETMAPATTRIBPARAMETERIVNVPROC) (GLenum target, GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETMAPATTRIBPARAMETERFVNVPROC) (GLenum target, GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLEVALMAPSNVPROC) (GLenum target, GLenum mode);

// NV_fence
typedef void (APIENTRY * PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
typedef void (APIENTRY * PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
typedef GLboolean (APIENTRY * PFNGLISFENCENVPROC) (GLuint fence);
typedef GLboolean (APIENTRY * PFNGLTESTFENCENVPROC) (GLuint fence);
typedef void (APIENTRY * PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLFINISHFENCENVPROC) (GLuint fence);
typedef void (APIENTRY * PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);

// NV_register_combiners 
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFVNVPROC) (GLenum pname,const GLfloat *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERIVNVPROC) (GLenum pname,const GLint *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFNVPROC) (GLenum pname,GLfloat param);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERINVPROC) (GLenum pname,GLint param);
typedef void (APIENTRY * PFNGLCOMBINERINPUTNVPROC) (GLenum stage,GLenum portion,GLenum variable,GLenum input,GLenum mapping,GLenum componentUsage);
typedef void (APIENTRY * PFNGLCOMBINEROUTPUTNVPROC) (GLenum stage,GLenum portion,GLenum abOutput,GLenum cdOutput,GLenum sumOutput,GLenum scale,GLenum bias,GLboolean abDotProduct,GLboolean cdDotProduct,GLboolean muxSum);
typedef void (APIENTRY * PFNGLFINALCOMBINERINPUTNVPROC) (GLenum variable,GLenum input,GLenum mapping,GLenum componentUsage);
typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage,GLenum portion,GLenum variable,GLenum pname,const GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage,GLenum portion,GLenum variable,GLenum pname,const GLint *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (GLenum stage,GLenum portion,GLenum pname,const GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (GLenum stage,GLenum portion,GLenum pname,GLint *params);
typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (GLenum variable,GLenum pname,const GLfloat *params);
typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (GLenum variable,GLenum pname,const GLfloat *params);

// NV_register_combiners2
typedef void (APIENTRY * PFNGLCOMBINERSTAGEPARAMETERFVNVPROC) (GLenum stage, GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC) (GLenum stage, GLenum pname, GLfloat *params);

// NV_vertex_array_range 
typedef void (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC) (GLsizei length, GLvoid *pointer);
typedef void (APIENTRY * PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);

// NV_vertex_program
typedef GLboolean (APIENTRY * PFNGLAREPROGRAMSRESIDENTNVPROC) (GLsizei n, const GLuint *programs, GLboolean *residences);
typedef void (APIENTRY * PFNGLBINDPROGRAMNVPROC) (GLenum target, GLuint id);
typedef void (APIENTRY * PFNGLDELETEPROGRAMSNVPROC) (GLsizei n, const GLuint *programs);
typedef void (APIENTRY * PFNGLEXECUTEPROGRAMNVPROC) (GLenum target, GLuint id, const GLfloat *params);
typedef void (APIENTRY * PFNGLGENPROGRAMSNVPROC) (GLsizei n, GLuint *programs);
typedef void (APIENTRY * PFNGLGETPROGRAMPARAMETERDVNVPROC) (GLenum target, GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRY * PFNGLGETPROGRAMPARAMETERFVNVPROC) (GLenum target, GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMIVNVPROC) (GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETPROGRAMSTRINGNVPROC) (GLuint id, GLenum pname, GLubyte *program);
typedef void (APIENTRY * PFNGLGETTRACKMATRIXIVNVPROC) (GLenum target, GLuint address, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBDVNVPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBFVNVPROC) (GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBIVNVPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVNVPROC) (GLuint index, GLenum pname, GLvoid* *pointer);
typedef GLboolean (APIENTRY * PFNGLISPROGRAMNVPROC) (GLuint id);
typedef void (APIENTRY * PFNGLLOADPROGRAMNVPROC) (GLenum target, GLuint id, GLsizei len, const GLubyte *program);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4DNVPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4DVNVPROC) (GLenum target, GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4FNVPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4FVNVPROC) (GLenum target, GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETERS4DVNVPROC) (GLenum target, GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETERS4FVNVPROC) (GLenum target, GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRY * PFNGLREQUESTRESIDENTPROGRAMSNVPROC) (GLsizei n, const GLuint *programs);
typedef void (APIENTRY * PFNGLTRACKMATRIXNVPROC) (GLenum target, GLuint address, GLenum matrix, GLenum transform);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERNVPROC) (GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DNVPROC) (GLuint index, GLdouble x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DVNVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FNVPROC) (GLuint index, GLfloat x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FVNVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SNVPROC) (GLuint index, GLshort x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SVNVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DNVPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DVNVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FNVPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FVNVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SNVPROC) (GLuint index, GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SVNVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DNVPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DVNVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FNVPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FVNVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SNVPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SVNVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DNVPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DVNVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FNVPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FVNVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SNVPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SVNVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4UBVNVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS1DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS1FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS1SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS2DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS2FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS2SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS3DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS3FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS3SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS4DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS4FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS4SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBS4UBVNVPROC) (GLuint index, GLsizei count, const GLubyte *v);


///////////////////////////// SGIS ///////////////////////////////////

// SGIS_detail_texture
typedef void (APIENTRY * PFNGLDETAILTEXFUNCSGISPROC) (GLenum target, GLsizei n, const GLfloat *points);
typedef void (APIENTRY * PFNGLGETDETAILTEXFUNCSGISPROC) (GLenum target, GLfloat *points);

// SGI_color_table
typedef void (APIENTRY * PFNGLCOLORTABLESGIPROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERFVSGIPROC) (GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERIVSGIPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLCOPYCOLORTABLESGIPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
typedef void (APIENTRY * PFNGLGETCOLORTABLESGIPROC) (GLenum target, GLenum format, GLenum type, GLvoid *table);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVSGIPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVSGIPROC) (GLenum target, GLenum pname, GLint *params);

// SGIS_fog_function
typedef void (APIENTRY * PFNGLFOGFUNCSGISPROC) (GLsizei n, const GLfloat *points);
typedef void (APIENTRY * PFNGLGETFOGFUNCSGISPROC) (const GLfloat *points);

// SGIS_multisample
typedef void (APIENTRY * PFNGLSAMPLEMASKSGISPROC) (GLclampf value, GLboolean invert);
typedef void (APIENTRY * PFNGLSAMPLEPATTERNSGISPROC) (GLenum pattern);

// SGIS_multitexture
typedef void (APIENTRY * PFNGLSELECTTEXTURESGISPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLSELECTTEXTURETRANSFORMSGISPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLSELECTTEXTURECOORDSETSGISPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DSGISPROC) (GLenum texture, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FSGISPROC) (GLenum texture, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1ISGISPROC) (GLenum texture, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SSGISPROC) (GLenum texture, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVSGISPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DSGISPROC) (GLenum texture, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FSGISPROC) (GLenum texture, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2ISGISPROC) (GLenum texture, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SSGISPROC) (GLenum texture, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVSGISPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DSGISPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FSGISPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3ISGISPROC) (GLenum texture, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SSGISPROC) (GLenum texture, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVSGISPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DSGISPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FSGISPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4ISGISPROC) (GLenum texture, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SSGISPROC) (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVSGISPROC) (GLenum texture, const GLshort *v);

// SGIS_pixel_texture
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERISGISPROC) (GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERIVSGISPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERFSGISPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERFVSGISPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC) (GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC) (GLenum pname, GLfloat *params);

// SGIS_sharpen_texture
typedef void (APIENTRY * PFNGLSHARPENTEXFUNCSGISPROC) (GLenum target, GLsizei n, const GLfloat *points);
typedef void (APIENTRY * PFNGLGETSHARPENTEXFUNCSGISPROC) (GLenum target, GLfloat *points);

// SGIS_texture4D
typedef void (APIENTRY * PFNGLTEXIMAGE4DSGISPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLsizei size4d, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE4DSGISPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint woffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei size4d, GLenum format, GLenum type, const GLvoid *pixels);

// SGIS_texture_color_mask
typedef void (APIENTRY * PFNGLTEXTURECOLORMASKSGISPROC) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

// SGIS_texture_filter4
typedef void (APIENTRY * PFNGLGETTEXFILTERFUNCSGISPROC) (GLenum target, GLenum filter, GLfloat *weights);
typedef void (APIENTRY * PFNGLTEXFILTERFUNCSGISPROC) (GLenum target, GLenum filter, GLsizei n, const GLfloat *weights);


///////////////////////////// WIN/WGL ///////////////////////////////////

// WGL_ARB_buffer_region
typedef HANDLE (WINAPI * PFNWGLCREATEBUFFERREGIONARBPROC) (HDC hDC, int iLayerPlane, UINT uType);
typedef VOID (WINAPI * PFNWGLDELETEBUFFERREGIONARBPROC) (HANDLE hRegion);
typedef BOOL (WINAPI * PFNWGLSAVEBUFFERREGIONARBPROC) (HANDLE hRegion, int x, int y, int width, int height);
typedef BOOL (WINAPI * PFNWGLRESTOREBUFFERREGIONARBPROC) (HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);

// WGL_ARB_extensions_string
typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hDC);

// WGL_ARB_make_current_read
typedef BOOL (WINAPI * PFNWGLMAKECONTEXTCURRENTARBPROC) (HDC hDrawDC, HDC hReadDC, HGLRC hGLRC);
typedef HDC (WINAPI * PFNWGLGETCURRENTREADDCARBPROC) (VOID);

// WGL_ARB_pbuffer
DECLARE_HANDLE(HPBUFFERARB);
typedef HPBUFFERARB (WINAPI * PFNWGLCREATEPBUFFERARBPROC) (HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList);
typedef HDC (WINAPI * PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
typedef int (WINAPI * PFNWGLRELEASEPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer, HDC hDC);
typedef BOOL (WINAPI * PFNWGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
typedef BOOL (WINAPI * PFNWGLQUERYPBUFFERARBPROC) (HPBUFFERARB hPbuffer, int iAttribute, int *piValue);

// WGL_ARB_pixel_format
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hDC, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (HDC hDC, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hDC, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

// WGL_EXT_extensions_string
typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC) (VOID);

// WGL_EXT_swap_constrol
typedef void (APIENTRY * PFNGLWGLSWAPINTERVALEXTPROC) (GLint interval);
typedef void (APIENTRY * PFNGLWGLGETSWAPINTERVALEXTPROC) (void);

// WGL_NV_allocate_memory
typedef void * (APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq, float priority);
typedef void (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);

// WIN_swap_hint
typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

//////////////////////////////////////////////////////////////////////////

#endif	// DEMOGL_GLEXT_H
