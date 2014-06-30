//MSRDC.h

#ifndef MSRDC_MSRDC_H__
#define MSRDC_MSRDC_H__
#include "FoundationKit.h"



using namespace VCF;

namespace MSRDC  {

#define MSRDC_MSRDC_CLASSID 		"43478D73-78E0-11CF-8E78-00A0D100038E"
/**
*class MSRDC
*UUID: 43478D73-78E0-11CF-8E78-00A0D100038E
*ProgID: 
*/
class MSRDC : public VCF::Object, public IRdc, public DRdcEvents {
public:
	BEGIN_CLASSINFO( MSRDC, "MSRDC::MSRDC", "VCF::Object", MSRDC_MSRDC_CLASSID );
	END_CLASSINFO( MSRDC );

	MSRDC();

	virtual ~MSRDC();

	virtual VCF::String getDataSourceName( );//[id(1)]

	virtual void setDataSourceName( VCF::String Val );//[id(1)]

	virtual VCF::String getSQL( );//[id(2)]

	virtual void setSQL( VCF::String Val );//[id(2)]

	virtual long getLoginTimeout( );//[id(3)]

	virtual void setLoginTimeout( long Val );//[id(3)]

	virtual VCF::String getVersion( );//[id(4)]

	virtual VCF::String getConnect( );//[id(5)]

	virtual void setConnect( VCF::String Val );//[id(5)]

	virtual long getQueryTimeout( );//[id(6)]

	virtual void setQueryTimeout( long Val );//[id(6)]

	virtual long getRowsetSize( );//[id(7)]

	virtual void setRowsetSize( long Val );//[id(7)]

	virtual ResultsetTypeConstants getResultsetType( );//[id(8)]

	virtual void setResultsetType( ResultsetTypeConstants Val );//[id(8)]

	virtual long getMaxRows( );//[id(9)]

	virtual void setMaxRows( long Val );//[id(9)]

	virtual VCF::String getLogMessages( );//[id(10)]

	virtual void setLogMessages( VCF::String Val );//[id(10)]

	virtual long getKeysetSize( );//[id(11)]

	virtual void setKeysetSize( long Val );//[id(11)]

	virtual short getEditMode( );//[id(12)]

	virtual void setEditMode( short Val );//[id(12)]

	virtual bool getEnabled( );//[id(-514)]

	virtual void setEnabled( bool Val );//[id(-514)]

	virtual _rdoResultset* getResultset( );//[id(13)]

	virtual void setResultset( _rdoResultset* Val );//[id(13)]

	virtual bool getReadOnly( );//[id(14)]

	virtual void setReadOnly( bool Val );//[id(14)]

	virtual short getOptions( );//[id(15)]

	virtual void setOptions( short Val );//[id(15)]

	virtual VCF::String getUserName( );//[id(16)]

	virtual void setUserName( VCF::String Val );//[id(16)]

	virtual VCF::String getPassword( );//[id(17)]

	virtual void setPassword( VCF::String Val );//[id(17)]

	virtual CursorDriverConstants getCursorDriver( );//[id(18)]

	virtual void setCursorDriver( CursorDriverConstants Val );//[id(18)]

	virtual IFontDisp* getFont( );//[id(-512)]

	virtual void setFont( IFontDisp* Val );//[id(-512)]

	virtual _rdoConnection* getConnection( );//[id(19)]

	virtual void setConnection( _rdoConnection* Val );//[id(19)]

	virtual EOFActionConstants getEOFAction( );//[id(20)]

	virtual void setEOFAction( EOFActionConstants Val );//[id(20)]

	virtual BOFActionConstants getBOFAction( );//[id(21)]

	virtual void setBOFAction( BOFActionConstants Val );//[id(21)]

	virtual long getErrorThreshold( );//[id(22)]

	virtual void setErrorThreshold( long Val );//[id(22)]

	virtual LockTypeConstants getLockType( );//[id(23)]

	virtual void setLockType( LockTypeConstants Val );//[id(23)]

	virtual PromptConstants getPrompt( );//[id(24)]

	virtual void setPrompt( PromptConstants Val );//[id(24)]

	virtual bool getTransactions( );//[id(25)]

	virtual AppearanceConstants getAppearance( );//[id(-520)]

	virtual void setAppearance( AppearanceConstants Val );//[id(-520)]

	virtual _rdoEnvironment* getEnvironment( );//[id(27)]

	virtual void setEnvironment( _rdoEnvironment* Val );//[id(27)]

	virtual bool getStillExecuting( );//[id(28)]

	virtual VCF::String getCaption( );//[id(-518)]

	virtual void setCaption( VCF::String Val );//[id(-518)]

	virtual OLE_COLOR getBackColor( );//[id(-501)]

	virtual void setBackColor( OLE_COLOR Val );//[id(-501)]

	virtual OLE_COLOR getForeColor( );//[id(-513)]

	virtual void setForeColor( OLE_COLOR Val );//[id(-513)]

	virtual long getBatchCollisionCount( );//[id(29)]

	virtual VCF::VariantData getBatchCollisionRows( );//[id(30)]

	virtual long getBatchSize( );//[id(31)]

	virtual void setBatchSize( long Val );//[id(31)]

	virtual short getUpdateOperation( );//[id(32)]

	virtual void setUpdateOperation( short Val );//[id(32)]

	virtual short getUpdateCriteria( );//[id(33)]

	virtual void setUpdateCriteria( short Val );//[id(33)]

	virtual void updateControls( );//[id(35)]

	virtual void updateRow( );//[id(36)]

	virtual void beginTrans( );//[id(37)]

	virtual void commitTrans( );//[id(38)]

	virtual void rollbackTrans( );//[id(39)]

	virtual void cancel( );//[id(40)]

	virtual void refresh( );//[id(-550)]

	virtual void aboutBox( );//[id(-552)]

	virtual void validate( );//[id(0)]

	virtual void reposition( );//[id(1)]

	virtual void error( long Number, long Scode, VCF::String Source, VCF::String HelpFile, long HelpContext );//[id(2)]

	virtual void queryCompleted( );//[id(3)]

	virtual void mouseDown( );//[id(-605)]

	virtual void mouseMove( );//[id(-606)]

	virtual void mouseUp( );//[id(-607)]

};

}  //end of namespace MSRDC

#endif //Adding method 6 of 7 for interface DRdcEvents

