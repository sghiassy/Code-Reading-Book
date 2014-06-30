//MSRDC.h

#ifndef _MSRDCINTERFACES_H__
#define _MSRDCINTERFACES_H__

#include "FoundationKit.h"



using namespace VCF;

namespace MSRDC  {

/**
*Interface DRdcEvents
*UUID: 43478D72-78E0-11CF-8E78-00A0D100038E
*/
class DRdcEvents : public VCF::Interface { 
public:
	virtual ~DRdcEvents(){};

	virtual void validate( ) = 0;//[id(0)]

	virtual void reposition( ) = 0;//[id(1)]

	virtual void error( long Number, long Scode, VCF::String Source, VCF::String HelpFile, long HelpContext ) = 0;//[id(2)]

	virtual void queryCompleted( ) = 0;//[id(3)]

	virtual void mouseDown( ) = 0;//[id(-605)]

	virtual void mouseMove( ) = 0;//[id(-606)]

	virtual void mouseUp( ) = 0;//[id(-607)]

};


/**
*Interface IRdc
*UUID: 43478D71-78E0-11CF-8E78-00A0D100038E
*/
class IRdc : public VCF::Interface { 
public:
	virtual ~IRdc(){};

	virtual VCF::String getDataSourceName( ) = 0;//[id(1)]

	virtual void setDataSourceName( VCF::String Val ) = 0;//[id(1)]

	virtual VCF::String getSQL( ) = 0;//[id(2)]

	virtual void setSQL( VCF::String Val ) = 0;//[id(2)]

	virtual long getLoginTimeout( ) = 0;//[id(3)]

	virtual void setLoginTimeout( long Val ) = 0;//[id(3)]

	virtual VCF::String getVersion( ) = 0;//[id(4)]

	virtual VCF::String getConnect( ) = 0;//[id(5)]

	virtual void setConnect( VCF::String Val ) = 0;//[id(5)]

	virtual long getQueryTimeout( ) = 0;//[id(6)]

	virtual void setQueryTimeout( long Val ) = 0;//[id(6)]

	virtual long getRowsetSize( ) = 0;//[id(7)]

	virtual void setRowsetSize( long Val ) = 0;//[id(7)]

	virtual ResultsetTypeConstants getResultsetType( ) = 0;//[id(8)]

	virtual void setResultsetType( ResultsetTypeConstants Val ) = 0;//[id(8)]

	virtual long getMaxRows( ) = 0;//[id(9)]

	virtual void setMaxRows( long Val ) = 0;//[id(9)]

	virtual VCF::String getLogMessages( ) = 0;//[id(10)]

	virtual void setLogMessages( VCF::String Val ) = 0;//[id(10)]

	virtual long getKeysetSize( ) = 0;//[id(11)]

	virtual void setKeysetSize( long Val ) = 0;//[id(11)]

	virtual short getEditMode( ) = 0;//[id(12)]

	virtual void setEditMode( short Val ) = 0;//[id(12)]

	virtual bool getEnabled( ) = 0;//[id(-514)]

	virtual void setEnabled( bool Val ) = 0;//[id(-514)]

	virtual _rdoResultset* getResultset( ) = 0;//[id(13)]

	virtual void setResultset( _rdoResultset* Val ) = 0;//[id(13)]

	virtual bool getReadOnly( ) = 0;//[id(14)]

	virtual void setReadOnly( bool Val ) = 0;//[id(14)]

	virtual short getOptions( ) = 0;//[id(15)]

	virtual void setOptions( short Val ) = 0;//[id(15)]

	virtual VCF::String getUserName( ) = 0;//[id(16)]

	virtual void setUserName( VCF::String Val ) = 0;//[id(16)]

	virtual VCF::String getPassword( ) = 0;//[id(17)]

	virtual void setPassword( VCF::String Val ) = 0;//[id(17)]

	virtual CursorDriverConstants getCursorDriver( ) = 0;//[id(18)]

	virtual void setCursorDriver( CursorDriverConstants Val ) = 0;//[id(18)]

	virtual IFontDisp* getFont( ) = 0;//[id(-512)]

	virtual void setFont( IFontDisp* Val ) = 0;//[id(-512)]

	virtual _rdoConnection* getConnection( ) = 0;//[id(19)]

	virtual void setConnection( _rdoConnection* Val ) = 0;//[id(19)]

	virtual EOFActionConstants getEOFAction( ) = 0;//[id(20)]

	virtual void setEOFAction( EOFActionConstants Val ) = 0;//[id(20)]

	virtual BOFActionConstants getBOFAction( ) = 0;//[id(21)]

	virtual void setBOFAction( BOFActionConstants Val ) = 0;//[id(21)]

	virtual long getErrorThreshold( ) = 0;//[id(22)]

	virtual void setErrorThreshold( long Val ) = 0;//[id(22)]

	virtual LockTypeConstants getLockType( ) = 0;//[id(23)]

	virtual void setLockType( LockTypeConstants Val ) = 0;//[id(23)]

	virtual PromptConstants getPrompt( ) = 0;//[id(24)]

	virtual void setPrompt( PromptConstants Val ) = 0;//[id(24)]

	virtual bool getTransactions( ) = 0;//[id(25)]

	virtual AppearanceConstants getAppearance( ) = 0;//[id(-520)]

	virtual void setAppearance( AppearanceConstants Val ) = 0;//[id(-520)]

	virtual _rdoEnvironment* getEnvironment( ) = 0;//[id(27)]

	virtual void setEnvironment( _rdoEnvironment* Val ) = 0;//[id(27)]

	virtual bool getStillExecuting( ) = 0;//[id(28)]

	virtual VCF::String getCaption( ) = 0;//[id(-518)]

	virtual void setCaption( VCF::String Val ) = 0;//[id(-518)]

	virtual OLE_COLOR getBackColor( ) = 0;//[id(-501)]

	virtual void setBackColor( OLE_COLOR Val ) = 0;//[id(-501)]

	virtual OLE_COLOR getForeColor( ) = 0;//[id(-513)]

	virtual void setForeColor( OLE_COLOR Val ) = 0;//[id(-513)]

	virtual long getBatchCollisionCount( ) = 0;//[id(29)]

	virtual VCF::VariantData getBatchCollisionRows( ) = 0;//[id(30)]

	virtual long getBatchSize( ) = 0;//[id(31)]

	virtual void setBatchSize( long Val ) = 0;//[id(31)]

	virtual short getUpdateOperation( ) = 0;//[id(32)]

	virtual void setUpdateOperation( short Val ) = 0;//[id(32)]

	virtual short getUpdateCriteria( ) = 0;//[id(33)]

	virtual void setUpdateCriteria( short Val ) = 0;//[id(33)]

	virtual void updateControls( ) = 0;//[id(35)]

	virtual void updateRow( ) = 0;//[id(36)]

	virtual void beginTrans( ) = 0;//[id(37)]

	virtual void commitTrans( ) = 0;//[id(38)]

	virtual void rollbackTrans( ) = 0;//[id(39)]

	virtual void cancel( ) = 0;//[id(40)]

	virtual void refresh( ) = 0;//[id(-550)]

	virtual void aboutBox( ) = 0;//[id(-552)]

};


}  //end of namespace MSRDC

#endif //_MSRDCINTERFACES_H__

