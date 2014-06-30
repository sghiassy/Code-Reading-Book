#ifndef _ROOMLIST_DIALOG_H
#define _ROOMLIST_DIALOG_H

#include <qdialog.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include "roomtree.h"

class RoomListDialog : public QDialog {
	Q_OBJECT
	public:
		RoomListDialog(ChatRoomTree *roomtree=0, QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);
		~RoomListDialog();

	signals:
		void roomSelected(const char *name);
		
	protected:
		virtual void resizeEvent(QResizeEvent *e);

	private:
		QListView *roomlist;
		QPushButton *go_button;
		QPushButton *cancel_button;

	private slots:
		void handleGoButtonClicked();
		void handleDoubleClick(QListViewItem *item);
};

#endif	// _ROOMLIST_DIALOG_H
