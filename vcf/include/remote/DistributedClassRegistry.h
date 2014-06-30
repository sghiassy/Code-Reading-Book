//DistributedClassRegistry.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _DISTRIBUTEDCLASSREGISTRY_H__
#define _DISTRIBUTEDCLASSREGISTRY_H__

#include <map>

using namespace VCF;


namespace VCFRemote  {

class ClassRegistryEntry;


/**
*Class DistributedClassRegistry documentation
*/
class REMOTEKIT_API DistributedClassRegistry : public VCF::Object { 
public:
	DistributedClassRegistry();

	virtual ~DistributedClassRegistry();

	static DistributedClassRegistry* getDistributedClassRegistry();

	/**
	*this is called in the start up of an application or VPL to register
	*the application and it's classes as distributed and available for 
	*remote creation
	*/
	bool registerClass( Class* classToRegister, const VCF::String& serverName, 
		                const VCF::String& userID,
						const bool& inProcessServer, const bool& isServerRemote );

	bool registerClass( const VCF::String& classID,
		                const VCF::String& userID,
						const VCF::String& localServerPath,
						const bool& inProcess,
						const bool& remote,
						const VCF::String& remoteServerPath );

	bool unRegisterClass( const VCF::String& classID );

	VCF::Enumerator<ClassRegistryEntry*>* getRegisteredEntries();

	/**
	*attempts to find a ClassRegistryEntry, assumes the name
	*param is either a valid userID or classID
	*returns NULL if nothing is found, or a valid ClassRegistryEntry pointer
	*if successful
	*/
	ClassRegistryEntry* findEntry( const VCF::String& name );
protected:
	static DistributedClassRegistry distributedClassRegistryInstance;
private:
	VCF::String m_registryEntryFile;
	std::map<VCF::String,ClassRegistryEntry*> m_regEntriesMap;//keyed by classID
	std::map<VCF::String,ClassRegistryEntry*> m_regEntriesMap2;//keyed by userID
	VCF::EnumeratorMapContainer<std::map<VCF::String,ClassRegistryEntry*>,ClassRegistryEntry*> m_regEntriesContainer;
};


}; //end of namespace VCFRemote

#endif //_DISTRIBUTEDCLASSREGISTRY_H__


