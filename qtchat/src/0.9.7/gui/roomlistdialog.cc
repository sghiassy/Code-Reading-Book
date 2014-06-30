#include "roomlistdialog.h"
#include <map>

struct TreeInfo {
	TreeNode *treeitem;
	QListViewItem *listviewitem;
};

typedef map<TreeNode *, QListViewItem *> treemaptype;
treemaptype treemap;

RoomListDialog::RoomListDialog(ChatRoomTree *roomtree, QWidget *parent, const char *name, bool modal, WFlags f) :
	QDialog(parent, name, modal, f) {

	roomlist = new QListView(this);
	roomlist->setRootIsDecorated(true);
	roomlist->setRootIsDecorated(true);
	roomlist->addColumn(QString("Name"));
	roomlist->addColumn(QString("Occupants"));
	connect(roomlist, SIGNAL(doubleClicked(QListViewItem *)),
		SLOT(handleDoubleClick(QListViewItem *)));
	go_button = new QPushButton("Go", this);
	go_button->resize(go_button->sizeHint());
	go_button->setDefault(true);
	connect(go_button, SIGNAL(clicked()), SLOT(accept()));
	connect(go_button, SIGNAL(clicked()), SLOT(handleGoButtonClicked()));
	cancel_button = new QPushButton("Cancel", this);
	cancel_button->resize(cancel_button->sizeHint());
	connect(cancel_button, SIGNAL(clicked()), SLOT(reject()));
	if(roomtree) {
		QListViewItem *item;
		for(TreeIterator ti(roomtree, PRE_ORDER); !ti.end(); ti++) {
			if(ti->isRoot()) continue;
			if(ti->getDepth() == 1) {
				item = new QListViewItem(roomlist, ti->toString());
			} else {
				item = new QListViewItem(treemap.find(ti->getParent())->second, ti->toString());
			}
			treemap.insert(treemaptype::value_type(ti, item));
		}
	}
	setMinimumSize(go_button->width()+cancel_button->width()+30, 150);
}

RoomListDialog::~RoomListDialog() {
	delete roomlist;
	delete go_button;
	delete cancel_button;
}

void RoomListDialog::resizeEvent(QResizeEvent *e) {
	go_button->move(10, e->size().height()-10-go_button->height());
	cancel_button->move(e->size().width()-10-cancel_button->width(), e->size().height()-10-cancel_button->height());
	roomlist->move(0,0);
	roomlist->resize(e->size().width(), go_button->y()-10);

}

void RoomListDialog::handleGoButtonClicked() {
	if(roomlist->selectedItem())
		emit roomSelected((const char *)(roomlist->selectedItem()->text(0)));
}

void RoomListDialog::handleDoubleClick(QListViewItem *item) {
	handleGoButtonClicked();
}
