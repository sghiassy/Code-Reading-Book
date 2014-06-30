#include "prototypes.h"
#include "util.h"
#include "wc.h"
#include <ctype.h>
#include <stdlib.h>
extern "C" {
#include <string.h>
}

Word::Word(const char *str) {
	if(str)
		word = strdup(str);
	else
		word = 0;
}

Word::Word(const Word *w) {
	if(w->word)
		word = strdup(w->word);
	else
		word = 0;
}

Word::Word(const Word &w) {
	if(w.word)
		word = strdup(w.word);
	else
		word = 0;
}

Word::~Word() {
	if(word)
		delete word;
}

Word::operator const char *() {
	return word;
}

bool Word::operator==(const Word &w) {
	return (strcmp(w.word, word)==0);
}

Word & Word::operator=(const Word &w) {
	if(word) delete word;
	if(w.word)
		word = strdup(w.word);
	else
		word = 0;
	return *this;
}

const Word NullWord(0);

int word_cmp(const void *a, const void *b) {
	const Word *wa = (const Word *)a;
	const Word *wb = (const Word *)b;
	if(wa->isNull() && wb->isNull()) return 0; // Null == Null
	if(wa->isNull()) return +1;	// Null > Word
	if(wb->isNull()) return -1;	// Word < Null
	return strcmp((const char *)wa, (const char *)wb);
}

template<class T>
void WordList::init(T _words[], int _n) {
	if(_n > 0) {
		words = new Word*[_n];
		for(int i=0; i<_n; i++)
			words[i] = new Word(_words[i]);
		n = _n;
	} else {
		words = 0;
		n = 0;
	}
}

WordList::WordList() {
	words = 0;
	n = 0;
}

WordList::WordList(Word _words[], int _n) {
	init(_words, _n);
}

WordList::WordList(const char *_words[], int _n) {
	init(_words, _n);
}

static const char *whitespace = " \n\r\t";

WordList::WordList(const char *line) {
	n=0;
	words = 0;
	if(line) {
		bool word = false;
		const char *cp;
		for(cp=line; *cp; cp++) {
			if(word) {
				if(isspace(*cp)) {
					word = !word;
					n++;
				}
			} else
				if(!isspace(*cp))
					word = !word;
		}

		char *tmpline = strdup(line);	// duplicate line to tokenize
		cp = strtok(tmpline, whitespace);	// find first word
		while(cp) {
			add((Word)cp);	// add word
			strtok(NULL, whitespace);	// find next word
		}
		free(tmpline);	// deallocate line
	}
}

WordList::WordList(Word word) {
	n = 1;
	words = new Word*[1];
	words[0] = new Word(word);
}

WordList::WordList(const WordList &wl) {
	init(wl.words, wl.n);
	qsort(words, n, sizeof(Word *), word_cmp);
}

WordList::~WordList() {
	if(words) delete words;
}

void WordList::add(Word word) {
	Word **tmp = new Word*[n+1];
	for(int i=0; i<n; i++)
		tmp[i] = new Word(words[i]);
	tmp[n] = new Word(word);
	if(words) delete words;
	words = tmp;
	n++;
}

WordList WordList::suggest(Word partial) const {
	return WordList();	// unimplemented so far
}

WordCompleter::WordCompleter() {
	for(int i=0; i<MAX_WORDLISTS; i++) lists[i] = 0;
	default_list = 0;
	line = 0;
	cursor_pos = 0;
}

WordCompleter::~WordCompleter() {
	for(int i=0; i<MAX_WORDLISTS; i++)
		if(lists[i]) delete lists[i];
	if(default_list) delete default_list;
	if(line) delete line;
}

void WordCompleter::setLine(const char *line, int cursor_pos) 
Word WordCompleter::getWordAt(int cursor_pos);	// return word at cursor_pos
Word WordCompleter::getWordAt();		// return word at current cursor position
WordList WordCompleter::getSuggestionsAt(int cursor_pos);	// get list of suggestions at cursor_pos
WordList getSuggestionsAt();	// get list of suggestions at current cursor

// Iterator Methods
Word getFirstSuggestionAt(int cursor_pos);
Word getFirstSuggestionAt();
Word getNextSuggestionAt(int cursor_pos);
Word getNextSuggestionAt();
Word getPrevSuggestionAt(int cursor_pos);
Word getPrevSuggestionAt();
