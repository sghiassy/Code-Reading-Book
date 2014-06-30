//DefaultComponentEditor.h

#ifndef _DEFAULTCOMPONENTEDITOR_H__
#define _DEFAULTCOMPONENTEDITOR_H__





namespace VCFBuilder  {

/**
*DefaultComponentEditor is the default component editor
*for a component using the VCFBuilder.
*The following commands are exposed by the DefaultComponentEditor:
*<ul>
*	<li>Delete, unless the component is the top level frame
*	<li>Cut
*	<li>Copy
*	<li>Paste
*	<li>Tab Order
*	<li>Send to Back
*	<li>Bring to Front
*</ul>
*/
class DefaultComponentEditor : public VCF::AbstractControlEditor { 
public:
	DefaultComponentEditor();

	virtual ~DefaultComponentEditor();

	virtual VCF::Command* getCommand( const ulong32& index );
protected:


};


}; //end of namespace VCF

#endif //_DEFAULTCOMPONENTEDITOR_H__


