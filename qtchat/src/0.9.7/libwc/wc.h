#ifndef _WC_H
#define _WC_H

class Word {
	public:
		Word(const char *str);
		Word(const Word &w);
		Word(const Word *w);
		virtual ~Word();

		operator const char *();
		bool operator==(const Word &w);
		Word& operator=(const Word &w);
		bool isNull() const {
			return word==0;
		}

	private:
		char *word;
};

extern const Word NullWord;

class WordList {
	public:
		WordList();
		WordList(Word *words, int n);
		WordList(const char *words[], int n);
		WordList(const char *line);
		WordList(Word word);
		WordList(const WordList &wl);
		virtual ~WordList();

		int count() const {
			return n;
		}
		void add(Word word);
		WordList suggest(Word partial) const;

	private:
		Word **words;
		int n;

		template<class T>
		void WordList::init(T _words[], int _n);
};

class SortedWordList : public WordList {
	public:
		SortedWordList();
		SortedWordList(Word *words, int n);
		SortedWordList(const char *words[], int n);
		SortedWordList(const char *line);
		SortedWordList(Word word);
		SortedWordList(const WordList &wl);
};

class WordCompleter {
	public:
		WordCompleter();
		virtual ~WordCompleter();


		void setLine(const char *line, int cursor_pos);
		Word getWordAt(int cursor_pos);	// return word at cursor_pos
		Word getWordAt();		// return word at current cursor position
		SortedWordList getSuggestionsAt(int cursor_pos);	// get list of suggestions at cursor_pos
		SortedWordList getSuggestionsAt();	// get list of suggestions at current cursor

	// Iterator Methods
		Word getFirstSuggestionAt(int cursor_pos);
		Word getFirstSuggestionAt();
		Word getNextSuggestionAt(int cursor_pos);
		Word getNextSuggestionAt();
		Word getPrevSuggestionAt(int cursor_pos);
		Word getPrevSuggestionAt();

	private:
#define MAX_WORDLISTS 256
		SortedWordList *lists[MAX_WORDLISTS];
		SortedWordList *default_list;
		char *line;
		int cursor_pos;
};

#endif	// _WC_H
