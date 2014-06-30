//CPPCodeParser.h
#include "ApplicationKit.h"
#include "CPPCodeParser.h"
#include "FileStream.h"


using namespace VCF;

using namespace VCFBuilder;

using namespace VCF::SrcParser;

CPPMethodCtx::CPPMethodCtx()
{
	m_scope = CCS_PRIVATE;
	m_isConst = false;
	m_isTemplate = false;
}

CPPMethodCtx::~CPPMethodCtx()
{
	std::vector<CPPArgumentCtx*>::iterator it = m_arguments.begin();
	while ( it != m_arguments.end() ) {
		CPPArgumentCtx* arg = *it;
		delete arg;
		arg = NULL;
		it ++;
	}
	m_arguments.clear();
}

void CPPMethodCtx::addAnArgument( const String& argName, const String& argType, const String& defaultValue, const bool& isConst )
{
	CPPArgumentCtx* newArg = new CPPArgumentCtx();
	newArg->m_argName = argName;
	newArg->m_argType = argType;
	newArg->m_defaultValue = defaultValue;
	newArg->m_isConst = isConst;
	m_arguments.push_back( newArg );
}

CPPClassCtx::CPPClassCtx()
{
	m_isTemplate = false;

}
	
CPPClassCtx::~CPPClassCtx()
{
	std::vector<CPPMethodCtx*>::iterator it = m_methods.begin();
	while ( it != m_methods.end() ) {
		CPPMethodCtx* method = *it;
		delete method;
		method = NULL;
		it ++;
	}
	m_methods.clear();

	std::vector<CPPMemberVarCtx*>::iterator it2 = m_memberVars.begin();
	while ( it2 != m_memberVars.end() ) {
		CPPMemberVarCtx* memberVar = *it2;
		delete memberVar;
		memberVar = NULL;
		it2 ++;
	}
	m_memberVars.clear();

	std::vector<InheritedClassCtx*>::iterator it3 = m_inheritedClasses.begin();
	while ( it3 != m_inheritedClasses.end() ) {
		InheritedClassCtx* superClass = *it3;
		delete superClass;
		superClass = NULL;
		it3 ++;
	}
	m_inheritedClasses.clear();

	std::vector<CPPVisibilityCtx*>::iterator it4 = m_visibilityDecls.begin();
	while ( it4 != m_visibilityDecls.end() ) {
		delete *it4;
		it4++;
	}
	m_visibilityDecls.clear();
}

CPPCodeParser::CPPCodeParser()
{
	m_parsedClassContainer.initContainer( this->m_parsedClasses );
	m_parsedClassMethodsContainer.initContainer( m_parsedClassMethods );
	m_parsingImplementation = false;
}

CPPCodeParser::~CPPCodeParser()
{
	clearClassCollection();
}

void CPPCodeParser::VisitClass( ClassCtx& cl )
{
	CPPClassCtx* newClass = new CPPClassCtx();
	m_parsedClasses.push_back( newClass );
	newClass->m_className = cl.GetName();
	newClass->m_srcPosition = cl.m_codePos.m_startPos;
	newClass->m_srcEndPosition = cl.m_codePos.m_endPos;
	newClass->m_linePosition = cl.m_codePos.m_lineNumber;

	StrListT::iterator superClassIt = cl.mSuperClassNames.begin();
	while ( superClassIt != cl.mSuperClassNames.end() ) {
		InheritedClassCtx* superClass = new InheritedClassCtx();
		superClass->m_className = *superClassIt;
		newClass->m_inheritedClasses.push_back( superClass );
		superClassIt ++;
	}	
	

	std::vector<VisibilityCtx*>::iterator visibilityIt = cl.m_visibilityDecls.begin();
	while ( visibilityIt != cl.m_visibilityDecls.end() ) {
		VisibilityCtx* vis = *visibilityIt;

		CPPVisibilityCtx* newVisibilty = new CPPVisibilityCtx();
		newVisibilty->m_linePosition = vis->m_pos.m_lineNumber;	
		newVisibilty->m_srcEndPosition = vis->m_pos.m_endPos;
		newVisibilty->m_srcPosition = vis->m_pos.m_startPos;
		newVisibilty->m_visibiltyName = vis->m_visibility;


		newClass->m_visibilityDecls.push_back( newVisibilty );
		visibilityIt ++;
	}	

	if ( cl.mTemplateTypes.size() > 0 ) {
		newClass->m_isTemplate = true;
		newClass->m_templateTypeName = cl.mTemplateTypes[0];
	}
	MMemberListT& ml = cl.GetMembers();
	MMemberListT::iterator it = ml.begin();
	while ( it != ml.end() ) {
		Context* ctx = *it;
		switch ( ctx->GetContextType() ) {
			case SP_CTX_NAMESPACE : {
					
			}
			break;

			case SP_CTX_CLASS : {
				//nested inner class 
			}
			break;

			case SP_CTX_TYPEDEF : {
				
			}
			break;

			case SP_CTX_PREPROCESSOR : {
				
			}
			break;

			case SP_CTX_ENUMERATION : {
				
			}
			break;

			case SP_CTX_ATTRIBUTE : {
				AttributeCtx* attr = (AttributeCtx*)ctx;

				CPPMemberVarCtx* memberVar = new CPPMemberVarCtx();

				memberVar->m_srcPosition = attr->m_codePos.m_startPos;
				memberVar->m_srcEndPosition = attr->m_codePos.m_endPos;
				memberVar->m_linePosition = attr->m_codePos.m_lineNumber;

				memberVar->m_memberName = attr->GetName();
				memberVar->m_memberType = attr->mType;
				switch( attr->mVisibility ) {
					case SP_VIS_PUBLIC : {
						memberVar->m_scope = CCS_PUBLIC;
					}
					break; 

					case SP_VIS_PROTECTED : {
						memberVar->m_scope = CCS_PROTECTED;
					}
					break; 

					case SP_VIS_PRIVATE : {
						memberVar->m_scope = CCS_PRIVATE;
					}
					break; 
				}

				newClass->m_memberVars.push_back( memberVar );
			}
			break;

			case SP_CTX_OPERATION : {
				OperationCtx* spMethod = (OperationCtx*)ctx;

				CPPMethodCtx* method = new CPPMethodCtx();
				method->m_srcPosition = spMethod->m_codePos.m_startPos;
				method->m_srcEndPosition = spMethod->m_codePos.m_endPos;
				method->m_linePosition = spMethod->m_codePos.m_lineNumber;

				method->m_methodName = ctx->GetName();
				method->m_returnType = spMethod->mRetType;
				method->m_isConst = spMethod->mIsConstant;

				switch( spMethod->mVisibility ) {
					case SP_VIS_PUBLIC : {
						method->m_scope = CCS_PUBLIC;
					}
					break; 

					case SP_VIS_PROTECTED : {
						method->m_scope = CCS_PROTECTED;
					}
					break; 

					case SP_VIS_PRIVATE : {
						method->m_scope = CCS_PRIVATE;
					}
					break; 
				}
				
				MMemberListT::iterator argIt = spMethod->GetMembers().begin();
				while ( argIt != spMethod->GetMembers().end() ) {
					ParameterCtx* param = (ParameterCtx*)*argIt;
					CPPArgumentCtx* methodArg = new CPPArgumentCtx();
					
					methodArg->m_srcPosition = param->m_codePos.m_startPos;
					methodArg->m_srcEndPosition = param->m_codePos.m_endPos;
					methodArg->m_linePosition = param->m_codePos.m_lineNumber;
					
					methodArg->m_argName = param->GetName();
					methodArg->m_argType = param->mType;
					methodArg->m_defaultValue = param->mInitVal;

					method->m_arguments.push_back( methodArg );
					
					argIt++;
				}

				newClass->m_methods.push_back( method );
			}
			break;

			case SP_CTX_PARAMETER : {
				
			}
			break;
		}
		
		it ++;
	}
}

void CPPCodeParser::VisitEnumeration( EnumerationCtx& en )
{

}

void CPPCodeParser::VisitTypeDef( TypeDefCtx& td )
{

}

void CPPCodeParser::VisitPreprocessorLine( PreprocessorLineCtx& pd )
{

}

void CPPCodeParser::VisitAttribute( AttributeCtx& attr )
{

}

void CPPCodeParser::VisitOperation( OperationCtx& op )
{
	if ( true == m_parsingImplementation ) {	
		
		CPPMethodCtx* method = new CPPMethodCtx();
		method->m_srcPosition = op.m_codePos.m_startPos;
		method->m_srcEndPosition = op.m_codePos.m_endPos;
		method->m_linePosition = op.m_codePos.m_lineNumber;
		
		method->m_methodName = op.GetName();
		method->m_returnType = op.mRetType;
		method->m_isConst = op.mIsConstant;
		
		switch( op.mVisibility ) {
			case SP_VIS_PUBLIC : {
				method->m_scope = CCS_PUBLIC;
			}
			break; 
			
			case SP_VIS_PROTECTED : {
				method->m_scope = CCS_PROTECTED;
			}
			break; 
			
			case SP_VIS_PRIVATE : {
				method->m_scope = CCS_PRIVATE;
			}
			break; 
		}
		
		MMemberListT::iterator argIt = op.GetMembers().begin();
		while ( argIt != op.GetMembers().end() ) {
			ParameterCtx* param = (ParameterCtx*)*argIt;
			CPPArgumentCtx* methodArg = new CPPArgumentCtx();
			
			methodArg->m_srcPosition = param->m_codePos.m_startPos;
			methodArg->m_srcEndPosition = param->m_codePos.m_endPos;
			methodArg->m_linePosition = param->m_codePos.m_lineNumber;
			
			methodArg->m_argName = param->GetName();
			methodArg->m_argType = param->mType;
			methodArg->m_defaultValue = param->mInitVal;
			
			method->m_arguments.push_back( methodArg );
			
			argIt++;
		}
		
		this->m_parsedClassMethods.push_back( method ); 
	}
}

void CPPCodeParser::parse( const String& fileName )
{
	clearClassCollection();

	if ( fileName.empty() ) {
		return;
	}

	FileStream fs( fileName, FS_READ );
	String s;
	fs >> s;

	parseCode( s );
}

void CPPCodeParser::parseCode( const VCF::String& cppCode, const bool& parsingImplementation )
{
	clearClassCollection();

	m_parsingImplementation = parsingImplementation;

	if ( true == cppCode.empty() ) {
		return;
	}	
	
	char* tmp = new char[cppCode.size()+1];
	memset( tmp, 0, cppCode.size()+1 );
	cppCode.copy( tmp, cppCode.size() );

	FileCtx* fileCtx = m_parser.ParseAll( tmp, tmp + cppCode.size() );

	VisitAll( *fileCtx, 1 );

	delete [] tmp;

	delete fileCtx;
	m_parsingImplementation = false;
}


Enumerator<CPPClassCtx*>* CPPCodeParser::getParsedClasses()
{
	return m_parsedClassContainer.getEnumerator();
}

void CPPCodeParser::clearClassCollection()
{
	std::vector<CPPClassCtx*>::iterator it = m_parsedClasses.begin();
	while ( it != m_parsedClasses.end() ) {
		CPPClassCtx* cppClass = *it;
		delete cppClass;
		cppClass = NULL;
		it++;
	}
	m_parsedClasses.clear();


	std::vector<CPPMethodCtx*>::iterator it2 = m_parsedClassMethods.begin();
	while ( it2 != m_parsedClassMethods.end() ) {
		CPPMethodCtx* cppMethod = *it2;
		delete cppMethod;
		cppMethod = NULL;
		it2++;
	}

	m_parsedClassMethods.clear();
}

VCF::Enumerator<CPPMethodCtx*>* CPPCodeParser::getParsedClassMethods()
{
	return m_parsedClassMethodsContainer.getEnumerator();
}