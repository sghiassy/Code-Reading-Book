// MenuItemPeer.h

#ifndef _MENUITEMPEER_H__
#define _MENUITEMPEER_H__


namespace VCF
{

class MenuItem;

class APPKIT_API MenuItemPeer  
{
public:
	virtual ~MenuItemPeer(){};

	virtual ulong32 getMenuID() = 0;

	virtual void addChild( MenuItem* child ) = 0;

	virtual void insertChild( const unsigned long& index, MenuItem* child ) = 0;
	
	virtual void deleteChild( MenuItem* child ) = 0;

	virtual void deleteChild( const unsigned long& index ) = 0;

	virtual void clearChildren() = 0;

	virtual bool isChecked() = 0;

	virtual void setChecked( const bool& checked ) = 0;

	virtual MenuItem* getParent() = 0;

	virtual bool hasParent() = 0;

	virtual MenuItem* getChildAt( const unsigned long& index ) = 0;

	virtual bool isEnabled() = 0;

	virtual void setEnabled( const bool& enabled ) = 0;

	virtual bool isVisible() = 0;

	virtual void setVisible( const bool& visible ) = 0;

	virtual bool getRadioItem() = 0;

	virtual void setRadioItem( const bool& value ) = 0;

	virtual void setMenuItem( MenuItem* item ) = 0;

	virtual void setCaption( const String& caption ) = 0;

	virtual void setAsSeparator( const bool& isSeperator ) = 0;
};

};

#endif  //_MENUITEMPEER_H__
