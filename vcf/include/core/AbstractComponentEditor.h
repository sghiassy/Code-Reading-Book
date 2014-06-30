//AbstractComponentEditor.h

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

#ifndef _ABSTRACTCOMPONENTEDITOR_H__
#define _ABSTRACTCOMPONENTEDITOR_H__




namespace VCF  {

/**
*Class AbstractComponentEditor documentation
*/
class APPKIT_API AbstractComponentEditor : public VCF::Object, public ComponentEditor { 
public:
	AbstractComponentEditor();

	virtual ~AbstractComponentEditor();

	virtual void intialize();

	virtual Command* getCommand( const ulong32& index );

	virtual ulong32 getCommandCount() {
		return m_commandCount;
	}

	virtual ulong32 getDefaultCommandIndex() {
		return m_defaultCommandIndex;
	}

	virtual Component* getComponent() {
		return m_component;
	}

	virtual void setComponent( Component* component );

	virtual String getComponentVFFFragment();
protected:
	ulong32 m_commandCount;
	ulong32 m_defaultCommandIndex;
	
	Component* m_component;
};

/**
*Class AbstractComponentEditor documentation
*/
class APPKIT_API AbstractControlEditor : public AbstractComponentEditor, public ControlEditor { 
public:
	AbstractControlEditor();

	virtual ~AbstractControlEditor();

	virtual bool isParentValid( Control* parent );

	virtual Control* getControl();

	virtual void setControl( Control* control );

	virtual void mouseDown( MouseEvent* event );

	virtual void mouseMove( MouseEvent* event );

	virtual void mouseUp( MouseEvent* event );
protected:

private:
};

}; //end of namespace VCF

#endif //_ABSTRACTCOMPONENTEDITOR_H__


