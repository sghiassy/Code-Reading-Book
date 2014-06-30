//CPPCodeGenerator.h

#ifndef _CPPCODEGENERATOR_H__
#define _CPPCODEGENERATOR_H__

#include "..\CPPParser\CPPCodeParser.h"


#define VCFINIT_METHOD_NAME		"vcfInit"

namespace VCFBuilder  {

class CPPStatement {
public:
	CPPStatement() {
		m_startPos = -1;
		m_endPos = -1;
	}

	virtual ~CPPStatement(){};

	long m_startPos;
	long m_endPos;
};

class CPPMethodCallStatement : public CPPStatement {
public:
	CPPMethodCallStatement(){};

	virtual ~CPPMethodCallStatement(){};

	CPPMethodCallStatement( const CPPMethodCallStatement& cms ) {
		m_endPos = cms.m_endPos;
		m_startPos = cms.m_startPos;
		m_variable = cms.m_variable;
		m_specifier = cms.m_specifier;
		m_methodName = cms.m_methodName;
		m_methodArgNames = cms.m_methodArgNames;
	}

	VCF::String m_variable;

	//either "." (a reference) or "->" (a pointer)
	VCF::String m_specifier;
	VCF::String m_methodName;
	std::vector<String> m_methodArgNames;
};

/**
*Class CPPCodeGenerator documentation
*/
class CPPCodeGenerator : public VCF::Object, public CodeGenerator { 
public:
	CPPCodeGenerator();

	virtual ~CPPCodeGenerator();

	virtual void addForm( Form* newForm );

	/**
	*set the owning form for the code generator
	*this will be referred to by the other operations
	*such as addComponent() or removeComponent()
	*/
	virtual void setOwningForm( Form* owningForm );

	virtual void setOwningClass( GenericClass* owningClass );

	virtual void addComponent( Component* newComponent );

	virtual void removeComponent( Component* newComponent );

	/**
	*adds a new event connection
	*the eventHandlerCallback 
	*being listened to (like a menu item)
	*and the source is the object implementing 
	*the function that will be called by the specified
	*event property handler that is created
	*/
	virtual void addEvent( const String& eventHandlerCallback, Component* source, EventProperty* eventProperty );

	/**
	*removes an event connection
	*/
	virtual void removeEvent( const String& eventHandlerCallback, Component* source, EventProperty* eventProperty );	

	virtual void addClass( GenericClass* newClass );

	virtual void addClassInstance( CPPClassInstance* classInstance );

	virtual void changeFormName( const String& newName, const String& oldName );

	virtual void changeClassName( const String& newName, const String& oldName );

	virtual void changeMemberName( const String& newName, const String& oldName );

	virtual String generateEventMethodName(  Component* listener, EventProperty* eventProperty );

	virtual String generateMemberVariableNameFromComponent( Component* component );

	virtual void initializeFormEventsFromSource();
protected:
	Form* m_owningForm;

	GenericClass* m_owningClass;

	CPPCodeParser m_codeParser;

	String m_vcfInitMethodName;

	long getMemberVariableInsertLocation();
	
	long getEventHandlerInsertLocation( Component* component );

	void insertEventHandlerCode( const String& eventCode, long insertPos );

	String generateEventHandlerCode( const String& eventHandlerCallback, Component* component, EventProperty* eventProperty );

	String getMemberNameFromComponent( Component* component );

	String m_classImplContents;

	void initializeClassImplContents( GenericClass* aClass );

	String m_classDeclContents;

	void initializeClassDeclContents( GenericClass* aClass );

	String createInitialCPPHeader( GenericClass* aClass, long& insertPos );

	String createDefaultClassDecl( GenericClass* aClass, long& insertPos );

	String createInitialCPPImpl( GenericClass* aClass );

	String createDefaultClassImpl( GenericClass* aClass );

	String generateVCFInitMethodDecl( GenericClass* aClass );
	
	String generateVCFInitMethodImpl( GenericClass* aClass );

	bool hasDeclMethodName( const String& method );

	bool hasImplMethodName( const String& method );

	long getDeclMethodInsertionPoint( GenericClass* aClass );
	
	void addNewComponentToClassDecl( Component* newComponent );

	void addNewComponentToClassImpl( Component* newComponent );

	void removeComponentFromClassDecl( Component* component );

	void removeComponentFromClassImpl( Component* component );

	void addMethodDeclToClass( GenericClass* aClass, CPPMethodCtx* newMethod );

	void addMethodImplToClass( GenericClass* aClass, CPPMethodCtx* newMethod );

	long getVisibilityInsertPos( CPPClassCtx* classCtx, const String& visibility );

	void writeClassDeclToFile( GenericClass* aClass );

	void writeClassImplToFile( GenericClass* aClass );

	String generateMethod( GenericClass* aClass, CPPMethodCtx* newMethod, const bool& declareAsImplementation );

	bool m_newClassIsDerivedFromFrame;

	void parseVCFInitMethod( std::vector<CPPMethodCallStatement>& methodStatements );

	bool isA( const String& className, const String& classNameToMatch );
};


}; //end of namespace VCFBuilder

#endif //_CPPCODEGENERATOR_H__


