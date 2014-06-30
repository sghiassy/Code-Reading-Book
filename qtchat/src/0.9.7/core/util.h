#ifndef _UTIL_H
#define _UTIL_H

#include <sys/types.h>

char *itoa(int i, char *buf, int n);	// n == size of buf
void setBitfield(int &target, int value, int mask);
int getBitfield(int target, int mask);

int processURLs(char *in_buf, int len);	// add ANSI to delimit URLs
const char *findURL(const char *str, int len=-1);

const char *findTag(const char *str, int len);
void removeTags(char *str, int len);

char *findCensoredWord(char *sentence, char replacechar = '\0');

int shell(const char *cmd, char *arguments);	// exec shell command

float getSimilarity(const char *str1, const char *str2);

extern "C" {
char *strdup(const char *str);	// my implementation of strdup
int strcasecmp(const char *str1, const char *str2);	// my imp. of strcasecmp
}

const char *toString(bool b);

bool toBool(const char *str);

void getFreqDist(const char *str, size_t len, unsigned int *result);
float getMaxFreq(const unsigned int *result);
float getAvgFreq(const unsigned int *result);
float getSigma(const unsigned int *result);

#endif
