#ifndef _LINEENTRYDIALOG_H
#define _LINEENTRYDIALOG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>

class LineEntryDialog : public QDialog {
	Q_OBJECT
	public:
		LineEntryDialog(QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);
		LineEntryDialog(const char *prompt, QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);
		~LineEntryDialog();

		QLineEdit *getLineEdit();
		const char *text() const;

	signals:
		void textChanged(const char *);
		void text(const char *);

	public slots:
		void setText(const char *);
		void selectAll();
		void deselect();
		void clear();

	protected slots:
		void returnRedirect();

	protected:
		QLineEdit *entry;
		QLabel *label;

		void setSize();
};

#endif	// _LINEENTRYDIALOG_H
