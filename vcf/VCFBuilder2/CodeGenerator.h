//CodeGenerator.h

#ifndef _CODEGENERATOR_H__
#define _CODEGENERATOR_H__


namespace VCFBuilder  {

class Form;
class GenericClass;
class CPPClassInstance;

/**
*Class CodeGenerator 
*A CodeGeneerator is called at various times to generate
*the proper code for a component or form
*The times this is called are as follows:
*	1)	When creating a new Form to generate the initial
*		code base (typically a .H and .CPP file of the Form
*		ClassName.H and ClassName.CPP)
*	2)	When adding a component to a Form class
*	3)	When removing a component from a Form class
*	4)	When adding a new event handler for a component's
*		selected event
*	5)	When removing an event handler
*	6)	When creating a new class
*	7)	When creating a new class instance. This is similar to (2) above
*		in that there is an implied parent-child
*		relationship involved, i.e. adding a member
*		variable to class that is itself a class,
*		thus involving a new class instance
*	8)	removing a class instance
*	9)	changing the name of a form
*	10)	changing the name of a class
*/
class CodeGenerator { 
public:
	virtual ~CodeGenerator(){};

	/**
	*set the owning form for the code generator
	*this will be referred to by the other operations
	*such as addComponent() or removeComponent()
	*/
	virtual void setOwningForm( Form* owningForm ) = 0;

	virtual void setOwningClass( GenericClass* owningClass ) = 0;

	virtual void addForm( Form* newForm ) = 0;

	virtual void addComponent( Component* newComponent ) = 0;

	virtual void removeComponent( Component* newComponent ) = 0;

	/**
	*initializes the events of a form from a source
	*file
	*/
	virtual void initializeFormEventsFromSource() = 0;

	/**
	*adds a new event connection
	*the listener componenent is the object 
	*being listened to (like a menu item)
	*and the source is the object implementing 
	*the function that will be called by the specified
	*event property handler that is created
	*/
	virtual void addEvent( const String& eventHandlerCallback, Component* source, EventProperty* eventProperty ) = 0;

	/**
	*removes an event connection
	*/
	virtual void removeEvent( const String& eventHandlerCallback, Component* source, EventProperty* eventProperty ) = 0;

	virtual void addClass( GenericClass* newClass ) = 0;

	virtual void addClassInstance( CPPClassInstance* classInstance ) = 0;

	virtual void changeFormName( const String& newName, const String& oldName ) = 0;

	virtual void changeClassName( const String& newName, const String& oldName ) = 0;

	/**
	*changes the name of a member variable for a Class/Form
	*If the class is one that is associated with a Form, then the 
	*following extra steps take place:
	*<ul>
	*	<li>the bind() method associated with this member is changed accordingly
	*	<li>any event callbacks are also updated as well
	*</ul>
	*/
	virtual void changeMemberName( const String& newName, const String& oldName ) = 0;

	virtual String generateEventMethodName(  Component* listener, EventProperty* eventProperty ) = 0;

	virtual String generateMemberVariableNameFromComponent( Component* component ) = 0;
};


}; //end of namespace VCFBuilder

#endif //_CODEGENERATOR_H__


