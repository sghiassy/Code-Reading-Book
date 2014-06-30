//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllextensions.cpp
// PURPOSE: in here is the Cextensions class implementation
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
// See also License.txt in the sourcepackage.
//
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: No changes.
// v1.2: Added to codebase
// v1.1: --
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

///////////////////////////////////////////////////////////////////////
//
// Global variables for extensionstring parser. Have file (.obj) scope.
//
///////////////////////////////////////////////////////////////////////

SExtParseInfo m_garrExtParseInfo[]=
{
	//////////////////////////
	// 3DFX extensions
	//////////////////////////
	{"GL_3DFX_multisample ", DGL_GL_3DFX_multisample},
	{"GL_3DFX_tbuffer ", DGL_GL_3DFX_tbuffer},
	{"GL_3DFX_texture_compression_FXT1 ", DGL_GL_3DFX_texture_compression_FXT1},
	//////////////////////////
	// ARB extensions
	//////////////////////////
	{"GL_ARB_imaging", DGL_GL_ARB_imaging},
	{"GL_ARB_multitexture ", DGL_GL_ARB_multitexture},
	{"GL_ARB_multisample ", DGL_GL_ARB_multisample},
	{"GL_ARB_texture_border_clamp ", DGL_GL_ARB_texture_border_clamp},
	{"GL_ARB_texture_compression ", DGL_GL_ARB_texture_compression},
	{"GL_ARB_texture_cube_map ", DGL_GL_ARB_texture_cube_map},
	{"GL_ARB_texture_env_add ", DGL_GL_ARB_texture_env_add},
	{"GL_ARB_texture_env_combine ", DGL_GL_ARB_texture_env_combine},
	{"GL_ARB_texture_env_crossbar ", DGL_GL_ARB_texture_env_crossbar},
	{"GL_ARB_texture_env_dot3 ", DGL_GL_ARB_texture_env_dot3},
	{"GL_ARB_transpose_matrix ", DGL_GL_ARB_transpose_matrix},
	{"GL_ARB_vertex_blend ", DGL_GL_ARB_vertex_blend},
	//////////////////////////
	// ATI(X) extensions
	//////////////////////////
	{"GL_ATIX_envmap_bumpmap ", DGL_GL_ATIX_envmap_bumpmap}, 
	{"GL_ATIX_pn_triangles ", DGL_GL_ATIX_pn_triangles}, 
	{"GL_ATIX_texture_env_combine3 ", DGL_GL_ATIX_texture_env_combine3},
	{"GL_ATIX_texture_env_dot3 ", DGL_GL_ATIX_texture_env_dot3},
	{"GL_ATIX_texture_env_route ", DGL_GL_ATIX_texture_env_route},
	{"GL_ATI_texture_mirror_once ", DGL_GL_ATI_texture_mirror_once},
	{"GL_ATI_vertex_blend ", DGL_GL_ATI_vertex_blend},
	{"GL_ATI_vertex_streams ", DGL_GL_ATI_vertex_streams},
	//////////////////////////
	// EXT extensions
	//////////////////////////
	{"GL_EXT_abgr ", DGL_GL_EXT_abgr},
	{"GL_EXT_bgra ", DGL_GL_EXT_bgra},
	{"GL_EXT_blend_color ", DGL_GL_EXT_blend_color},
	{"GL_EXT_blend_func_separate ", DGL_GL_EXT_blend_func_separate},
	{"GL_EXT_blend_minmax ", DGL_GL_EXT_blend_minmax},
	{"GL_EXT_blend_subtract ", DGL_GL_EXT_blend_subtract},
	{"GL_EXT_color_subtable ", DGL_GL_EXT_color_subtable},
	{"GL_EXT_copy_texture ", DGL_GL_EXT_copy_texture},
	{"GL_EXT_clip_volume_hint ", DGL_GL_EXT_clip_volume_hint},
	{"GL_EXT_compiled_vertex_array ", DGL_GL_EXT_compiled_vertex_array},
	{"GL_EXT_convolution ", DGL_GL_EXT_convolution},
	{"GL_EXT_coordinate_frame ", DGL_GL_EXT_coordinate_frame},
	{"GL_EXT_cull_vertex ", DGL_GL_EXT_cull_vertex},
	{"GL_EXT_draw_range_elements ", DGL_GL_EXT_draw_range_elements},
	{"GL_EXT_filter_anisotropic ", DGL_GL_EXT_filter_anisotropic},
	{"GL_EXT_fog_coord ", DGL_GL_EXT_fog_coord},
	{"GL_EXT_histogram ", DGL_GL_EXT_histogram},
	{"GL_EXT_index_array_formats ", DGL_GL_EXT_index_array_formats},
	{"GL_EXT_index_func ", DGL_GL_EXT_index_func},
	{"GL_EXT_index_material ", DGL_GL_EXT_index_material},
	{"GL_EXT_light_max_exponent ", DGL_GL_EXT_light_max_exponent},
	{"GL_EXT_light_texture ", DGL_GL_EXT_light_texture},
	{"GL_EXT_multi_draw_arrays ", DGL_GL_EXT_multi_draw_arrays},
	{"GL_EXT_multisample ", DGL_GL_EXT_multisample},
	{"GL_EXT_packed_pixels ", DGL_GL_EXT_packed_pixels},
	{"GL_EXT_paletted_texture ", DGL_GL_EXT_paletted_texture},
	{"GL_EXT_pixel_transform ", DGL_GL_EXT_pixel_transform},
	{"GL_EXT_point_parameters ", DGL_GL_EXT_point_parameters},
	{"GL_EXT_polygon_offset ", DGL_GL_EXT_polygon_offset},
	{"GL_EXT_rescale_normal ", DGL_GL_EXT_rescale_normal},
	{"GL_EXT_secondary_color ", DGL_GL_EXT_secondary_color},
	{"GL_EXT_separate_specular_color ", DGL_GL_EXT_separate_specular_color},
	{"GL_EXT_shared_texture_palette ", DGL_GL_EXT_shared_texture_palette},
	{"GL_EXT_stencil_wrap ", DGL_GL_EXT_stencil_wrap},
	{"GL_EXT_subtexture ", DGL_GL_EXT_subtexture},
	{"GL_EXT_texgen_reflection ", DGL_GL_EXT_texgen_reflection},
	{"GL_EXT_texture ", DGL_GL_EXT_texture},
	{"GL_EXT_texture3D ", DGL_GL_EXT_texture3D},
	{"GL_EXT_texture_compression_s3tc ", DGL_GL_EXT_texture_compression_s3tc},
	{"GL_EXT_texture_cube_map ", DGL_GL_EXT_texture_cube_map},
	{"GL_EXT_texture_edge_clamp ", DGL_GL_EXT_texture_edge_clamp},
	{"GL_EXT_texture_env_add ", DGL_GL_EXT_texture_env_add},
	{"GL_EXT_texture_env_combine ", DGL_GL_EXT_texture_env_combine},
	{"GL_EXT_texture_env_dot3 ", DGL_GL_EXT_texture_env_dot3},
	{"GL_EXT_texture_filter_anisotropic ", DGL_GL_EXT_texture_filter_anisotropic},
	{"GL_EXT_texture_lod_bias ", DGL_GL_EXT_texture_lod_bias},
	{"GL_EXT_texture_object ", DGL_GL_EXT_texture_object},
	{"GL_EXT_texture_perturb_normal ", DGL_GL_EXT_texture_perturb_normal},
	{"GL_EXT_vertex_array ", DGL_GL_EXT_vertex_array},
	{"GL_EXT_vertex_weighting ", DGL_GL_EXT_vertex_weighting},
	//////////////////////////
	// NV extensions
	//////////////////////////
	{"GL_NV_blend_square ", DGL_GL_NV_blend_square},
	{"GL_NV_evaluators ", DGL_GL_NV_evaluators},
	{"GL_NV_fence ", DGL_GL_NV_fence},
	{"GL_NV_fog_distance ", DGL_GL_NV_fog_distance},
	{"GL_NV_light_max_exponent ", DGL_GL_NV_light_max_exponent},
	{"GL_NV_packed_depth_stencil ", DGL_GL_NV_packed_depth_stencil},
	{"GL_NV_register_combiners ", DGL_GL_NV_register_combiners},
	{"GL_NV_register_combiners2 ", DGL_GL_NV_register_combiners2},
	{"GL_NV_texgen_compression_vtc ", DGL_GL_NV_texgen_compression_vtc},
	{"GL_NV_texgen_emboss ", DGL_GL_NV_texgen_emboss},
	{"GL_NV_texgen_reflection ", DGL_GL_NV_texgen_reflection},
	{"GL_NV_texture_env_combine4 ", DGL_GL_NV_texture_env_combine4},
	{"GL_NV_texture_shader ", DGL_GL_NV_texture_shader},
	{"GL_NV_texture_shader2 ", DGL_GL_NV_texture_shader2},
	{"GL_NV_texture_rectangle ", DGL_GL_NV_texture_rectangle},
	{"GL_NV_vertex_array_range ", DGL_GL_NV_vertex_array_range},
	{"GL_NV_vertex_program ", DGL_GL_NV_vertex_program},
	//////////////////////////
	// SGI(S) extensions
	//////////////////////////
	{"GL_SGI_color_matrix ", DGL_GL_SGI_color_matrix},
	{"GL_SGI_color_table ", DGL_GL_SGI_color_table},
	{"GL_SGIS_detail_texture ", DGL_GL_SGIS_detail_texture},
	{"GL_SGIS_fog_function ", DGL_GL_SGIS_fog_function},
	{"GL_SGIS_generate_mipmap ", DGL_GL_SGIS_generate_mipmap},
	{"GL_SGIS_multisample ", DGL_GL_SGIS_multisample},
	{"GL_SGIS_multitexture ", DGL_GL_SGIS_multitexture},
	{"GL_SGIS_pixel_texture ", DGL_GL_SGIS_pixel_texture},
	{"GL_SGIS_point_line_texgen ", DGL_GL_SGIS_point_line_texgen},
	{"GL_SGIS_point_parameters ", DGL_GL_SGIS_point_parameters},
	{"GL_SGIS_sharpen_texture ", DGL_GL_SGIS_sharpen_texture},
	{"GL_SGIS_texture4D ", DGL_GL_SGIS_texture4D},
	{"GL_SGIS_texture_border_clamp ", DGL_GL_SGIS_texture_border_clamp},
	{"GL_SGIS_texture_color_mask ", DGL_GL_SGIS_texture_color_mask},
	{"GL_SGI_texture_color_table ", DGL_GL_SGI_texture_color_table},
	{"GL_SGIS_texture_edge_clamp ", DGL_GL_SGIS_texture_edge_clamp},
	{"GL_SGI_texture_edge_clamp ", DGL_GL_SGI_texture_edge_clamp},
	{"GL_SGIS_texture_filter4	 ", DGL_GL_SGIS_texture_filter4	},
	{"GL_SGIS_texture_lod ", DGL_GL_SGIS_texture_lod},
	{"GL_SGIS_texture_select ", DGL_GL_SGIS_texture_select},
	//////////////////////////
	// WIN/WGL extensions
	//////////////////////////
	{"GL_WIN_swap_hint ", DGL_GL_WIN_swap_hint},
	{"GL_WIN_phong_shading ", DGL_GL_WIN_phong_shading},
	{"GL_WIN_specular_fog ", DGL_GL_WIN_specular_fog},
	{"WGL_ARB_buffer_region ", DGL_WGL_ARB_buffer_region},
	{"WGL_ARB_extensions_string ", DGL_WGL_ARB_extensions_string},
	{"WGL_ARB_make_current_read ", DGL_WGL_ARB_make_current_read},
	{"WGL_ARB_pbuffer ", DGL_WGL_ARB_pbuffer},
	{"WGL_ARB_pixel_format ", DGL_WGL_ARB_pixel_format},
	{"WGL_EXT_depth_float ", DGL_WGL_EXT_depth_float},
	{"WGL_EXT_extensions_string ", DGL_WGL_EXT_extensions_string},
	{"WGL_EXT_make_current_read ", DGL_WGL_EXT_make_current_read},
	{"WGL_EXT_pbuffer ", DGL_WGL_EXT_pbuffer},
	{"WGL_EXT_pixel_format ", DGL_WGL_EXT_pixel_format},
	{"WGL_EXT_swap_control ", DGL_WGL_EXT_swap_control},
	{"WGL_NV_allocate_memory ", DGL_WGL_NV_allocate_memory},
	//////////////////////////
	// MISC extensions
	//////////////////////////
	{"GL_IBM_texture_mirrored_repeat ", DGL_GL_IBM_texture_mirrored_repeat},
	{"GL_KTX_buffer_region ", DGL_GL_KTX_buffer_region},
	{"GL_S3_s3tc ", DGL_GL_S3_s3tc},

	//////////////////////////
	//{"name", id},
	// ADD MORE HERE 
	//////////////////////////

	//////////////////////////
	// LEAVE LINE BELOW
	//////////////////////////
	{"DGLEXPARSEINFOEND", -1}
};

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: constructor
CExtensions::CExtensions()
{
	int		i;

	// initialize extensionarr
	for(i=0;i<MAX_EXT;i++)
	{
		m_arrbExtensions[i]=false;
	}
}


// Purpose: destructor
CExtensions::~CExtensions()
{
}


// Purpose: sets extension passed to TRUE, i.e. available
void
CExtensions::SetExtensionAvailability(int iExtension)
{
	m_arrbExtensions[iExtension]=true;
}


// Purpose: gets extension availability.
bool
CExtensions::GetExtensionAvailability(const int iExtension)
{
	return m_arrbExtensions[iExtension];
}


// Purpose: this routine parses the extensionstring stored in DemoDat and sets flags in the 
// extension flagstore m_arrbExtensions[] so other effects can query this object and set 
// functionpointers to the extensionfunctions themselves.
void
CExtensions::ParseExtensionsString()
{
	char		*pszExtensionString;
	int			i;
	CStdString	sToLog;
	
	// Get the extensionstring
	pszExtensionString = m_gpDemoDat->GetOGLExtensions();

	for(i=0;m_garrExtParseInfo[i].m_iID>=0;i++)
	{
		// Can't use DEMOGL_ExtensionCheckIfAvailable(char *) because we're called from another thread.
		if((strstr(pszExtensionString, m_garrExtParseInfo[i].m_sName)!=NULL))
		{
			m_gpExtensions->SetExtensionAvailability(m_garrExtParseInfo[i].m_iID);
			sToLog.Format("Extension ' %s' found and marked Available.", m_garrExtParseInfo[i].m_sName);
			// Log that the extension was found and is available. Flag to not update the screen, since
			// this will flood some nVidia drivers with WM_PAINT messages which will stall the
			// messageloop. 
			LogFeedback(&sToLog[0],true,"CExtensions::ParseExtensionString",true, false);
		}
	}
}

