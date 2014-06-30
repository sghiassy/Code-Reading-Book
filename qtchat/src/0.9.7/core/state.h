#ifndef _STATEOBJECT_H
#define _STATEOBJECT_H

#include <iostream.h>

class StateObject {
	friend ostream& operator<<(ostream &s, const StateObject &o);
	friend istream& operator>>(istream &s, StateObject &o);

	protected:
		virtual void saveState(ostream &s) const = 0;	// reimplement me
		virtual void restoreState(istream &s) = 0;	// reimplement me
};

enum ParamException {
	PARAM_TAG_NOT_FOUND,
	PARAM_BAD_VALUE,
	PARAM_BAD_STREAM,
	PARAM_BAD_ARGS
};

void getParam(istream &is, const char *tag, int &i) throw (ParamException);
void getParam(istream &is, const char *tag, float &f) throw (ParamException);
void getParam(istream &is, const char *tag, bool &b) throw (ParamException);
void getParam(istream &is, const char *tag, char *str, size_t len) throw (ParamException);

void setParam(ostream &os, const char *tag, const int &i) throw (ParamException);
void setParam(ostream &os, const char *tag, const float &f) throw (ParamException);
void setParam(ostream &os, const char *tag, const bool &b) throw (ParamException);
void setParam(ostream &os, const char *tag, const char *str, size_t len=0) throw (ParamException);

#endif	// _STATEOBJECT_H
