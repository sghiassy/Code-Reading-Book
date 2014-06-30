//ProjectObjectTraverser.h

#ifndef _PROJECTOBJECTTRAVERSER_H__
#define _PROJECTOBJECTTRAVERSER_H__



namespace VCFBuilder  {

/**
*Class ProjectObjectTraverser documentation
*/
class ProjectObjectTraverser { 
public:
	virtual ~ProjectObjectTraverser();

	virtual void processClass( CPPClass* classObject ) = 0;

	virtual void processComponent( ComponentInstance* compInstObject ) = 0;

	virtual void processForm( Form* formObject ) = 0;
};


}; //end of namespace VCFBuilder

#endif //_PROJECTOBJECTTRAVERSER_H__


