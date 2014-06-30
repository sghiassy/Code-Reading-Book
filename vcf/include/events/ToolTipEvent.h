//ToolTipEvent.h

/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/

#ifndef _TOOLTIPEVENT_H__
#define _TOOLTIPEVENT_H__




namespace VCF  {

class GraphicsContext;

#define TOOLTIP_EVENT_TIP_REQESTED		CUSTOM_EVENT_TYPES + 3000



//Class ToolTipEvent documentation

class APPKIT_API ToolTipEvent : public Event { 
public:
	ToolTipEvent( Object* source, const ulong32& type );

	virtual ~ToolTipEvent();

	String getToolTipString() {
		return m_tooltipString;
	}

	void setToolTipString( const String& tooltipString ) {
		m_tooltipString = tooltipString;
	}

	Point* getToolTipLocation() {
		return &m_tooltipLocation;
	}

	void setToolTipLocation( Point* pt ) {
		m_tooltipLocation = *pt;
	}

	Size* getToolTipSize() {
		return &m_tooltipSize;
	}

	void setToolTipSize( Size* size ) {
		m_tooltipSize = *size;
	}

	GraphicsContext* getCustomToolTipContext() {
		return m_customToolTipGraphicsCtx;
	}
	
	void setCustomTooltipContext( GraphicsContext* customToolTipGraphicsCtx ) {
		m_customToolTipGraphicsCtx = customToolTipGraphicsCtx;
	}
protected:
	String m_tooltipString;
	GraphicsContext* m_customToolTipGraphicsCtx;
	Point m_tooltipLocation;
	Size m_tooltipSize;
};


}; //end of namespace VCF


#endif //_TOOLTIPEVENT_H__


