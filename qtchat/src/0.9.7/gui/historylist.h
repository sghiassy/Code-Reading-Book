#ifndef _HISTORYLIST_H
#define _HISTORYLIST_H
#include <qlistbox.h>

class HistoryListWidget : public QWidget {
	public:
		HistoryListWidget(QWidget *parent=0, const char *name=0, WFlags=0);
		~HistoryListWidget();

	private:
		QListBox *history;
};

#endif	// _HISTORYLIST_H
