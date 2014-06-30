//CommandGroup.h

#ifndef _COMMANDGROUP_H__
#define _COMMANDGROUP_H__




namespace VCF  {

/**
*Class CommandGroup documentation
*/
class APPKIT_API CommandGroup : public AbstractCommand { 
public:
	CommandGroup();

	virtual ~CommandGroup();

	virtual void undo();

	virtual void redo();

	virtual void execute();

	void addCommand( Command* command ) {
		m_commands.push_back( command );
	}
protected:
	std::vector<Command*> m_commands;
private:
};


}; //end of namespace VCF

#endif //_COMMANDGROUP_H__


