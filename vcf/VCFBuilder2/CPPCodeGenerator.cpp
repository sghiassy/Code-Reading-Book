//CPPCodeGenerator.h
#include "ApplicationKit.h"
#include "CodeGenerator.h"
#include "CPPCodeGenerator.h"
#include "Form.h"
#include "VFFInputStream.h"
#include "ComponentInstance.h"
#include "Project.h"
#include "ComponentInfo.h"

using namespace VCF;

using namespace VCFBuilder;


CPPCodeGenerator::CPPCodeGenerator()
{
	m_owningForm = NULL;
	m_owningClass = NULL;
	m_vcfInitMethodName = VCFINIT_METHOD_NAME;
	m_newClassIsDerivedFromFrame = false;
}

CPPCodeGenerator::~CPPCodeGenerator()
{
	m_owningForm = NULL;
	m_owningClass = NULL;
}	

void CPPCodeGenerator::addForm( Form* newForm )
{
	//initialize the form

	//add a new class as well
	GenericClass* formClass = newForm->getFormClass();
	String formSuperClassName = "";
	String formSuperClassID = "";
	FileStream vffFS( newForm->getVFFFileName(), FS_READ );		
	VFFInputStream vffIS( &vffFS );

	vffIS.getOuterClassNameAndUUID( formSuperClassName, formSuperClassID );
	
	formClass->setSuperClassName( formSuperClassName );	

	m_newClassIsDerivedFromFrame = true;

	if ( NULL != formClass ) {
		addClass( formClass );
	}

	initializeFormEventsFromSource();

	m_newClassIsDerivedFromFrame = false;
}

void CPPCodeGenerator::setOwningClass( GenericClass* owningClass )
{
	m_owningClass = owningClass;
	m_owningForm = NULL;
}

void CPPCodeGenerator::setOwningForm( Form* owningForm )
{
	m_owningClass = NULL;
	m_owningForm = owningForm;
	if ( NULL != m_owningForm ) {
		m_owningClass = m_owningForm->getFormClass();
	}
}

void CPPCodeGenerator::addComponent( Component* newComponent )
{
	//update the class declaration
	addNewComponentToClassDecl( newComponent );

	addNewComponentToClassImpl( newComponent );
}

void CPPCodeGenerator::removeComponent( Component* component )
{
	removeComponentFromClassDecl( component );
	
	removeComponentFromClassImpl( component );
}

void CPPCodeGenerator::addEvent( const String& eventHandlerCallback, Component* source, EventProperty* eventProperty )
{
	
	String codeSnippet = generateEventHandlerCode( eventHandlerCallback, source, eventProperty ); 	
	StringUtils::trace( codeSnippet + "\n" );

	//add the code to the C++ implementation
	initializeClassImplContents( m_owningForm->getFormClass() );

	long eventInsertLocation = getEventHandlerInsertLocation(source);

	insertEventHandlerCode( codeSnippet, eventInsertLocation );

	//add the code to the C++ class decl
	this->initializeClassDeclContents( m_owningForm->getFormClass() );

	CPPMethodCtx newMethod;
	newMethod.m_methodName = eventHandlerCallback;
	newMethod.m_returnType = "void";
	newMethod.m_isConst = false;
	newMethod.addAnArgument( "event", eventProperty->getEventClassName() + "*", "", false );	
	
	addMethodDeclToClass( m_owningForm->getFormClass(), &newMethod );

	addMethodImplToClass( m_owningForm->getFormClass(), &newMethod );
}

void CPPCodeGenerator::removeEvent( const String& eventHandlerCallback, Component* source, EventProperty* eventProperty )
{
	initializeClassImplContents( m_owningForm->getFormClass() );

	long eventInsertLocation = getEventHandlerInsertLocation( source );
	//C++ impl already parsed in getEventHandlerInsertLocation()
	std::vector<CPPMethodCallStatement> methods;
	
	parseVCFInitMethod( methods );

	std::vector<CPPMethodCallStatement>::iterator it = methods.begin();	
	String fullCallback = m_owningForm->getName() + "::" + eventHandlerCallback;
	String addHandler = "add" + eventProperty->getEventMethodName() + "Handler";

	while ( it != methods.end() ) {
		CPPMethodCallStatement& cms = *it;

		if ( cms.m_methodArgNames.size() > 1 ) {
			if ( (cms.m_methodArgNames[1] == fullCallback) && (addHandler == cms.m_methodName) ) {

				m_classImplContents.erase( cms.m_startPos, cms.m_endPos - cms.m_startPos );

				writeClassImplToFile( m_owningForm->getFormClass() );

				break;
			}
		}
		it++;
	}
	
}

bool CPPCodeGenerator::isA( const String& className, const String& classNameToMatch )
{
	bool result = false;
	ClassRegistry* classReg = ClassRegistry::getClassRegistry();
	Class* clazz = classReg->getClass( className );
	while ( NULL != clazz ) {
		Class* superClass = clazz->getSuperClass();
		if ( NULL != superClass ) {
			if ( classNameToMatch == superClass->getClassName() ) {
				result = true;
				break;
			}
		}
		clazz = superClass;
	}
	return result;
}

void CPPCodeGenerator::addClass( GenericClass* newClass )
{
	m_classImplContents = "";
	m_classDeclContents = "";

	initializeClassImplContents( newClass );
	if ( true == m_classImplContents.empty() ) {
		//need to create from scratch
		m_classImplContents = createInitialCPPImpl( newClass );

		m_classImplContents += createDefaultClassImpl( newClass );

		if ( true == m_classImplContents.empty() ) {
			String error = "Code Generator failed to generate for requested class \"" + newClass->getName() + "\"";
			throw RuntimeException( MAKE_ERROR_MSG_2(error) );
		}
		
		writeClassImplToFile( newClass );
	}
	else {
		
	}	

	initializeClassDeclContents( newClass );

	if ( true == m_classDeclContents.empty() ) {
		//need to create from scratch
		long insertPos = -1;
		String tmp = createInitialCPPHeader( newClass, insertPos );
		if ( -1 != insertPos ) {
			m_classDeclContents = tmp;
			long pos;
			tmp = createDefaultClassDecl( newClass, pos );
			
			m_classDeclContents.insert( insertPos, tmp ); 
		}
	}
	else {
		//parse the header for member variables
		this->m_codeParser.parseCode( m_classDeclContents );
		Enumerator<CPPClassCtx*>* parsedClasses = m_codeParser.getParsedClasses();
		CPPClassCtx* foundParsedClass = NULL;
		while ( true == parsedClasses->hasMoreElements() ) {
			CPPClassCtx* aClass = parsedClasses->nextElement();
			if ( newClass->getName() ==  aClass->m_className ) {
				foundParsedClass = aClass;
				break;
			}
		}

		if ( NULL != foundParsedClass ) {
			std::vector<CPPMemberVarCtx*>::iterator it = foundParsedClass->m_memberVars.begin();
			while ( it != foundParsedClass->m_memberVars.end() ) {
				CPPMemberVarCtx* memberVar = *it;
				String componentClassName = "VCF::Component";
				String className = memberVar->m_memberType;
				int pos = className.find("*");
				if ( pos != String::npos ) {
					className.erase( pos, 1 );
				}
				pos = className.find(" ");
				while ( pos != String::npos ) {
					className.erase( pos, 1 );
					pos = className.find(" ", pos);
				}

				if ( isA( className, componentClassName ) ) {

					m_owningForm->getOwningProject()->addExistingComponent( memberVar->m_memberName, m_owningForm );
					
										
				}
				it ++;
			}
		}
	}	
	

	if ( true == m_classDeclContents.empty() ) {
		String error = "Code Generator failed to generate for requested class \"" + newClass->getName() + "\"";
		throw RuntimeException( MAKE_ERROR_MSG_2(error) );
	}
	
	writeClassDeclToFile( newClass );
}

void CPPCodeGenerator::addClassInstance( CPPClassInstance* classInstance )
{

}

void CPPCodeGenerator::changeFormName( const String& newName, const String& oldName )
{
	//change the class name here as well

	m_owningClass = m_owningForm->getFormClass();
	
	
}

void CPPCodeGenerator::changeClassName( const String& newName, const String& oldName )
{
	if ( NULL == m_owningClass ) {
		//error ! 
		throw RuntimeException( MAKE_ERROR_MSG_2("No class specified to perform this operation on.") );
	}
	
	initializeClassDeclContents( m_owningClass );
	int pos = m_classDeclContents.find( oldName );
	while ( pos != String::npos ) {
		m_classDeclContents.erase( pos, oldName.size() );
		m_classDeclContents.insert( pos, newName );
		pos = m_classDeclContents.find( oldName, pos+1 );
	}

	//try and fix the include guards and CLSID as well
	String upperCaseOldName = StringUtils::upperCase( oldName );
	String upperCaseNewName = StringUtils::upperCase( newName );
	pos = m_classDeclContents.find( upperCaseOldName );
	while ( pos != String::npos ) {
		m_classDeclContents.erase( pos, upperCaseOldName.size() );
		m_classDeclContents.insert( pos, upperCaseNewName );
		
		pos = m_classDeclContents.find( upperCaseOldName, pos+1 );
	}

	writeClassDeclToFile( m_owningClass );

	
	initializeClassImplContents( m_owningClass );
	pos = m_classImplContents.find( oldName );
	while ( pos != String::npos ) {
		m_classImplContents.erase( pos, oldName.size() );
		m_classImplContents.insert( pos, newName );
		
		pos = m_classImplContents.find( oldName, pos+1 );
	}
	
	writeClassImplToFile( m_owningClass );
}

void CPPCodeGenerator::changeMemberName( const String& newName, const String& oldName )
{
	if ( NULL != m_owningForm ) {
		initializeClassDeclContents( m_owningForm->getFormClass() );

		int pos = m_classDeclContents.find( oldName );
		while ( pos != String::npos ) {
			m_classDeclContents.erase( pos, oldName.size() );
			m_classDeclContents.insert( pos, newName );

			pos = m_classDeclContents.find( oldName, pos+1 );
		}
		writeClassDeclToFile( m_owningForm->getFormClass() );



		initializeClassImplContents( m_owningForm->getFormClass() );
		pos = m_classImplContents.find( oldName );
		while ( pos != String::npos ) {
			m_classImplContents.erase( pos, oldName.size() );
			m_classImplContents.insert( pos, newName );

			pos = m_classImplContents.find( oldName, pos+1 );
		}

		writeClassImplToFile( m_owningForm->getFormClass() );

	}
	else if ( NULL != this->m_owningClass ) {

	}
	else {
		//not being used correctly - error condition, throw exception	
	}
}

String CPPCodeGenerator::generateEventMethodName(  Component* listener, EventProperty* eventProperty )
{
	String result = listener->getName() + "_" + eventProperty->getEventMethodName();
	return result;
}

long CPPCodeGenerator::getMemberVariableInsertLocation()
{
	return -1;
}

long CPPCodeGenerator::getEventHandlerInsertLocation( Component* component )
{
	if ( true == m_classImplContents.empty() ) {
		throw RuntimeException( MAKE_ERROR_MSG_2("Code Generator has no source contents to search! CPPCodeGenerator::getEventHandlerInsertLocation() - did initializeClassImplContents() get called ?") );
	}

	long result = -1;
	
	m_codeParser.parseCode( m_classImplContents, true );

	String vcfInitMethodName = 	m_owningForm->getFormClass()->getName() + "::vcfInit";

	Enumerator<CPPMethodCtx*>* methods = m_codeParser.getParsedClassMethods();
	CPPMethodCtx* vcfInitMethod = NULL;
	while ( true == methods->hasMoreElements() ) {
		CPPMethodCtx* method = methods->nextElement();
		if ( method->m_methodName == vcfInitMethodName ) {
			vcfInitMethod = method;
			break;
		}
	}

	if ( NULL != vcfInitMethod ) {
		result = vcfInitMethod->m_srcEndPosition-2;
	}

	
	return result;
}

void CPPCodeGenerator::insertEventHandlerCode( const String& eventCode, long insertPos )
{
	if ( insertPos < 0 ) {
		throw RuntimeException( MAKE_ERROR_MSG_2("Inavlid insert position for CPPCodeGenerator::insertEventHandlerCode()") );
	}	

	if ( insertPos < m_classImplContents.size() ) {
		
		m_classImplContents.insert( insertPos, eventCode );

		writeClassImplToFile( m_owningForm->getFormClass() );		
	}
}

String CPPCodeGenerator::getMemberNameFromComponent( Component* component )
{
	String result;

	result = component->getName();

	if ( result == this->m_owningForm->getName() ) {
		result = "this";
	}

	return result;
}

String CPPCodeGenerator::generateMemberVariableNameFromComponent( Component* component )
{
	String result;

	result = component->getName();

	return result;
}

String CPPCodeGenerator::generateEventHandlerCode( const String& eventHandlerCallback, Component* component, EventProperty* eventProperty )
{
	String result;
	
	String memberName = getMemberNameFromComponent( component );

	String formClassName = m_owningForm->getFormClass()->getName();

	result = "\n\t" + memberName + "->add" + eventProperty->getEventMethodName() + "Handler( \n";
	result += "\t\t\t new " + eventProperty->getHandlerClassName() + "<" + formClassName + ">( this, " + 
			formClassName + "::" + eventHandlerCallback + ", \"" + memberName + "_" + eventProperty->getEventMethodName() + "Handler\" ) );\n";

	return result;
}

void CPPCodeGenerator::initializeClassImplContents( GenericClass* aClass )
{
	m_classImplContents = "";
	try {
		FileStream classImplFS( aClass->getImplFileName(), FS_READ );		
		classImplFS >> m_classImplContents;
	}
	catch ( BasicFileError& e ) {
		StringUtils::trace( "File (" + aClass->getImplFileName() + ")not present - creating a new one...\n" );	
	}
}

void CPPCodeGenerator::initializeClassDeclContents( GenericClass* aClass )
{
	m_classDeclContents = "";
	try {
		FileStream classImplFS( aClass->getDeclFileName(), FS_READ );
		classImplFS >> m_classDeclContents;
	}
	catch ( BasicFileError& e ) {
		StringUtils::trace( "File (" + aClass->getDeclFileName() + ")not present - creating a new one...\n" );	
	}
}

String CPPCodeGenerator::createInitialCPPHeader( GenericClass* aClass, long& insertPos ) 
{
	insertPos = -1;
	String result;
	String className = aClass->getName();
	result += "//" + className + ".h\n\n";
	String includeGuard = "_" + StringUtils::upperCase( className ) + "_H__";
	result += "#ifndef "+ includeGuard + "\n";
	result += "#define " + includeGuard + "\n\n\n";
	
	//no namespaces yet ?

	insertPos = result.size();

	result += "\n\n#endif //_" + includeGuard + "\n\n";

	return result;
}

String CPPCodeGenerator::createDefaultClassDecl( GenericClass* aClass, long& insertPos )
{
	String result;

	String className = aClass->getName();
	result += "class " + className;
	String superClassName = aClass->getSuperClassName();
	if ( false == superClassName.empty() ) {
		result += " : public " + superClassName;
	}
	
	result += " { \n";
	result += "public:\n";
	result += "\t" + className + "();\n\n";
	result += "\tvirtual ~" + className + "();\n\n";
	
	if ( true == m_newClassIsDerivedFromFrame ) {
		result += this->generateVCFInitMethodDecl( aClass );
	}

	insertPos = result.size();

	result += "protected:\n\n";

	result += "};";

	return result;
}

String CPPCodeGenerator::createInitialCPPImpl( GenericClass* aClass )
{
	String result;
	
	result += "//" + aClass->getName() + ".cpp\n\n";
	result += "#include \"ApplicationKit.h\"\n";
	result += "#include \"" + aClass->getName() + ".h\"\n\n";
	result += "using namespace VCF;\n\n\n";

	return result;
}

String CPPCodeGenerator::createDefaultClassImpl( GenericClass* aClass )
{
	String result;

	String className = aClass->getName();

	result += className + "::" + className + "()\n";
	result += "{\n";
	result += "\n";
	result += "}\n\n\n";

	result += className + "::~" + className + "()\n";
	result += "{\n";
	result += "\n";
	result += "}\n\n";

	if ( true == m_newClassIsDerivedFromFrame ) {
		result += generateVCFInitMethodImpl( aClass );
	}

	return result;
}

String CPPCodeGenerator::generateVCFInitMethodDecl( GenericClass* aClass )
{
	String result;

	result += "\tvoid " + m_vcfInitMethodName + "();\n\n";	

	return result;
}
	
String CPPCodeGenerator::generateVCFInitMethodImpl( GenericClass* aClass )
{
	String result;

	result += "void " + aClass->getName() + "::vcfInit()\n{\n\n}\n\n";
	return result;
}

bool CPPCodeGenerator::hasDeclMethodName( const String& method )
{
	bool result = false;

	result = m_classDeclContents.find( method ) != String::npos;

	return result;
}

bool CPPCodeGenerator::hasImplMethodName( const String& method )
{
	bool result = false;

	result = m_classImplContents.find( method ) != String::npos;

	return result;
}

long CPPCodeGenerator::getDeclMethodInsertionPoint( GenericClass* aClass )
{
	long result = -1;

	String className = aClass->getName();
	int pos = m_classDeclContents.find( "class " );
	
	if ( pos != String::npos ) {
		pos = m_classDeclContents.find( className, pos );
		if ( pos != String::npos ) {
			
		}
	}

	return result;
}

void CPPCodeGenerator::addNewComponentToClassDecl( Component* newComponent )
{
	//update the class decl
	GenericClass* formClass = m_owningForm->getFormClass();

	initializeClassDeclContents( formClass );
	
	if ( true == m_classDeclContents.empty() ) {
		//throw some kind of exception 
	}

	//find insert pos
	m_codeParser.parseCode( m_classDeclContents );
	Enumerator<CPPClassCtx*>* parsedClasses = m_codeParser.getParsedClasses();
	CPPClassCtx* foundParsedClass = NULL;
	while ( true == parsedClasses->hasMoreElements() ) {
		CPPClassCtx* aClass = parsedClasses->nextElement();
		if ( formClass->getName() ==  aClass->m_className ) {
			foundParsedClass = aClass;
			break;
		}
	}
	
	//check to see if we found the right class
	if ( NULL != foundParsedClass ) {	
		//look for the last "protected" section - if it is not there then 
		//add it

		//foundParsedClass->m_srcEndPosition		

		CPPVisibilityCtx* lastProtectedVis = NULL;
		std::vector<CPPVisibilityCtx*>::iterator it = foundParsedClass->m_visibilityDecls.begin();
		while ( it != foundParsedClass->m_visibilityDecls.end() ) {
			CPPVisibilityCtx* vis = *it;
			if ( (vis->m_visibiltyName == "protected:") || (vis->m_visibiltyName == "private:") ) {
				lastProtectedVis = vis;
			}
			it++;
		}

		int insertPos = String::npos;
		//OK either we have found the last protected decl, or...
		if ( NULL != lastProtectedVis ) {
			insertPos = lastProtectedVis->m_srcEndPosition;
		}
		else { //we have not, in which case we have to add one 
			insertPos = foundParsedClass->m_srcEndPosition - 1;
			String protectedStr = "\nprotected:";
			m_classDeclContents.insert( insertPos, protectedStr );
			insertPos += protectedStr.size();
		}

		if ( insertPos != String::npos ) {
			String memberVar = "\n\t" + newComponent->getClassName() + "* " + newComponent->getName() + ";\n";

			m_classDeclContents.insert( insertPos, memberVar );

			//check for forward declarations - for now this is hacky 
			//later we'll add support for correctly parsing them

			String componentFwdDecl = "\nclass " + newComponent->getClassName() + ";\n";
			int fwdDeclPos = m_classDeclContents.find( componentFwdDecl );
			if ( fwdDeclPos == String::npos ) {
				const VCFChar* tmp = m_classDeclContents.begin() + foundParsedClass->m_srcPosition;
				const VCFChar* start = m_classDeclContents.begin();
				while ( (tmp > start) && (*tmp != '\n') ) {
					tmp --;
				}

				m_classDeclContents.insert( (tmp - start), componentFwdDecl );
			}

			writeClassDeclToFile( formClass );
		}
	}
}

void CPPCodeGenerator::addNewComponentToClassImpl( Component* newComponent )
{
	GenericClass* formClass = m_owningForm->getFormClass();

	initializeClassImplContents( formClass );
	
	if ( true == m_classImplContents.empty() ) {
		//throw some kind of exception 
	}
	
	m_codeParser.parseCode( m_classImplContents, true );	

	String vcfInitMethodName = 	m_owningForm->getFormClass()->getName() + "::vcfInit";

	Enumerator<CPPMethodCtx*>* methods = m_codeParser.getParsedClassMethods();
	CPPMethodCtx* vcfInitMethod = NULL;

	while ( true == methods->hasMoreElements() ) {
		CPPMethodCtx* method = methods->nextElement();
		if ( method->m_methodName == vcfInitMethodName ) {
			vcfInitMethod = method;
			break;
		}
	}

	if ( NULL != vcfInitMethod ) {

		int pos = vcfInitMethod->m_srcEndPosition - 2;
		String bindMethodStatement;
		bindMethodStatement += "\n\tthis->bindVariable( &" + newComponent->getName() + ", \"" + 
									newComponent->getName() + "\" );\n";

		m_classImplContents.insert( pos, bindMethodStatement );


		//find last #include
		String includeStr = "#include";
		int includePos = String::npos;
		pos = m_classImplContents.find( includeStr );
		while ( String::npos != pos ) {
			includePos = pos;	
			pos = m_classImplContents.find( includeStr, pos+1 );
		}
		if ( String::npos != includePos ) {
			const VCFChar* start = m_classImplContents.begin();
			const VCFChar* tmp = m_classImplContents.begin() + includePos;
			int size = m_classImplContents.size();
			while ( (*tmp != '\n') && ((tmp-start) < size) ) {
				includePos ++;
				tmp ++;
			}
			
			ComponentInfo* componentInfo = UIToolkit::getDefaultUIToolkit()->getComponentInfo( newComponent->getClass() );
			
			if ( NULL != componentInfo ) {
				Enumerator<String>* componentHeaders = componentInfo->getRequiredHeaders();
				includeStr = "";
				while ( true == componentHeaders->hasMoreElements() ) {
					String header = componentHeaders->nextElement();
					includeStr += "\n#include \"" + header + "\"\n";
				}
				
				//look to see if it is already present
				if ( String::npos == m_classImplContents.find( includeStr ) ) {
					m_classImplContents.insert( includePos+1, includeStr );	
				}
			}
			else { //put a warning comment in the code to look up this header
				includeStr = "\n//warning: unable to determine correct header to use with " + newComponent->getClassName() + "\n";
				if ( String::npos == m_classImplContents.find( includeStr ) ) {					
					m_classImplContents.insert( includePos+1, includeStr );		
				}
			}
		}

		writeClassImplToFile( formClass );
	}
	
}

void CPPCodeGenerator::removeComponentFromClassDecl( Component* component )
{
	//update the class decl
	GenericClass* formClass = m_owningForm->getFormClass();

	initializeClassDeclContents( formClass );
	
	if ( true == m_classDeclContents.empty() ) {
		//throw some kind of exception 
	}	
	//find removal pos
	m_codeParser.parseCode( m_classDeclContents );
	Enumerator<CPPClassCtx*>* parsedClasses = m_codeParser.getParsedClasses();
	CPPClassCtx* foundParsedClass = NULL;
	while ( true == parsedClasses->hasMoreElements() ) {
		CPPClassCtx* aClass = parsedClasses->nextElement();
		if ( formClass->getName() ==  aClass->m_className ) {
			foundParsedClass = aClass;
			break;
		}
	}
	
	//check to see if we found the right class
	if ( NULL != foundParsedClass ) {	
		std::vector<CPPMemberVarCtx*>::iterator it = foundParsedClass->m_memberVars.begin();
		while ( it != foundParsedClass->m_memberVars.end() ) {
			CPPMemberVarCtx* memberVar = *it;
			
			if ( memberVar->m_memberName == component->getName() ) {
				//HA! we have found the match ! - so lets remove it
				m_classDeclContents.erase( memberVar->m_srcPosition, (memberVar->m_srcEndPosition - memberVar->m_srcPosition)+2 );
				writeClassDeclToFile( formClass );

				break;
			}
			it ++;
		}
	}
}

void CPPCodeGenerator::removeComponentFromClassImpl( Component* component )
{
	GenericClass* formClass = m_owningForm->getFormClass();

	initializeClassImplContents( formClass );
	
	if ( true == m_classImplContents.empty() ) {
		//throw some kind of exception 
	}	
	//find removal pos
	m_codeParser.parseCode( m_classImplContents, true );
	
	String vcfInitMethodName = 	m_owningForm->getFormClass()->getName() + "::vcfInit";

	Enumerator<CPPMethodCtx*>* methods = m_codeParser.getParsedClassMethods();

	CPPMethodCtx* vcfInitMethod = NULL;

	while ( true == methods->hasMoreElements() ) {
		CPPMethodCtx* method = methods->nextElement();
		if ( method->m_methodName == vcfInitMethodName ) {
			vcfInitMethod = method;
			break;
		}
	}

	if ( NULL != vcfInitMethod ) {
		int pos = vcfInitMethod->m_srcEndPosition - 2;
		
		
		std::vector<CPPMethodCallStatement> methodStatements;
		
		parseVCFInitMethod( methodStatements );
		
		std::vector<CPPMethodCallStatement>::iterator it = methodStatements.begin();	
		String bindMethod = "bindVariable";		
		
		while ( it != methodStatements.end() ) {
			CPPMethodCallStatement& cms = *it;
			
			if ( cms.m_methodArgNames.size() > 1 ) {
				String arg1 = "&" + component->getName();
				if ( (cms.m_methodArgNames[0] == arg1) && (bindMethod == cms.m_methodName) ) {
					
					m_classImplContents.erase( cms.m_startPos, cms.m_endPos - cms.m_startPos );
					
					writeClassImplToFile( formClass );
					
					break;
				}
			}
			it++;
		}
	}
	
}

long CPPCodeGenerator::getVisibilityInsertPos( CPPClassCtx* classCtx, const String& visibility )
{
	long result = String::npos;

	CPPVisibilityCtx* lastVis = NULL;
	std::vector<CPPVisibilityCtx*>::iterator it = classCtx->m_visibilityDecls.begin();
	while ( it != classCtx->m_visibilityDecls.end() ) {
		CPPVisibilityCtx* vis = *it;
		if ( vis->m_visibiltyName == visibility ) {
			lastVis = vis;
		}
		it++;
	}
	
	
	//OK either we have found the last vis or...
	if ( NULL != lastVis ) {
		result = lastVis->m_srcEndPosition;
	}

	return result;
}

void CPPCodeGenerator::addMethodDeclToClass( GenericClass* aClass, CPPMethodCtx* newMethod ) 
{
	initializeClassDeclContents( aClass );
	if ( true == m_classDeclContents.empty() ) {
		//throw some kind of exception 
	}	
	m_codeParser.parseCode( m_classDeclContents );
	//does the method exist ?

	Enumerator<CPPClassCtx*>* parsedClasses = m_codeParser.getParsedClasses();
	CPPClassCtx* foundParsedClass = NULL;
	while ( true == parsedClasses->hasMoreElements() ) {
		CPPClassCtx* aClassCtx = parsedClasses->nextElement();
		if ( aClass->getName() ==  aClassCtx->m_className ) {
			foundParsedClass = aClassCtx;
			break;
		}
	}
	
	//check to see if we found the right class
	bool isMethodPresent = false;

	if ( NULL != foundParsedClass ) {
		std::vector<CPPMethodCtx*>::iterator it = foundParsedClass->m_methods.begin();
		while ( it != foundParsedClass->m_methods.end() ) {
			CPPMethodCtx* method = *it;
			if ( method->m_methodName == newMethod->m_methodName ) {
				isMethodPresent = true;
				break;
			}
			it ++;
		}
		

		if ( false == isMethodPresent ) {
			
			long insertPos = getVisibilityInsertPos( foundParsedClass, "protected:" );
			if ( insertPos != String::npos ) {
				insertPos ++; //scooch it past the carriage return ...
			}
			if ( insertPos == String::npos ) {
				insertPos = foundParsedClass->m_srcEndPosition - 1;
				String publicVis = "\nprotected:\n";
				m_classDeclContents.insert( insertPos, publicVis );
				insertPos += publicVis.size();
			}

			if ( insertPos != String::npos ) {
				String methodDecl = generateMethod( aClass, newMethod, false );
				
				m_classDeclContents.insert( insertPos, methodDecl );
				
				writeClassDeclToFile( aClass );
			}
		}
	}
	
}

void CPPCodeGenerator::addMethodImplToClass( GenericClass* aClass, CPPMethodCtx* newMethod )
{
	initializeClassImplContents( aClass );

	if ( true == m_classImplContents.empty() ) {
		//throw some kind of exception 
	}	
	m_codeParser.parseCode( m_classImplContents, true );

	bool isMethodPresent = false;

	//does the method exist ?
	Enumerator<CPPMethodCtx*>* methods = m_codeParser.getParsedClassMethods();
	CPPMethodCtx* foundMethod = NULL;
	
	String newMethodName = aClass->getName() + "::" + newMethod->m_methodName;

	while ( true == methods->hasMoreElements() ) {
		CPPMethodCtx* method = methods->nextElement();
		if ( newMethodName ==  method->m_methodName ) {
			
			isMethodPresent = true;
			break;
		}
	}	
	

	if ( false == isMethodPresent ) {
		String methodImpl = generateMethod( aClass, newMethod, true );
		m_classImplContents += methodImpl;
		writeClassImplToFile( aClass );
	}
}

void CPPCodeGenerator::writeClassDeclToFile( GenericClass* aClass )
{
	FileStream fs( aClass->getDeclFileName(), FS_WRITE );
	fs << m_classDeclContents;
}

void CPPCodeGenerator::writeClassImplToFile( GenericClass* aClass )
{
	FileStream fs( aClass->getImplFileName(), FS_WRITE );
	fs << m_classImplContents;	
}

String CPPCodeGenerator::generateMethod( GenericClass* aClass, CPPMethodCtx* newMethod, const bool& declareAsImplementation )
{
	String result = "\n";
	if ( false == declareAsImplementation ) {
		result += "\t";
	}
	result += newMethod->m_returnType + " ";

	if ( true == declareAsImplementation ) {
		result += aClass->getName() + "::";
	}

	result += newMethod->m_methodName + "(";

	std::vector<CPPArgumentCtx*>::iterator argIt = newMethod->m_arguments.begin();
	while ( argIt != newMethod->m_arguments.end() ) {
		if ( argIt == newMethod->m_arguments.begin() ) {
			result += " ";
		}
		CPPArgumentCtx* arg = *argIt;
		if ( arg->m_isConst ) {
			result += "const ";	
		}
		result += arg->m_argType + " " + arg->m_argName;
		argIt++;
		if ( argIt != newMethod->m_arguments.end() ) {
			result += ", ";
		}
		else {
			result += " ";
		}
	}
	result += ")";
	if ( newMethod->m_isConst ) {
		result += " const ";
	}

	if ( true == declareAsImplementation ) {
		result += "\n{\n\n}\n\n";
	}
	else {
		result += ";\n";
	}
	
	return result;
}


/*
class MethodParser  {
public:

	const String::iterator skipWhiteSpace( const String::iterator p ) {
		while ( (*p == ' ') || (*p=='\n') || (*p=='\r') || (*p=='\t') ) {
			p++;
		}
		return p;
	}


	bool isCharID( const VCFChar& c ) {
		return ((c >= 'a') && (c <= 'z')) || (((c >= 'A') && (c <= 'Z'))) || (((c >= '0') && (c <= '9'))) || (c == '_') || (c == ':') || (c == '&') || (c == '*')|| (c == '.') || (c == '"');
	}

	void doMethod( const String::iterator p ) {
		p = skipWhiteSpace( p );
		
		const String::iterator start  = p;

		const String::iterator end = p;
		while ( *end != ';' ) {
			end ++;
		}
		

		
	}
};
*/


/**
*Note: 
*This method is EXTREMELY fragile ! it has the responisibillity of actaully attempting to parse 
*the code in the vcfInit() method and determine where the various method statements are
*so that other methods can remove sections if need be, or do other things with the information
*that is retrieved here. What we would like is an actual C++ statement parser, but this is quite
*beyond the scope for now !
*/
void CPPCodeGenerator::parseVCFInitMethod( std::vector<CPPMethodCallStatement>& methodStatements )
{
	String vcfInitMethodName = 	m_owningForm->getFormClass()->getName() + "::vcfInit";

	Enumerator<CPPMethodCtx*>* methods = m_codeParser.getParsedClassMethods();
	CPPMethodCtx* vcfInitMethod = NULL;
	while ( true == methods->hasMoreElements() ) {
		CPPMethodCtx* method = methods->nextElement();
		if ( method->m_methodName == vcfInitMethodName ) {
			vcfInitMethod = method;
			break;
		}
	}

	if ( NULL != vcfInitMethod ) {
		long offset = vcfInitMethod->m_srcPosition;

		String vcfInit = m_classImplContents.substr( vcfInitMethod->m_srcPosition, 
													vcfInitMethod->m_srcEndPosition - vcfInitMethod->m_srcPosition );
		
		
		
		String::iterator p = NULL;		
		String::iterator p2 = NULL;

		int pos = vcfInit.find( "->" );
		int erased = 0;
		while ( (pos != String::npos) && (vcfInit.size() > 0) ) {
			
			p = vcfInit.begin() + pos;
			while ( (*p != ' ') && (*p != '\t')  && (*p != '\n')  ) {
				p--;
			}
			int pos2 = p - vcfInit.begin()+1;
			int lineEnd = String::npos;

			CPPMethodCallStatement cms;
			cms.m_startPos = pos2 + offset;


			cms.m_variable = vcfInit.substr( pos2, pos-pos2 );
			cms.m_specifier = "->";						

			pos2 = pos+2;
			pos = vcfInit.find( "(", pos );

			lineEnd = vcfInit.find( ";", pos );

			cms.m_methodName = vcfInit.substr( pos2, pos-pos2);			
			
			
			int tmpPos = vcfInit.find( "(", pos+1 ); //find the next open paren
			
			if ( (tmpPos != String::npos) && (tmpPos < lineEnd ))  {
				pos = tmpPos;
				pos2 = pos+1;
				pos = vcfInit.find( ")", pos ); //find the next close paren	
			}
			else {
				pos2 = pos+1;
			}

			
			
			//read the args
			String arg1;
			String arg2;
			String arg3;
			
			p = vcfInit.begin() + pos2;
			while ( (*p == ' ') || (*p == '\t') || (*p == '\n')  ) {
				p++;
			}

			p2 = p;
			while ( ((*p >= 'a') && (*p <= 'z')) || (((*p >= 'A') && (*p <= 'Z'))) || (((*p >= '0') && (*p <= '9'))) || (*p == '_') || (*p == ':') || (*p == '&') || (*p == '*')|| (*p == '.') || (*p == '"') ) {
				p++;
			}
			
			arg1 = vcfInit.substr( p2 - vcfInit.begin(), p - p2 );

			if ( *p == ',' ) {
				p++;
			}

			pos2 = p - vcfInit.begin();


			p = vcfInit.begin() + pos2;
			while ( (*p == ' ') || (*p == '\t') || (*p == '\n')  ) {
				p++;
			}

			p2 = p;
			while ( ((*p >= 'a') && (*p <= 'z')) || (((*p >= 'A') && (*p <= 'Z'))) || (((*p >= '0') && (*p <= '9'))) || (*p == '_') || (*p == ':') || (*p == '&') || (*p == '*') || (*p == '.') || (*p == '"') ) {
				p++;
			}
			
			arg2 = vcfInit.substr( p2 - vcfInit.begin(), p - p2 );

			if ( *p == ',' ) {
				p++;
			}

			pos2 = p - vcfInit.begin();

			pos = vcfInit.find( ";", pos );
			vcfInit.erase( 0, pos );
			
			cms.m_endPos = pos + 1 + offset;
			cms.m_methodArgNames.push_back( arg1 );
			cms.m_methodArgNames.push_back( arg2 );
			
			offset += pos;

			methodStatements.push_back( cms );

			pos = vcfInit.find( "->" );
		}
	}	
}

void CPPCodeGenerator::initializeFormEventsFromSource()
{	
	initializeClassImplContents( m_owningForm->getFormClass() );

	m_codeParser.parseCode( this->m_classImplContents, true );

	std::vector<CPPMethodCallStatement> methodStatements;

	parseVCFInitMethod( methodStatements );

	std::vector<CPPMethodCallStatement>::iterator it = 	methodStatements.begin();
	String componentName;
	String eventHandlerCallback;
	while ( it != methodStatements.end() ) {
		eventHandlerCallback = "";
		CPPMethodCallStatement& cms = *it;
		String tmp = cms.m_methodName;
		componentName = cms.m_variable;
		if ( (true == componentName.empty()) || (componentName == "this") ) {
			componentName = m_owningForm->getName();	
		}
		int pos = tmp.find( "add" );
		int pos2 = tmp.find( "Handler" );	
		if ( (pos != String::npos) && (pos2 != String::npos) ) {
			tmp.erase( pos, 3 );
			pos2 -= 3;
			tmp.erase( pos2, 7 );
			if ( cms.m_methodArgNames.size() > 1 ) {
				eventHandlerCallback = cms.m_methodArgNames[1];
				pos = eventHandlerCallback.find( "::" );
				if ( pos != String::npos ) {
					eventHandlerCallback.erase( 0, pos + 2 );

					m_owningForm->addComponentEventHandler( componentName, tmp, eventHandlerCallback );
				}
			}			
		}

		it ++;
	}

}
