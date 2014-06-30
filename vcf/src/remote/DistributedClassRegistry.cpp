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

#include "RemoteObjectKit.h"
#include "DistributedClassRegistry.h"
#include "ClassRegistryEntry.h"

using namespace VCF;

using namespace VCFRemote;

#define  DISTRIB_CLASS_REG_DB		 "Software\\VCF\\Remote"
#define  DISTRIB_CLASS_DB_FILE		 "DistribClassRegDB"

DistributedClassRegistry DistributedClassRegistry::distributedClassRegistryInstance;


DistributedClassRegistry* DistributedClassRegistry::getDistributedClassRegistry()
{
	return &DistributedClassRegistry::distributedClassRegistryInstance;
}

DistributedClassRegistry::DistributedClassRegistry()
{
	m_registryEntryFile = "";
	
	m_regEntriesContainer.initContainer( m_regEntriesMap );

	Registry reg;
	reg.setRoot( RKT_LOCAL_MACHINE );
	if ( true == reg.openKey( DISTRIB_CLASS_REG_DB, false ) ) {
		m_registryEntryFile = reg.getStringValue( DISTRIB_CLASS_DB_FILE );
		if ( m_registryEntryFile.empty() ) {
			//throw exception - class registry db file not found !!!!!	
		}
	}
	else {
		//throw exception - class registry db file not found !!!!!
	}
	
	FileStream fs( m_registryEntryFile, FS_READ_WRITE );
	int entryCount = 0;
	fs >> entryCount;
	for ( int i=0;i<entryCount;i++) {
		ClassRegistryEntry* entry = new ClassRegistryEntry();
		fs >> entry;
		m_regEntriesMap[ entry->getClassID() ] = entry;		
		m_regEntriesMap2[ entry->getUserID() ] = entry;
	} 
}

DistributedClassRegistry::~DistributedClassRegistry()
{	
	bool okToWriteRegEntries = !(m_registryEntryFile.empty());
	OutputStream* os = NULL;
	FileStream* fs = NULL;
	if ( true == okToWriteRegEntries ) {
		fs = new FileStream( m_registryEntryFile, FS_READ_WRITE );
		int entryCount = m_regEntriesMap.size();
		(*fs) << entryCount;
		//os = dynamic_cast<OutputStream*>( &fs );
	}
	std::map<String,ClassRegistryEntry*>::iterator it = m_regEntriesMap.begin();
	while ( it != m_regEntriesMap.end() ) {
		ClassRegistryEntry* entry = it->second;
		if ( true == okToWriteRegEntries ) {
			 (*fs) << entry;
		}
		delete entry;
		entry = NULL;
		it++;
	}
	if ( true == okToWriteRegEntries ) {
		delete fs;
		fs = NULL;
	}
	m_regEntriesMap.clear();
	m_regEntriesMap2.clear();
}

bool DistributedClassRegistry::registerClass( Class* classToRegister, const VCF::String& serverName, 
												const VCF::String& userID,
												const bool& inProcessServer, 
												const bool& isServerRemote )
{
	bool result = false;
	if ( NULL == classToRegister ) {
		return result;
	}

	
	return result;
}

bool DistributedClassRegistry::registerClass( const VCF::String& classID,
												const VCF::String& userID,
												const VCF::String& localServerPath,
												const bool& inProcess,
												const bool& remote,
												const VCF::String& remoteServerPath )
{
	bool result = false;

	ClassRegistryEntry* entry = 
		new ClassRegistryEntry(  classID, userID, localServerPath, inProcess, remote, remoteServerPath );
	
	m_regEntriesMap[classID] = entry;
	m_regEntriesMap2[userID] = entry;
	result = true;
	return result;
}

VCF::Enumerator<ClassRegistryEntry*>* DistributedClassRegistry::getRegisteredEntries()
{
	return m_regEntriesContainer.getEnumerator();
}

ClassRegistryEntry* DistributedClassRegistry::findEntry( const VCF::String& name )
{
	ClassRegistryEntry* result = NULL;
	/*
	*searches the classID map first, then the userID map
	*could a smarter way, perhaps read the first few charaters
	*and check the form to see if it a valid UUID ?
	*/
	std::map<VCF::String,ClassRegistryEntry*>::iterator found = m_regEntriesMap.find( name );
	if ( found != m_regEntriesMap.end() ) {
		result = found->second;
	}
	else {
		found = m_regEntriesMap2.find( name );
		if ( found != m_regEntriesMap2.end() ){
			result = found->second;
		}
	}
	return result;
}

bool DistributedClassRegistry::unRegisterClass( const VCF::String& classID )
{
	bool result = false;
	std::map<VCF::String,ClassRegistryEntry*>::iterator found = m_regEntriesMap.find( classID );
	if ( found != m_regEntriesMap.end() ) {
		ClassRegistryEntry* entry = found->second;
		delete entry;
		m_regEntriesMap.erase( found );
		entry = NULL;
		result = true;
	}
	return result;
}