#include <iomanip>
#include <ctype.h>
#include <string.h>
#include "state.h"
#include "util.h"

ostream& operator<<(ostream &s, const StateObject &o) {
	o.saveState(s);
	return s;
}

istream& operator>>(istream &s, StateObject &o) {
	o.restoreState(s);
	return s;
}

void findParamHeader(istream &is, const char *tag) throw (ParamException) {
	if(is && tag) {
		while(*tag && isspace(*tag)) tag++;	// tag must start on nonwhitespace
		streampos prev_pos = is.tellg();
		streampos curr_line_pos;
		is.seekg(0, ios::beg);
		if(is) {
			char buf[512];
			curr_line_pos=is.tellg();
			while(is.getline(buf, 512)) {	// read in a line
				const char *cp=buf;
				while(*cp && isspace(*cp)) cp++;	// find first non-whitespace char
				if(strncasecmp(cp, tag, strlen(tag))==0) {	// tag found
					while(*cp && *cp != '=') cp++;	// find '=' sign
					is.seekg(curr_line_pos+(streampos)(cp-buf+1), ios::beg);	// seek stream to char after '='
					return;
				}
				curr_line_pos=is.tellg();
			}	// end while()... tag not found
		} else {	// can't rewind stream
			is.seekg(prev_pos);	// restore former position & throw exception
		}
		throw PARAM_TAG_NOT_FOUND;
	} else throw PARAM_BAD_STREAM;
}

void getParam(istream &is, const char *tag, bool &b) throw (ParamException) {
	try {
		findParamHeader(is, tag);	// may throw to caller
	} catch (ParamException e) {
		if(e == PARAM_TAG_NOT_FOUND)
			cerr << "getParam(): Unable to find tag \"" << tag << '\"' << endl;
		return;
	}
	char buf[64];
	is >> buf;
	if(strncasecmp(buf, "true", 4)==0) b=true;
	else if(strncasecmp(buf, "false", 5)==0) b=false;
	else throw PARAM_BAD_VALUE;
}

void getParam(istream &is, const char *tag, int &i) throw (ParamException) {
	try {
		findParamHeader(is, tag);	// may throw to caller
	} catch (ParamException e) {
		if(e == PARAM_TAG_NOT_FOUND)
			cerr << "getParam(): Unable to find tag \"" << tag << '\"' << endl;
//		throw e;
		return;
	}
	int tmp;
	is >> tmp;
	if(is) i=tmp;
	else throw PARAM_BAD_VALUE;
}

void getParam(istream &is, const char *tag, float &f) throw (ParamException) {
	try {
		findParamHeader(is, tag);	// may throw to caller
	} catch (ParamException e) {
		if(e == PARAM_TAG_NOT_FOUND)
			cerr << "getParam(): Unable to find tag \"" << tag << '\"' << endl;
//		throw e;
		return;
	}
	float tmp;
	is >> tmp;
	if(is) f=tmp;
	else throw PARAM_BAD_VALUE;
}

void getParam(istream &is, const char *tag, char *str, size_t len) throw (ParamException) {
	if(!str || len < 0) throw PARAM_BAD_ARGS;
	try {
		findParamHeader(is, tag);	// may throw to caller
	} catch (ParamException e) {
		if(e == PARAM_TAG_NOT_FOUND)
			cerr << "getParam(): Unable to find tag \"" << tag << '\"' << endl;
//		throw;
		return;
	}
	char *tmp = new char[len];
	is.getline(tmp, len);
	if(is) strncpy(str, tmp+1, len);
	else throw PARAM_BAD_VALUE;
	delete tmp;
}

void setParamHeader(ostream &os, const char *tag) throw (ParamException) {	// may throw to caller
	if(!os || !tag) throw PARAM_BAD_ARGS;
	os << setw(17) << setiosflags(ios::left) << tag << " = ";
}

void setParam(ostream &os, const char *tag, const int &i) throw (ParamException) {
	try {
		setParamHeader(os, tag);	// may throw to caller
	} catch (ParamException e) {
		cerr << "setParam(ostream&, const char*, const int&): Exception thrown." << endl;
//		throw e;
		return;
	}
	os << i << endl;
}

void setParam(ostream &os, const char *tag, const float &f) throw (ParamException) {
	try {
		setParamHeader(os, tag);	// may throw to caller
	} catch (ParamException e) {
		cerr << "setParam(ostream&, const char*, const float&): Exception thrown." << endl;
//		throw e;
		return;
	}
	os << f << endl;
}

void setParam(ostream &os, const char *tag, const bool &b) throw (ParamException) {
	try {
		setParamHeader(os, tag);	// may throw to caller
	} catch (ParamException e) {
		cerr << "setParam(ostream&, const char*, const bool&): Exception thrown." << endl;
//		throw e;
		return;
	}
	os << toString(b) << endl;
}

void setParam(ostream &os, const char *tag, const char *str, size_t len) throw (ParamException) {
	if(!str || len < 0) throw PARAM_BAD_ARGS;
	try {
		setParamHeader(os, tag);	// may throw to caller
	} catch (ParamException e) {
		cerr << "setParam(ostream&, const char*, const char*, size_t): Exception thrown." << endl;
//		throw e;
		return;
	}
	if(len > 0)
		os.write(str, len);
	else
		os << str << endl;
}

