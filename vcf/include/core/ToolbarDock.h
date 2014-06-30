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

#ifndef _TOOLBARDOCK_H__
#define _TOOLBARDOCK_H__


namespace VCF {

enum ToolbarDockSide {
	TDS_TOP=0,
	TDS_LEFT,
	TDS_RIGHT,
	TDS_BOTTOM
};

static String ToolbarDockSideNames[] = { "TDS_TOP", 
                                         "TDS_LEFT", 
										 "TDS_RIGHT", 
										 "TDS_BOTTOM" };

enum ToolbarBackgroundStyle {
	TBKS_NONE=0,
	TBKS_TILE,
	TBKS_STRETCH
};


static String ToolbarBackgroundStyleNames[] = { "TBKS_NONE", 
											"TBKS_TILE", 
											"TBKS_STRETCH" };


#define DEFAULT_TOOLBAR_HEIGHT			25

class Toolbar;

#define TOOLBARDOCK_CLASSID			"54AE13E1-113D-4dcf-90F9-18FBBF713179"


class APPKIT_API ToolbarDock : public CustomControl, public AbstractContainer {
public:
	
	BEGIN_CLASSINFO(ToolbarDock, "VCF::ToolbarDock", "VCF::CustomControl", TOOLBARDOCK_CLASSID);
	LABELED_ENUM_PROPERTY( ToolbarDockSide, "dockSide", ToolbarDock::getDockSide, ToolbarDock::setDockSide, 
						   TDS_TOP, TDS_BOTTOM, 4, ToolbarDockSideNames);	

	LABELED_ENUM_PROPERTY( ToolbarBackgroundStyle, "backgroundStyle", ToolbarDock::getBackgroundStyle, ToolbarDock::setBackgroundStyle, 
						   TBKS_NONE, TBKS_STRETCH, 3, ToolbarBackgroundStyleNames);	
	
	OBJECT_PROPERTY( Image, "background", ToolbarDock::getBackground, ToolbarDock::setBackground );
	
	PROPERTY( bool, "usingBackground", ToolbarDock::isUsingBackground, ToolbarDock::setUsingBackground, PROP_BOOL );	
	PROPERTY( bool, "allowsDragging", ToolbarDock::allowsDragging, ToolbarDock::setAllowsDragging, PROP_BOOL );	

	END_CLASSINFO(ToolbarDock);

	ToolbarDock();

	virtual ~ToolbarDock();	
	
	ToolbarDockSide getDockSide();

	void setDockSide( const ToolbarDockSide& dockSide );

	Image* getBackground();

	void setBackground( Image* background );

	ToolbarBackgroundStyle getBackgroundStyle();

	void setBackgroundStyle( const ToolbarBackgroundStyle& backgroundStyle );

	bool isUsingBackground();

	void setUsingBackground( const bool& usingBackground );

	bool allowsDragging();

	void setAllowsDragging( const bool& allowsDrag );

	virtual void paint( GraphicsContext* context );

	virtual void mouseDown( MouseEvent* event );
	
	virtual void mouseMove( MouseEvent* event );

	virtual void mouseUp( MouseEvent* event );

	void addToolbar( Toolbar* toolbar );
	
protected:

	/**
	*add overriden to prevent the adding of non-Toolbar Controls - this is 
	*the only type if control the ToolbarDock will allow - anythign will
	*cause an exception to be thrown
	*/
	virtual void add( Control * child );

	virtual void add( Control * child, const AlignmentType& alignment );
	
	virtual void remove( Control* child );

	ToolbarDockSide m_dockSide;
	ToolbarBackgroundStyle m_backgroundStyle;
	Image* m_backgroundImage;
	bool m_isUsingBackground;
	bool m_allowsDockDragging;
	std::vector<Toolbar*> m_toolbars;
	double m_originalHeightWidth; //used for either the original height or width depending on the m_dockSide
	Toolbar* findToolbar( Point* pt );

	void recalcToolbarLayout();
};


/**
*DockManager class uses to manage ToolbarDock's and floating toolbars
*/
class DockManager : public VCF::Object {
public:
	static DockManager* create();

	DockManager();

	virtual ~DockManager();
	
	static DockManager* getDockManager() {
		return dockManagerInstance;
	}

	void addDock( ToolbarDock* dock ) {
		m_docks.push_back( dock );
	}

	void removeDock( ToolbarDock* dock );

	/**
	*@param Point point is in Screen coordinates
	*/
	ToolbarDock* findDock( Point* point );

	void addFloatingToolbar( Toolbar* toolbar );

	void removeFloatingToolbar( Toolbar* toolbar );
private:
	std::vector<ToolbarDock*> m_docks;
	std::vector<Toolbar*> m_floatingToolbars;
	static DockManager* dockManagerInstance;
};

}; //end of namespace VCF

#endif //_TOOLBARDOCK_H__