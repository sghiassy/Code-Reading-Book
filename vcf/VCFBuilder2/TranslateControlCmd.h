//TranslateControlCmd.h

#ifndef _TRANSLATECONTROLCMD_H__
#define _TRANSLATECONTROLCMD_H__



#include "AbstractCommand.h"


using namespace VCF;


namespace VCFBuilder  {

/**
*Class TranslateControlCmd documentation
*/
class TranslateControlCmd : public VCF::AbstractCommand { 
public:
	TranslateControlCmd( const String& controlName, Rect* oldBounds, Rect* newBounds );

	virtual ~TranslateControlCmd();

	virtual void undo();

	virtual void redo();

	virtual void execute();

protected:
	String m_controlName;
	Rect m_oldBounds;
	Rect m_newBounds;

	Control* findControl();
private:
};

class SizeControlCmd : public TranslateControlCmd {
public:
	SizeControlCmd(  const String& controlName, Rect* oldBounds, Rect* newBounds );

	virtual ~SizeControlCmd(){};


};



}; //end of namespace VCFBuilder

#endif //_TRANSLATECONTROLCMD_H__


