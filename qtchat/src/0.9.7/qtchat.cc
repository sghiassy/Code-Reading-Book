#include <qapplication.h>
#include <qobject.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mainwidget.h"
#include <signal.h>
#include "chatter.h"

int debuglevel = 	// global var for debug messages
	// default flags to watch
	DEBUGLEVEL_NETWORK |
	DEBUGLEVEL_NOTICE |
	DEBUGLEVEL_WARNING |
	DEBUGLEVEL_ERROR | 
	DEBUGLEVEL_FATAL /*|
	DEBUGLEVEL_CONSTRUCTOR |
	DEBUGLEVEL_DESTRUCTOR*/;


class NetscapePID_URL {
	public:
		NetscapePID_URL() {
			pid=0;
			url=0;
		}
		NetscapePID_URL(pid_t p,const char *u) {
			if(u) url=strdup(u);
			else url=0;
			pid=p;
		}
		NetscapePID_URL(const NetscapePID_URL &n) {
			url=0;
			operator=(n);
		}
		~NetscapePID_URL() {
			if(url) free(const_cast<char *>(url));
		}
		pid_t getPID() const { return pid; }
		const char *getURL() const { return url; }

		void setPID(pid_t p) { pid = p; }
		void setURL(const char *u) {
			if(url) free(const_cast<char *>(url));
			if(u) url=strdup(u);
			else url=0;
		}
		void setPID_URL(pid_t p, const char *u) {
			setPID(p); setURL(u);
		}
		NetscapePID_URL& operator=(const NetscapePID_URL &n) {
			setPID(n.getPID());
			setURL(n.getURL());
			return *this;
		}

	private:
		pid_t pid;
		const char *url;
};

const static int MAX_NETSCAPE_PIDS  = 64;
NetscapePID_URL netscapePIDs[MAX_NETSCAPE_PIDS];
int numNetscapePIDs=0;

int addNetscapePID(pid_t pid, const char *url) {
	if(numNetscapePIDs < MAX_NETSCAPE_PIDS) {
		netscapePIDs[numNetscapePIDs++].setPID_URL(pid, url);
		return 0;
	} else
		return -1;
}

const char *getNetscapePID_URL(pid_t pid) {
	for(int i=0; i<numNetscapePIDs; i++)
		if(netscapePIDs[i].getPID()==pid)
			return netscapePIDs[i].getURL();
	return 0;
}

int removeNetscapePID(pid_t pid) {
	for(int i=0; i<numNetscapePIDs; i++)
		if(netscapePIDs[i].getPID()==pid) {
			netscapePIDs[i] = netscapePIDs[numNetscapePIDs-1];
			numNetscapePIDs--;
			return 0;
		}
	return -1;
}

void exec_die_handler(int sig) {
	int status=0;
	int pid = wait(&status);
	const char *url;
	cerr << "Forked program exited with status " << WEXITSTATUS(status) << '.' << endl;
	if((url=getNetscapePID_URL(pid))!=0 && WEXITSTATUS(status)!=0) {
		// unsuccessful attempt made to run 'netscape -remote'
		char *cp=strdup(url);	// get a modifiable copy of string
		::shell("netscape", cp);	// reissue netscape command w/o '-remote'
		free(cp);	// free string copy
		removeNetscapePID(pid);
	}
}

int main(int argc, char *argv[]) {
	struct stat sb;

	// init the data
	numNetscapePIDs=0;

	// init the GUI toolkit
	QApplication app(argc, argv);
	QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

	// get/create ~/.qtchat directory
	char path[PATH_MAX];
	char qtchat_ini_path[PATH_MAX];
	char chatters_ini_path[PATH_MAX];
	const char *cp;
	if((cp=getenv("HOME"))==NULL) cp = ".";
	strncpy(path, cp, PATH_MAX);
	strncat(path, "/.qtchat", PATH_MAX);
	int stat_val;
	if((stat_val=stat(path, &sb)) || !(sb.st_mode & S_IFDIR)) {
		if(!stat_val) {	// ~/.qtchat exists but not a directory
			cerr << argv[0] << ": ~/.qtchat exists but is not a directory." << endl;
			exit(1);
		}
		// ~/.qtchat doesn't exist
		cerr << argv[0] << ": Creating ~/.qtchat directory...." << endl;
		if(mkdir(path, 0700)) {
			cerr << argv[0] << ": Unable to create ~/.qtchat directory." << endl;
		}
	}

	strncpy(chatters_ini_path, path, PATH_MAX);
	strncat(chatters_ini_path, "/chatters.ini", PATH_MAX);
	strncpy(qtchat_ini_path, path, PATH_MAX);
	strncat(qtchat_ini_path, "/qtchat.ini", PATH_MAX);

	// init chatters
	ifstream chatters_ifs(chatters_ini_path);
	if(chatters_ifs) {
		Chatter::loadChatters(chatters_ifs);
		chatters_ifs.close();
	} else {
		cerr << argv[0] << ": Unable to open \"" << path << "\" for reading." << endl;
		cerr << argv[0] << ": Chatter states not restored." << endl;
	}

	// prepare to init QtChat state
	ifstream qtchat_ifs(qtchat_ini_path);
	if(qtchat_ifs) {
	} else {
		cerr << argv[0] << ": Unable to open \"" << qtchat_ini_path << "\" for reading." << endl;
		cerr << argv[0] << ": QtChat state not restored." << endl;
	}


	// init main widget
	MainWidget *mainwidget = new MainWidget::MainWidget();
	if(qtchat_ifs) {
		cerr << "Restoring MainWidget state..." << flush;
		try {
			mainwidget->restoreState(qtchat_ifs);
		} catch (...) {
			cerr << endl;
			cerr << "main(): Unable to restore state... possibly new config file version." << endl;
		}
		qtchat_ifs.close();
		cerr << "done." << endl;
	}
	mainwidget->resize(600, 400);
	mainwidget->show();

	// init signal handler
	signal(SIGCHLD, &exec_die_handler);

	// *** RUN MAIN EVENT HANDLER LOOP ***
	int return_code = app.exec();

	// Save QtChat state to file.
	ofstream qtchat_ofs(qtchat_ini_path);
	if(qtchat_ofs) {
		try {
			mainwidget->saveState(qtchat_ofs);
		} catch (ParamException) {
			cerr << endl;
			cerr << "main(): Unable to save QtChat state." << endl;
		}
	} else {
		cerr << argv[0] << ": Unable to open \"" << qtchat_ini_path << "\" for writing." << endl;
		cerr << argv[0] << ": QtChat state not saved." << endl;
	}

	// Save the chatters' states to file.
	ofstream chatters_ofs(chatters_ini_path);
	if(chatters_ofs) {
		cerr << "Saving Chatters' state..." << flush;
		Chatter::saveChatters(chatters_ofs);
		chatters_ofs.close();
		cerr << "done." << endl;
	} else {
		cerr << argv[0] << ": Unable to open \"" << chatters_ini_path << "\" for writing." << endl;
		cerr << argv[0] << ": Chatter states not saved." << endl;
	}

	// delete memory allocated by chatters[]
	Chatter::destroyAll();

	// go home
	return return_code;
}

