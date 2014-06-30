//Browser.cpp


#include "ApplicationKit.h"
#include "TreeControl.h"
#include "TreeModel.h"
#include "DefaultTreeItem.h"
#include "CPPParser.h"
#include "CPPCodeParser.h"

using namespace VCF;
using namespace VCFBuilder;


class BrowserWindow : public Window {
public:
	BrowserWindow() {
		setCaption( "Hello World !!!!" );
		setVisible( true );

		TreeControl* tc = new TreeControl();
		add( tc, ALIGN_CLIENT );
		tc->setVisible( true );
		TreeModel* tm = tc->getTreeModel();
	
		CPPCodeParser parser;
		String fileName = "D:\\code\\vcf\\include\\core\\Property.h";//"D:\\code\\vcf\\VCFBuilder2\\Packages\\CPPParser\\Browser\\Test.h";
		parser.parse( fileName );

		Enumerator<CPPClassCtx*>* classes = parser.getParsedClasses();
		while ( classes->hasMoreElements() ) {
			CPPClassCtx* clazz = classes->nextElement();
			String className = clazz->m_className;
			if ( clazz->m_isTemplate ) {
				className += " <class " + clazz->m_templateTypeName + ">";
			}
			TreeItem* item = new DefaultTreeItem( className );
			
			tm->addNodeItem( item );
			std::vector<CPPMemberVarCtx*>::iterator varIt = clazz->m_memberVars.begin();
			while ( varIt !=  clazz->m_memberVars.end() ) {
				CPPMemberVarCtx* var = *varIt;
				TreeItem* varItem = new DefaultTreeItem( var->m_memberName );
				tm->addNodeItem( varItem, item );
				varIt++;
			}
			std::vector<CPPMethodCtx*>::iterator methIt = clazz->m_methods.begin();
			while ( methIt !=  clazz->m_methods.end() ) {
				CPPMethodCtx* var = *methIt;
				String name = var->m_methodName;
				name += "(";
				std::vector<CPPArgumentCtx*>::iterator argIt = var->m_arguments.begin();
				while ( argIt != var->m_arguments.end() ) {
					CPPArgumentCtx* arg = *argIt;
					name += arg->m_argType + " " + arg->m_argName;
					if ( !arg->m_defaultValue.empty() ) {
						name += " = " + arg->m_defaultValue;
					}
					argIt++;
					if ( argIt != var->m_arguments.end() ) {
						name += ", ";
					}
				}
				name += ")";
				TreeItem* varItem = new DefaultTreeItem( name );
				tm->addNodeItem( varItem, item );
				methIt++;
			}
		}
	}

	virtual ~BrowserWindow(){};

};




class BrowserApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new BrowserWindow();
		setMainWindow(mainWindow);
		mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );

		return result;
	}

};


int main(int argc, char *argv[])
{
	BrowserApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

