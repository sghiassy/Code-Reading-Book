#include <ctype.h>
#include <iostream.h>
#include <limits.h>
#include <stdio.h>
#ifndef __USE_BSD
#define __USE_BSD
#endif
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"
#include "debug.h"

const char *censoredWords[] = {
	"ass",
	"bitch",
	"cunt",
	"dick",
	"fuck",
	"pussy",
	"shit",
	0
};

inline void swapchars(char *cp1, char *cp2) {
	char tmp;
	tmp = *cp1;
	*cp1 = *cp2;
	*cp2 = tmp;
}

char *itoa(int i, char *buf, int n) {
	if(buf && n > 1) {
		char *cp = buf;
		bool neg = i < 0;
		if(neg) {	// this isn't dangerous; the buffer is at least 2 chars
			*cp++ = '-';
			i = -i;
		}
		while(i && cp < buf + n - 1) {
			*cp++ = '0' + i%10;
			i/=10;
		}
		if(cp < buf + n)
			*cp = '\0';
		else
			return 0;
		char *cp2 = neg ? buf+1 : buf;
		cp--;
		while(cp2 < cp)
			swapchars(cp2++, cp--);
		return buf;
	} else
		return 0;
}

// setBitfield() sets specified bitfield in 'target' to the right-justified
// bitfield in 'value'.  The location of the bitfield in 'target' is
// specified by the bitmask 'mask'.  So setBitfield(x, 0x80, 1) sets bit
// 7 of 'x' to 1 and setBitfield(y, 0x3C, 9) sets bits 2-5 of 'y' to 9.
void setBitfield(int &target, int value, int mask) {
	target &= ~mask;
	int temp_mask = mask;
	while((temp_mask&1) != 1) {
		temp_mask >>= 1;	// shift right one bit
		value <<= 1;	// shift left one bit
	}
	value &= mask;	// mask off bad bits
	target |= value;
}

int getBitfield(int target, int mask) {
	target &= mask;
	while((mask&1) != 1) {
		target >>= 1;
		mask >>= 1;
	}
	return target;
}

float getSimilarity(const char *str1, const char *str2) {
	// returns 1.0 for exactly-matching strings
	// and some heuristically-determined intermediate value otherwise

	if(str1 == str2) return 1.0f;		// points to same string, or both NULL
	if(!str1 || !str2) return 0.0f;	// one string is NULL

	size_t minlen, maxlen;
	size_t tmp1 = strlen(str1), tmp2 = strlen(str2);
	minlen = tmp1 < tmp2 ? tmp1 : tmp2;
	maxlen = tmp1 > tmp2 ? tmp1 : tmp2;
	float sum_similarity = 0.0f;
	for(size_t i=0; i<minlen; i++) {
		if(str1[i]==str2[i]) sum_similarity+=1.0f;
		else if(tolower(str1[i])==tolower(str2[i])) sum_similarity+=0.92f;
	}
	return sum_similarity/(float)maxlen;
}

const char *findURL(const char *str, int len) {
	static const char *URLs[] = {
		"file:",
		"ftp.",
		"ftp:",
		"gopher:",
		"http:",
		"https:",
		"irc:",
		"mailto:",
		"news:",
		"pnm:",
		"telnet:",
		"www.",
		0
	};
	const char *cp = str;
	if(len < 0) len = INT_MAX;

	while((cp-str)<len && *cp) {
		if(isspace(*cp)) {	// find start of word
			cp++;
			continue;
		}
		int len_remaining = (int)len - (cp-str);
		bool not_in_choices = false;
		// loop through list of URLs
		for(int urls_index = 0; URLs[urls_index] && !not_in_choices; urls_index++) {
			const char *url = URLs[urls_index];
			if(url[0]<cp[0]) continue;	// haven't reached it yet
			int i;
			for(i=0; i<len_remaining && url[i]; i++) {
				if(url[i] != cp[i]) {	// past possible choices
					if(url[i] > cp[i]) not_in_choices=true;
					break;
				}
			}
			if(url[i] == '\0') {	// found one
				return cp;
			}
		}
//		while((cp-str)<len && *cp && !isspace(*cp))	// skip over word
			cp++;	// go to next char
	}
	return (const char *)0;
}

int processURLs(char *in_buf, int len) {	// add ANSI to delimit URLs
	int out_buf_sz = strlen(in_buf)+90;	// allow for 10 URLs in line (10*9 chars)
	char *out_buf = new char[out_buf_sz];
	memset(out_buf, 0, out_buf_sz);	// zero output buffer
	char *out_buf_ptr = out_buf;	// pointer into output buffer
	char *in_buf_ptr = in_buf;		// pointer into input buffer
	int num_URLs_found = 0;

	// null terminated ANSI header/trailer for URL
	static const char header[] = "\x1b\x5b\x6c\x6d";	// the 9 chars
	static const char trailer[] = "\x1b\x5b\x78\x6c\x6d";

	while(const char *cp = findURL(in_buf_ptr)) {	// find next URL
		num_URLs_found++;
		while(in_buf_ptr != cp) {	// copy chars up to URL into output buffer
			*out_buf_ptr++ = *in_buf_ptr++;	// ... and increment pointers
		}
		// copy header into output buffer
		for(int i=0; i<4; i++)
			*out_buf_ptr++ = header[i];
		// find end of URL
		while(*in_buf_ptr && !isspace(*in_buf_ptr) && *in_buf_ptr != '>')
			*out_buf_ptr++ = *in_buf_ptr++;
		for(int i=0; i<5; i++)	// copy trailer into output buffer
			*out_buf_ptr++ = trailer[i];
	}
	while(*in_buf_ptr)	// copy remaining chars to output buffer
		*out_buf_ptr++ = *in_buf_ptr++;
	if(num_URLs_found) {
		*in_buf_ptr = '\0';
		strncpy(in_buf, out_buf, len);	// copy answer into caller's buffer
	}
	delete out_buf;
	return num_URLs_found;
}

char *findCensoredWord(char *sentence, char replacechar) {
	// check each substring in sentence
	for(const char *curr_pos=sentence; *curr_pos; curr_pos++) {
		const char *badword;
		for(int i=0; (badword=censoredWords[i]) && *badword <= *curr_pos; i++) {
			if(char *cp=strstr(sentence, badword)) {
				if(replacechar) {
					while(*badword) {
						*cp++ = replacechar;
						badword++;
					}
				}
				return cp;
			}
		}
	}
	return 0;
}

const char *findTag(const char *str, int len) {
	const char *src;
	const char *Tags[] = {
		"font",
		"fade",
		"snd",
		"alt",
		"#",
		0
	};

	for(src=str; *src && src-str < len; src++) {	// loop through string
		if(*src != '<') continue;	// scan until tag delimiter
		const char *tag_start = src;	// save start of tag
		src++;	// move to next char past tag opening delimiter
		while(*src && isspace(*src)) src++;	// skip over any whitespace
		for(int i=0; Tags[i]; i++) {	// check against list of tags
			if(*src && strncasecmp(src, Tags[i], strlen(Tags[i]))==0) {	// is this an opening tag?
				if(strchr(tag_start, '>')) {
					return tag_start;
				}
			}
			if(*src == '/' && strncasecmp(src+1, Tags[i], strlen(Tags[i]))==0) {	// closing tag?
				if(strchr(tag_start, '>')) {
					return tag_start;
				}
			}
		}
	}
	return (const char *)0;	// nothing found
}

void removeTags(char *str, int len) {
	if(!str || len<=0) return;	// check bad args
	char *tag=str-1;
	while((tag = (char *)(findTag(tag+1, len - (tag-str))))) {
		char *tag_end = tag;	// look for end of tag
		while(*tag_end != '>') tag_end++;

		char *dst = tag;
		char *src= tag_end+1;
		while(*src) *dst++ = *src++;	// copy over tag
		*dst = '\0';	// terminate modified string
		tag--;	// move the tag ptr back since it was just deleted
	}
}

char *strdup(const char *str) {	// my implementation of strdup
	if(!str || !(*str)) return 0;
	char *cp = new char[strlen(str)+1];
	if(!cp) return 0;
	else return strcpy(cp, str);
}

int strcasecmp(const char *str1, const char *str2) {	// my imp. of strcasecmp
	while(*str1 && *str2) {	// this'll segfault if any of 'em is NULL
		if(*str1 < *str2) return -1;
		if(*str1 > *str2) return +1;
		str1++; str2++;
	}
	if(!*str1 && !*str2) return 0;
	if(*str1) return -1;
	return +1;
}

const char *toString(bool b) {
	return (b ? "true" : "false");
}

bool toBool(const char *str) {
	if(strcasecmp(str, "true")==0) return true;
	return false;
}

// ABJ 10-08-1999 10:40 - added shell command
extern char **environ;	// can't seem to find it in any other header file

int shell(const char *cmd, char *argstr) {
	if(!cmd) {
		if(::debuglevel & DEBUGLEVEL_ERROR)
			cerr << "YahooNetCommand::shell(): Bad argument." << endl;
		return -1;
	}
	char *argv[128] = {const_cast<char *>(cmd)};	// create arg array & init first element
	// The last statement should init all remaining elements to 0, but...
	for(int i=1; i<128; i++) argv[i] = 0;
	int i=1;	// argv index
	if(argstr) {	// if we have any args
		for(char *cp=strtok(argstr, " \t\n\r"); cp && i<127; cp=strtok(NULL, " \t\n\r")) {
			argv[i++] = cp;
		}
	}
	// now let's create a new process to handle the request.
	pid_t process_id = fork();	// attempt to create process
	if(process_id == 0) {	// child process
//		sleep(1);	// wait for a second for the parent to save the PID
		if(execvp(cmd, argv)) {	// exec the new process
			// Execution here iff cmd failed
			char message[64];	// form error message
			strncpy(message, "Error executing '", 64);
			strncat(message, cmd, 64);
			if(argstr) {
				strncat(message, " ", 64);
				strncat(message, argstr, 64);
			}
				strncat(message, "'.", 64);
			if(::debuglevel & DEBUGLEVEL_ERROR) {
				cerr << message << endl;
				perror("execvp()");
			}
		}
		exit(1);
	} else if(process_id > 0) {	// parent process
		return process_id;
	} else {	// fork() failed
		const char *message = "Error: Unable to fork new process.";
		if(::debuglevel & DEBUGLEVEL_ERROR) {
			cerr << message << endl;
			perror("fork()");
		}
//		message.prepend("*** ");
//		message.append(" ***");
//		emit printChatMessage(message, 0, CET_SYSTEM_MESSAGE);
		return -1;
	}
	return 0;
}
// END ABJ 10-08-1999 10:40 - added shell comman

void getFreqDist(const char *str, size_t len, unsigned int *result) {
	if(!str || !result) return;
	for(int i=0; i<256; i++) result[i]=0;
	for(size_t i=0; i<len; i++) {
		result[((unsigned char)(str[i]))]++;
	}
}

float getMaxFreq(const unsigned int *result) {
	unsigned int max_freq = 0;
	for(int i=0; i<256; i++)
		if(result[i] > max_freq) max_freq = result[i];
	return (float)max_freq;
}

float getAvgFreq(const unsigned int *result) {
	unsigned int freq_sum = 0;
	int num_freqs = 0;
	for(int i=0; i<256; i++) {
		if(result[i]) {
			freq_sum += result[i];
			num_freqs++;
		}
	}
	if(num_freqs)
		return (float)freq_sum / (float)num_freqs;
	else
		return 0.0;
}

float getSigma(const unsigned int *result) {
	unsigned int num_nonzero = 0;
	for(int i=0; i<256; i++) {
		if(result[i]) num_nonzero++;
	}
	return (float)num_nonzero;
}
