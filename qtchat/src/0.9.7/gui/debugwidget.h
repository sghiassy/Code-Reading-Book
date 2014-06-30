#ifndef _DEBUGWIDGET_H
#define _DEBUGWIDGET_H

#include <qcolor.h>
#include <qcolordialog.h>
#ifndef INCLUDE_MENUITEM_DEF
#define INCLUDE_MENUITEM_DEF
#include <qmenubar.h>
#endif	// INCLUDE_MENUITEM_DEF
#include <qmultilineedit.h>
#include <qstatusbar.h>
#include "debug.h"

class MyMultiLineEdit;

class DebugWidget : public QWidget {
	Q_OBJECT
	public:
		DebugWidget(QWidget *parent=0, const char *name=0, WFlags f=0);
		~DebugWidget();

		QMultiLineEdit *getMultiLineEdit() {
			return edit_widget;
		}

	public slots:
		// get data from app
		void dataTransferred(const unsigned char *_data, int data_len, bool incoming);

		// Export most of the QMultiLineEdit widget's slots
		void clear() {
			edit_widget->clear();
		}
		void append(const QString cp) {
			edit_widget->append(cp);
		}
		void deselect() {
			edit_widget->deselect();
		}
		void selectAll() {
			edit_widget->selectAll();
		}

		void findText() {
		}

// Options functions
		void toggleIncoming();
		void setIncoming(bool enable);
		void toggleOutgoing();
		void setOutgoing(bool enable);
		void toggleFreeze();
		void setFreeze(bool b);
		void setIncomingColor(QColor c) {
			incoming_color = c;
		}
		void setOutgoingColor(QColor c) {
			outgoing_color = c;
		}
		void setIncomingColor() {
			setIncomingColor(QColorDialog::getColor(incoming_color, this));
		}
		void setOutgoingColor() {
			setOutgoingColor(QColorDialog::getColor(outgoing_color, this));
		}

// Selected text functions
		void cut() {
			edit_widget->cut();
		}
		void copyText() {
			edit_widget->copyText();
		}
		void paste() {
			edit_widget->paste();
		}
//		void deleteText();

// Save widget text
		void saveTextAs();

	private:
		QMultiLineEdit *edit_widget;
		QMenuBar *menu_bar;
		QStatusBar *status_bar;
		bool show_incoming;
		bool show_outgoing;
		QColor incoming_color;
		QColor outgoing_color;
		bool frozen;

	protected:
		virtual void resizeEvent(QResizeEvent *e);

	private slots:
//		void handleTextChanged();
};

#endif	// _DEBUGWIDGET_H
