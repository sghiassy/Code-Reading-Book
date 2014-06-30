//$$root$$.h

#ifndef $$ROOT_$$_H__
#define $$ROOT_$$_H__


#ifdef WIN32
/**
*this is only here for microsoft VPL implementation
*/
	#ifdef $$ROOT_$$_EXPORTS
	#define $$ROOT_$$_API __declspec(dllexport)
	#else
	#define $$ROOT_$$_API __declspec(dllimport)
	#endif //$$ROOT_$$_EXPORTS
#else
	#define $$ROOT_$$_API
#endif //WIN32


/**
*This is the registration entry point for the VPL. initPackage() gets called
*by the VCF Builder when dynamically loading the library. It should also be 
*called by the application using components from the VPL. This should be done
*automatically for you by the code generation capabilities of the VCF Builder,
*but just in case it is left out, here's what to do:

//put the following code in your Application derived class's initRunningApplication()
//method, where vplFilename is a string that represents the fully qualified filename
//that points to your VPL.
	this->loadVPL( vplFilename );


*Any classes you have that have declared RTTI info, need to register themselves with
*the ClassRegistry. The easiest way is through using the REGISTER_CLASSINFO macro. To
*use, do the following for each class you need to register:
	//className represents the class you want to register
	REGISTER_CLASSINFO(className)
*The REGISTER_CLASSINFO macro does require that your RTTI was declared using the 
*ClassInfo macros. For more information please see the VCF documentation, and/or
*look at the comments in the Class.h and ClassInfo.h headers.
*Also, components and/or PropertyEditor registration should be done here as well.
*Please see the documentation on the Component::registerComponent() method and the
*PropertyEditorManager::registerPropertyEditor() method
*/
extern "C" $$ROOT_$$_API void initPackage();


#endif //$$FILE_NAME_SYMBOL$$
