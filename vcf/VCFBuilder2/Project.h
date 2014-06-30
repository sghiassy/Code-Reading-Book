//Project.h

#ifndef _PROJECT_H__
#define _PROJECT_H__



using namespace VCF;


#define PROJECT_CLASSID		"4490261b-91bd-4331-8c37-04cf6edf7ba2"

#define PROJECT_CONTROL_LISTENER		"ProjectControlListener"

namespace VCFBuilder  {

class ProjectObject;
class Form;
class CPPClass;
class CPPClassInstance;
class CodeGenerator;
class GenericClass;
class ComponentInstance;

#define PROJECT_ITEM_ADDED			CUSTOM_EVENT_TYPES + 3000
#define PROJECT_ITEM_REMOVED		CUSTOM_EVENT_TYPES + 3001
#define PROJECT_ITEM_CHANGED		CUSTOM_EVENT_TYPES + 3002
#define PROJECT_SETTINGS_CHANGED	CUSTOM_EVENT_TYPES + 3003

enum ProjectItemType {
	PIT_CLASS = 0,
	PIT_CLASS_INST,
	PIT_FORM,
	PIT_COMPONENT,
	PIT_PROJECT
};

class ProjectEvent : public Event {
public:
	ProjectEvent( Object* source, ProjectObject* objectThatChanged, const ProjectItemType& itemType, const ulong32& eventType ):
		Event( source, eventType ) {
		m_objectThatChanged = objectThatChanged;
		m_itemType = itemType;
	}
	
	ProjectEvent( Object* source, const ulong32& eventType ):
		Event( source, eventType ) {
		m_objectThatChanged = NULL;
		m_itemType = PIT_PROJECT;
	}

	virtual ~ProjectEvent(){};

	ProjectItemType getItemType() {
		return m_itemType;
	}

	ProjectObject* getObjectThatChanged() {
		return m_objectThatChanged;
	}
protected:
	ProjectObject* m_objectThatChanged;
	ProjectItemType m_itemType;
};


template <class SOURCE_TYPE> class ProjectEventHandler : public EventHandlerInstance<SOURCE_TYPE,ProjectEvent> {
public:
	ProjectEventHandler( SOURCE_TYPE* source,
		EventHandlerInstance<SOURCE_TYPE,ProjectEvent>::OnEventHandlerMethod handlerMethod, 
		const String& handlerName="") :
			EventHandlerInstance<SOURCE_TYPE,ProjectEvent>( source, handlerMethod, handlerName ) {
			
	}
	
	virtual ~ProjectEventHandler(){};
};




/**
*Class Project documentation
*/
class Project : public VCF::Model { 
public:
	BEGIN_ABSTRACT_CLASSINFO(Project, "VCFBuilder::Project", "VCF::Model", PROJECT_CLASSID)
	EVENT( "VCFBuilder::ProjectEventHandler", "VCFBuilder::ProjectEvent", "ProjectChanged" )
	END_CLASSINFO(Project)

	Project( const bool& createAsDefaultProject=true );

	virtual ~Project();

	void init();
	
	EVENT_HANDLER_LIST(ModelEmptied)
	EVENT_HANDLER_LIST(ModelValidate)
	

    virtual void addModelValidationHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelValidate) 
	}
	
	virtual void removeModelValidationHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelValidate) 
	}

	virtual void addModelHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelEmptied) 
	}

	virtual void removeModelHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelEmptied) 
	}

	FIRE_EVENT(ModelValidate,ValidationEvent)

	FIRE_EVENT(ModelEmptied,ModelEvent)

	//ProjectListener
	EVENT_HANDLER_LIST(ProjectChanged)
	ADD_EVENT(ProjectChanged) 
	REMOVE_EVENT(ProjectChanged) 
	FIRE_EVENT(ProjectChanged,ProjectEvent)

	virtual void validate();

    virtual void empty();

	void setPathName( const String& projectPath );

	void setName( const String& name );

	String getPathName() {
		return 	 m_projectPath;
	}

	String getName() {
		return m_projectName;
	}	

	Enumerator<ProjectObject*>* getObjectsInProject();

	virtual void addForm( Form* newForm );

	virtual void removeForm( Form* formToRemove );

	virtual void addClass( CPPClass* newClass );

	virtual void removeClass( CPPClass* classToRemove );

	virtual void addClassInstance( CPPClassInstance* newInstance );

	virtual void removeClassInstance( CPPClassInstance* instanceToRemove );

	/**
	*Projects are responsible for manufacturing the correct type of 
	*GenericClass instance
	*/
	virtual GenericClass* createNewClass( ProjectObject* parent ) = 0;

	/**
	*Projects are responsible for manufacturing the correct type of 
	*Form instance
	*/
	virtual Form* createNewForm( ProjectObject* parent ) = 0;

	Enumerator<Form*>* getForms();

	Enumerator<CPPClass*>* getClasses();

	Enumerator<CPPClassInstance*>* getClassInstances();

	Form* getMainForm() {
		return m_mainForm;
	}

	/**
	*adds a new componetn to a project and returns the newly created component
	*/
	Component* addNewComponent( Class* componentClass, Form* form );

	void addExistingComponent( const String& componentName, Form* form );

	void linkComponentToComponentInstance( Component* component, ComponentInstance* compInstance );

	String getVFFPath() {
		return m_vffPath;		
	}

	void setVFFPath( const String& vffPath ) {
		m_vffPath = vffPath;		
	}

	String getMemberPrefix() {
		return m_memberPrefix;		
	}

	void setMemberPrefix( const String& memberPrefix ) {
		m_memberPrefix = memberPrefix;		
	}

	String getClassNamePrefix() {
		return m_classNamePrefix;		
	}

	void setClassNamePrefix( const String& classNamePrefix ) {
		m_classNamePrefix = classNamePrefix;		
	}

	String getIncludePath() {
		return m_includePath;		
	}

	void setIncludePath( const String& includePath ) {
		m_includePath = includePath;		
	}

	String getSrcPath() {
		return m_srcPath;		
	}

	void setSrcPath( const String& srcPath ) {
		m_srcPath = srcPath;		
	}

	String getResourcePath() {
		return m_resourcePath;		
	}

	void setResourcePath( const String& resourcePath ) {
		m_resourcePath = resourcePath;		
	}

	void removeComponent( Component* component, Form* owningForm );

	void setMainForm( Form* mainForm );	

	CodeGenerator* getCodeGenerator() {
		return m_codeGenerator;
	}

	virtual void intializeSettings() = 0;

protected:
	String m_projectPath;
	String m_projectName;
	String m_vffPath;
	String m_memberPrefix;
	String m_classNamePrefix;
	String m_includePath;
	String m_srcPath;
	String m_resourcePath;

	CodeGenerator* m_codeGenerator;

	std::vector<ProjectObject*> m_projectObjects;

	std::vector<Form*> m_projectForms;
	std::vector<CPPClass*> m_projectClasses;
	std::vector<CPPClassInstance*> m_projectInstances;

	EnumeratorContainer<std::vector<ProjectObject*>, ProjectObject*> m_objContainer;	
	EnumeratorContainer<std::vector<Form*>, Form*> m_formContainer;	
	EnumeratorContainer<std::vector<CPPClass*>, CPPClass*> m_classContainer;	
	EnumeratorContainer<std::vector<CPPClassInstance*>, CPPClassInstance*> m_classInstContainer;
	
	std::map<String,ProjectObject*> m_projectComponents;

	void onControlParentChanged( ControlEvent* e );
	Form* m_mainForm;

	ControlEventHandler<Project>* m_controlHandler;
private:
};


}; //end of namespace VCFBuilder

#endif //_PROJECT_H__


