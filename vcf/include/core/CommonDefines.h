/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

//CommonDefines.h

#ifndef _COMMON_DEFINES_H__
#define _COMMON_DEFINES_H__



namespace VCF
{

/**
*Standard types for representing 
*property data
*/
enum PropertyDescriptorType{
	PROP_UNDEFINED = 0,
	PROP_INT,
	PROP_LONG,
	PROP_SHORT,
	PROP_ULONG,
	PROP_FLOAT,
	PROP_CHAR,
	PROP_DOUBLE,
	PROP_OBJECT,
	PROP_BOOL,
	PROP_STRING,
	PROP_ENUM,
	PROP_INTERFACE
};

/**
*the controls alignment type
*/
enum AlignmentType{
    ALIGN_NONE=0,
    ALIGN_TOP,
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_BOTTOM,
    ALIGN_CLIENT
};

/**
*A string array for the controls alignement types
*/
static String AlignmentTypeNames[] = { "ALIGN_NONE", 
                                         "ALIGN_TOP", 
										 "ALIGN_LEFT", 
										 "ALIGN_RIGHT", 
										 "ALIGN_BOTTOM", 
										 "ALIGN_CLIENT" };


/**
*An enum of anchor types for a control,
*may be masked together 
*/
enum AnchorType {
	ANCHOR_NONE = 0,
	ANCHOR_TOP = 1,
	ANCHOR_LEFT = 2,
	ANCHOR_BOTTOM = 4,
	ANCHOR_RIGHT = 8
};

/**
*an array of Anchor type names
*/
static String AnchorTypeNames[] = { "ANCHOR_NONE", 
                                         "ANCHOR_TOP", 
										 "ANCHOR_LEFT", 
										 "ANCHOR_BOTTOM", 
										 "ANCHOR_RIGHT" };


/**
*An enum of icon styles for list views
*/
enum IconStyleType {
	IS_LARGE_ICON=0,
	IS_SMALL_ICON,
	IS_LIST,
	IS_DETAILS
};

/**
*An enum of icon alignment types for list views
*/
enum IconAlignType {
	IA_NONE=0,
	IA_TOP,
	IA_LEFT,
	IA_AUTO_ARRANGE
};

static String IconStyleTypeNames[] = { "IS_LARGE_ICON", 
                                         "IS_SMALL_ICON", 
										 "IS_LIST", 
										 "IS_DETAILS" };

static String IconAlignTypeNames[] = { "IA_NONE", 
                                         "IA_TOP", 
										 "IA_LEFT", 
										 "IA_AUTO_ARRANGE" };

/**
*Mouse button masks
*/
#define MOUSE_UNDEFINED					0
#define MOUSE_LEFT_BUTTON				1
#define MOUSE_MIDDLE_BUTTON				2
#define MOUSE_RIGHT_BUTTON				4

/**
*Keyboard masks
*/
#define KEY_UNDEFINED					0
#define KEY_ALT							1
#define KEY_SHIFT						2
#define KEY_CTRL						4


//conversion defines
#define INT_STR_CONVERSION			"%d "
#define LONG_STR_CONVERSION			"%d "
#define SHORT_STR_CONVERSION		"%d "
#define FLOAT_STR_CONVERSION		"%.5f "
#define DOUBLE_STR_CONVERSION		"%.5f "
#define BOOL_STR_CONVERSION_TRUE	"true "
#define BOOL_STR_CONVERSION_FALSE	"false "

#define STR_INT_CONVERSION			"%d"
#define STR_LONG_CONVERSION			"%d"
#define STR_ULONG_CONVERSION		"%d"
#define STR_CHAR_CONVERSION			"%c"
#define STR_SHORT_CONVERSION		"%d"
#define STR_FLOAT_CONVERSION		"%f"
#define STR_DOUBLE_CONVERSION		"%f"
#define STR_BOOL_CONVERSION_TRUE	"true"
#define STR_BOOL_CONVERSION_FALSE	"false"


#define VIRT_KEY_F1					200
#define VIRT_KEY_F2					201
#define VIRT_KEY_F3					202
#define VIRT_KEY_F4					203
#define VIRT_KEY_F5					204
#define VIRT_KEY_F6					205
#define VIRT_KEY_F7					206
#define VIRT_KEY_F8					207
#define VIRT_KEY_F9					208
#define VIRT_KEY_F10				209
#define VIRT_KEY_F11				210
#define VIRT_KEY_F12				211
#define VIRT_KEY_UP_ARROW			212
#define VIRT_KEY_LEFT_ARROW			213
#define VIRT_KEY_RIGHT_ARROW		214
#define VIRT_KEY_DOWN_ARROW			215
#define VIRT_KEY_PG_UP				216
#define VIRT_KEY_PG_DOWN			217
#define VIRT_KEY_DELETE				218
#define VIRT_KEY_RETURN				219
#define VIRT_KEY_BACKSPACE			220

};
	

#endif