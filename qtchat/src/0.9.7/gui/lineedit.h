#ifndef _LINEEDIT_H
#define _LINEEDIT_H

#include <qlineedit.h>
#include <qstrlist.h>
#include "chardialog.h"
#include "debug.h"

class LineEdit : public QLineEdit {
	public:
		LineEdit(int histsize=-1, QWidget *parent=0, const char *name=0);
		~LineEdit();

// History list methods
		void setHistSize(int size);
		int getHistSize() const {
			return histsize;
		}
		void setHistory(QStrList *list);
		QStrList getHistory(bool shallow=false);

	// Export most of the history list methods
		void clearHistory() {
			history->clear();
		}
		unsigned int getHistorySize() const {
			return history->count()-1;
		}
		bool isHistoryEmpty() const {
			return history->isEmpty();
		}

	protected:
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void dragEnterEvent(QDragEnterEvent *e);
		virtual void dropEvent(QDropEvent *e);

	private:
		QStrList *history;
		CharDialog *char_dialog;
		int histsize;
};

#endif	// _LINEEDIT_H
