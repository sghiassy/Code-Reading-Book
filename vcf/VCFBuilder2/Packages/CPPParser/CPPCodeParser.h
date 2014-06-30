//CPPCodeParser.h

#ifndef _CPPCODEPARSER_H__
#define _CPPCODEPARSER_H__


#include "CPPParserApplication.h"
#include "CPPParser.h"


namespace VCFBuilder  {

enum CPPClassScope {
	CCS_PRIVATE=0,
	CCS_PROTECTED,
	CCS_PUBLIC
};
class CPPPARSER_API CPPCodeCtx  {
public:
	CPPCodeCtx() {
		m_srcPosition = -1;
		m_srcEndPosition = -1;
		m_linePosition = -1;
	}

	long m_srcPosition;	
	long m_srcEndPosition;
	long m_linePosition;
};


class CPPPARSER_API CPPVisibilityCtx : public CPPCodeCtx {
public:

	VCF::String m_visibiltyName;
};

class CPPPARSER_API CPPArgumentCtx : public CPPCodeCtx{
public:
	CPPArgumentCtx() {
		m_defaultValue = "";
		m_isConst = false;
	}

	VCF::String m_argName;
	VCF::String m_argType;
	VCF::String m_defaultValue;
	bool m_isConst;
};

class CPPPARSER_API CPPMethodCtx : public CPPCodeCtx {
public:
	CPPMethodCtx();

	virtual ~CPPMethodCtx();

	VCF::String m_methodName;
	VCF::String m_returnType;
	bool m_isConst;
	CPPClassScope m_scope;
	bool m_isTemplate;
	VCF::String m_templateTypeName;

	std::vector<CPPArgumentCtx*> m_arguments;

	void addAnArgument( const String& argName, const String& argType, const String& defaultValue, const bool& isConst );
};

class CPPPARSER_API CPPMemberVarCtx : public CPPCodeCtx {
public:
	CPPMemberVarCtx() {
		m_scope = CCS_PRIVATE;
	}

	VCF::String m_memberName;
	VCF::String m_memberType;
	CPPClassScope m_scope;
};

class CPPPARSER_API InheritedClassCtx {
public:
	InheritedClassCtx() {
		m_scope = CCS_PUBLIC;
	}

	VCF::String m_className;
	CPPClassScope m_scope;
};

class CPPPARSER_API CPPClassCtx : public CPPCodeCtx {
public:
	CPPClassCtx();
	
	virtual ~CPPClassCtx();

	std::vector<CPPMethodCtx*> m_methods;
	std::vector<CPPMemberVarCtx*> m_memberVars;
	std::vector<CPPVisibilityCtx*> m_visibilityDecls;
	bool m_isTemplate;
	VCF::String m_templateTypeName;
	VCF::String m_className;
	std::vector<InheritedClassCtx*> m_inheritedClasses;
};

/**
*Class CPPCodeParser documentation
*/
class CPPPARSER_API CPPCodeParser : public VCF::Object, public VCF::SrcParser::CtxVisitor { 
public:
	CPPCodeParser();

	virtual ~CPPCodeParser();

	// method for spVisitor class 
	virtual void VisitClass( VCF::SrcParser::ClassCtx& cl );
	
	virtual void VisitEnumeration( VCF::SrcParser::EnumerationCtx& en );
	
	virtual void VisitTypeDef( VCF::SrcParser::TypeDefCtx& td );
	
	virtual void VisitPreprocessorLine( VCF::SrcParser::PreprocessorLineCtx& pd );
	
	virtual void VisitAttribute( VCF::SrcParser::AttributeCtx& attr );
	
	virtual void VisitOperation( VCF::SrcParser::OperationCtx& op );

	void parse( const VCF::String& fileName );

	void parseCode( const VCF::String& cppCode, const bool& parsingImplementation=false );

	VCF::Enumerator<CPPClassCtx*>* getParsedClasses();

	VCF::Enumerator<CPPMethodCtx*>* getParsedClassMethods();
	
protected:
	void clearClassCollection();
	
	std::vector<CPPClassCtx*> m_parsedClasses;
	VCF::EnumeratorContainer<std::vector<CPPClassCtx*>,CPPClassCtx*> m_parsedClassContainer;

	std::vector<CPPMethodCtx*> m_parsedClassMethods;
	VCF::EnumeratorContainer<std::vector<CPPMethodCtx*>,CPPMethodCtx*> m_parsedClassMethodsContainer;

	VCF::SrcParser::SourceParser   m_parser;
	bool m_parsingImplementation;
private:
};


}; //end of namespace VCFBuilder

#endif //_CPPCODEPARSER_H__


