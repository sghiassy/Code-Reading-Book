//////////////////////////////////////////////////////////////////////
// FILE: DemoGL_Extensions.h
// PURPOSE: include file for DemoGL powered applications that use IsExtensionAvailable() function of DemoGL.
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
// v1.3: Added all known extensiondefinitions known. Fixed some bugs.
// v1.2: Added more extensions
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////
// Last changed on: 03-may-2001.
//////////////////////////////////////////////////////////////////////

#ifndef _DEMOGL_EXTENSIONS_H
#define _DEMOGL_EXTENSIONS_H


////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// extension offset definitions. ALL known extensions are specified, allthough not every
// functioncall is defined in DemoGL_glext.h. 

#define	DGL_GL_ARB_multitexture				0			// ARB_multitexture extension (good -> use when available!)
#define	DGL_GL_ARB_MULTITEXTURE				DGL_GL_ARB_multitexture			// for backwards compatibility.
#define DGL_GL_EXT_abgr						1
#define	DGL_GL_EXT_blend_logic_op			2
#define	DGL_GL_EXT_clip_volume_hint			3
#define	DGL_GL_EXT_cmyka					4
#define	DGL_GL_EXT_compiled_vertex_array	5
#define	DGL_GL_EXT_coordinate_frame			6
#define	DGL_GL_EXT_copy_texture				7
#define	DGL_GL_EXT_cull_vertex				8
#define	DGL_GL_EXT_fog_coord				9
#define	DGL_GL_EXT_fragment_lighting		10
#define	DGL_GL_EXT_import_context			11
#define	DGL_GL_EXT_index_array_formats		12
#define	DGL_GL_EXT_index_func				13
#define	DGL_GL_EXT_index_material			14
#define	DGL_GL_EXT_index_texture			15
#define	DGL_GL_EXT_light_texture			16
#define	DGL_GL_EXT_misc_attribute			17
#define	DGL_GL_EXT_multitexture				18			// Developed for Quake. Don't use. Use ARB_multitexture instead
#define	DGL_GL_EXT_nurbs_tessalator			19
#define	DGL_GL_EXT_object_space_tess		20
#define	DGL_GL_EXT_paletted_texture			21			// nVidia: only GeForce and higher
#define	DGL_GL_EXT_pixel_transform			22
#define	DGL_GL_EXT_pixel_transform_color_table	23
#define	DGL_GL_EXT_point_parameters			24
#define	DGL_GL_EXT_polygon_offset			25
#define	DGL_GL_EXT_scene_maker				26
#define	DGL_GL_EXT_secondary_color			27
#define	DGL_GL_EXT_shared_texture_palette	28			// nVidia: only GeForce and higher
#define	DGL_GL_EXT_subtexture				29
#define	DGL_GL_EXT_texture					30
#define	DGL_GL_EXT_texture_env				31
#define	DGL_GL_EXT_texture_object			32			
#define	DGL_GL_EXT_texture_perturb_normal	33
#define	DGL_GL_EXT_vertex_array				34
#define DGL_GL_EXT_texture_env_combine		35

// Hewlett Packard (HP) extensions
#define DGL_GL_HP_image_transform			36
#define DGL_GL_HP_occlusion_test			37
#define DGL_GL_HP_texture_lighting			38

// IBM extensions
#define DGL_GL_IBM_rasterpos_clip			39
#define DGL_GL_IBM_static_vertex_array		40
#define DGL_GL_IBM_vertex_array_set			41

// Intel extensions
#define DGL_GL_INTEL_parallel_arrays		42
#define DGL_GL_INTEL_texture_scissor		43			

// PGI extensions
#define DGL_GL_PGI_misc_hints				50
#define DGL_GL_PGI_vertex_hints				51

// SGI extensions
#define DGL_GL_SGIX_clipmap					52
#define DGL_GL_SGIX_cushion					53
#define DGL_GL_SGIX_depth_texture			54
#define DGL_GL_SGIX_detail_texture			55
#define DGL_GL_SGIX_dmbuffer				56
#define DGL_GL_SGIX_filter4_parameters		57
#define DGL_GL_SGIX_flush_raster			58
#define DGL_GL_SGIX_fog_func				59
#define DGL_GL_SGIX_fog_offset				60
#define DGL_GL_SGIX_framezoom				61
#define DGL_GL_SGIX_generate_mipmap			62
#define DGL_GL_SGIX_instruments				63
#define DGL_GL_SGIX_interlace				64
#define DGL_GL_SGIX_ir_instruments			65
#define DGL_GL_SGIX_list_priority			66
#define DGL_GL_SGIX_multisample				67
#define DGL_GL_SGIX_pixel_texture			68
#define DGL_GL_SGIX_reference_plane			69
#define DGL_GL_SGIX_shadow					70
#define DGL_GL_SGIX_sharpen_texture			71
#define DGL_GL_SGIX_sprite					72
#define DGL_GL_SGIX_swap_barrier			73
#define DGL_GL_SGIX_swap_control			74
#define DGL_GL_SGIX_swap_group				75
#define DGL_GL_SGIX_tag_sample_buffer		76
#define DGL_GL_SGIX_texture_border_clamp	77
#define DGL_GL_SGIX_texture_color_table		78
#define DGL_GL_SGIX_texture_add_env			79
#define DGL_GL_SGIX_texture_filter4			80
#define DGL_GL_SGIX_texture_lod_bias		81
#define DGL_GL_SGIX_texture_select			82
#define DGL_GL_SGIX_texture4D				83
#define DGL_GL_SGIX_video_resize			84
#define DGL_GL_SGIX_video_source			85
#define DGL_GL_SGIX_video_sync				86
#define DGL_GL_SGIX_ycrcb					87
#define DGL_GL_SGIX_blend_alpha_minmax		88
#define DGL_GL_SGIX_impact_pixel_texture	89

// Sun's extensions
#define DGL_GL_EXT_multi_draw_array			90

// Windows extensions
#define	DGL_WGL_EXT_swap_control			91
#define DGL_GL_WIN_phong_shading			92
#define DGL_WGL_EXT_display_color_table		93
#define DGL_GL_WIN_swap_hint				94
#define DGL_GL_WIN_specular_fog				95

// other extensions from misc vendors
#define DGL_GL_EXT_packed_pixels			100
#define DGL_GL_EXT_stencil_wrap				101
#define DGL_GL_KTX_buffer_region			102			// Kinetix buffer region extension

// NV(idia) extensions
#define DGL_GL_NV_texgen_reflection			200			
#define DGL_GL_NV_texture_env_combine4		201
#define DGL_GL_EXT_texture_env_add			202
#define DGL_GL_EXT_bgra						203
#define	DGL_GL_EXT_blend_color				204			// GeForce specific
#define DGL_GL_EXT_blend_minmax				205			// GeForce specific
#define	DGL_GL_EXT_blend_subtract			206			// GeForce specific
#define DGL_GL_EXT_light_max_exponent		207
#define DGL_GL_EXT_rescale_normal			208			// GeForce specific
#define DGL_GL_EXT_texture_cube_map			209			// GeForce specific
#define DGL_GL_EXT_filter_anisotropic		210			// GeForce specific
#define DGL_GL_EXT_texture_lod_bias			211			// GeForce specific
#define DGL_GL_NV_register_combiners		212			// GeForce specific
#define DGL_GL_NV_texgen_emboss				213			// GeForce specific
#define DGL_GL_NV_vertex_array_range		214			// GeForce specific
#define DGL_GL_SGIS_texture_lod				215			// GeForce specific
#define DGL_GL_EXT_vertex_weighting			216
#define DGL_GL_NV_blend_square				217
#define	DGL_GL_NV_fog_distance				218
#define DGL_GL_EXT_separate_specular_color	219

//////////////////////////////////////////////////////////
// Added in DemoGL v1.3. Stored here to keep original numbering in tact (for backwards compatibility.)
//////////////////////////////////////////////////////////

#define DGL_GL_ARB_multisample					300
#define DGL_GL_ARB_texture_cube_map				301
#define DGL_GL_ARB_texture_compression			302
#define	DGL_GL_ARB_transpose_matrix				303
#define DGL_GL_ARB_texture_env_add				304

#define	DGL_GL_ATIX_texture_env_dot3			320
#define	DGL_GL_ATI_texture_mirror_once			321
#define	DGL_GL_ATI_vertex_blend					322
#define	DGL_GL_ATI_vertex_streams				323

#define	DGL_GL_EXT_blend_func_separate			350
#define	DGL_GL_EXT_color_subtable				351
#define DGL_GL_EXT_convolution					352
#define	DGL_GL_EXT_draw_range_elements			353
#define DGL_GL_EXT_histogram					354
#define	DGL_GL_EXT_multi_draw_arrays			355
#define	DGL_GL_EXT_multisample					356
#define DGL_GL_EXT_texgen_reflection			357
#define DGL_GL_EXT_texture3D					358
#define	DGL_GL_EXT_texture_compression_s3tc		359
#define DGL_GL_EXT_texture_edge_clamp			360
#define	DGL_GL_EXT_texture_filter_anisotropic	361

#define DGL_GL_SGI_color_matrix					400
#define DGL_GL_SGI_color_table					401
#define DGL_GL_SGIS_detail_texture				402
#define	DGL_GL_SGIS_fog_function				403
#define	DGL_GL_SGIS_generate_mipmap				404
#define	DGL_GL_SGIS_multisample					405
#define DGL_GL_SGIS_multitexture				406
#define DGL_GL_SGIS_pixel_texture				407
#define	DGL_GL_SGIS_point_line_texgen			408
#define	DGL_GL_SGIS_point_parameters			409
#define	DGL_GL_SGIS_sharpen_texture				410
#define DGL_GL_SGIS_texture4D					411
#define DGL_GL_SGI_texture_color_table			412
#define	DGL_GL_SGIS_texture_border_clamp		413
#define	DGL_GL_SGIS_texture_color_mask			414
#define	DGL_GL_SGIS_texture_edge_clamp			415
#define DGL_GL_SGIS_texture_filter4				416
#define	DGL_GL_SGIS_texture_select				417

#define DGL_GL_S3_s3tc							430

#define	DGL_GL_NV_light_max_exponent			450
#define DGL_GL_NV_fence							451

#define	DGL_GL_3DFX_multisample					470
#define	DGL_GL_3DFX_tbuffer						471
#define	DGL_GL_3DFX_texture_compression_FXT1	472

#define	DGL_WGL_ARB_extensions_string			490
#define DGL_WGL_EXT_extensions_string			491
#define DGL_WGL_EXT_make_current_read			492
#define DGL_WGL_EXT_pbuffer						493
#define DGL_WGL_EXT_pixel_format				494

///////////////////////////
// New defines in v1.31
///////////////////////////
#define DGL_WGL_ARB_pixel_format				495
#define	DGL_WGL_ARB_make_current_read			496
#define DGL_WGL_ARB_pbuffer						497
#define DGL_GL_ARB_texture_border_clamp			498
#define DGL_GL_ARB_texture_env_combine			499
#define DGL_GL_ARB_texture_env_crossbar			500
#define DGL_GL_ARB_texture_env_dot3				501
#define DGL_GL_ARB_vertex_blend					502
#define DGL_GL_ATIX_envmap_bumpmap				503				// ATIX extensions are experimental
#define DGL_GL_ATIX_pn_triangles				504				// ATIX extensions are experimental
#define DGL_GL_ATIX_texture_env_combine3		505				// ATIX extensions are experimental
#define DGL_GL_ATIX_texture_env_route			506				// ATIX extensions are experimental
#define DGL_GL_EXT_texture_env_dot3				507
#define DGL_GL_SGI_texture_edge_clamp			508

#define DGL_WGL_EXT_depth_float					509
#define DGL_WGL_NV_allocate_memory				510
#define DGL_GL_NV_vertex_program				511
#define DGL_GL_NV_evaluators					512
#define DGL_GL_NV_texture_rectangle				513
#define DGL_GL_NV_texture_shader				514
#define DGL_GL_NV_texture_shader2				515
#define DGL_GL_NV_register_combiners2			516
#define DGL_GL_NV_packed_depth_stencil			517
#define DGL_GL_IBM_texture_mirrored_repeat		518
#define DGL_GL_NV_texgen_compression_vtc		519
#define DGL_WGL_ARB_buffer_region				520
#define DGL_GL_ARB_imaging						521

#endif // _DEMOGL_EXTENSIONS_H