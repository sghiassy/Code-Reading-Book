//DefaultUIProject.h

#ifndef _DEFAULTUIPROJECT_H__
#define _DEFAULTUIPROJECT_H__


namespace VCFBuilder  {

/**
*Class DefaultUIProject documentation
*/
class DefaultUIProject : public Project { 
public:
	DefaultUIProject();

	virtual ~DefaultUIProject();

	virtual void addForm( Form* newForm );

	virtual void removeForm( Form* formToRemove );

	virtual void addClass( CPPClass* newClass );

	virtual void removeClass( CPPClass* classToRemove );

	virtual void addClassInstance( CPPClassInstance* newInstance );

	virtual void removeClassInstance( CPPClassInstance* instanceToRemove );

	virtual GenericClass* createNewClass( ProjectObject* parent );

	virtual Form* createNewForm( ProjectObject* parent );

	virtual void intializeSettings();
protected:

private:
};


}; //end of namespace VCFBuilder

#endif //_DEFAULTUIPROJECT_H__


