#ifndef _LOGIN_H
#define _LOGIN_H

#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qstringlist.h>
#include <stdlib.h>
#include "menubar.h"
#include "net.h"
#include "state.h"


class LoginDialog : public QDialog, public StateObject {
	Q_OBJECT
	public:
		LoginDialog(QWidget *parent = 0, QString name=0, WFlags f=0);
		~LoginDialog();

		// StateObject virtual methods
		void saveState(ostream &os) const;
		void restoreState(istream &is);

		// Accessors
		QString getUsername() const {
			return username->currentText();
		}
		QStringList getUserNames() const {
			QStringList names;
			for(int i=0; i<username->count(); i++)
				names.append(username->text(i));
			return names;
		}
		QString getPassword() const {
			// not exactly secure, since a memory scanner will
			// detect the password, but hey... this is Yahoo.
			// Don't keep secure info on an unsecure network.
			// We'll clear the password as soon as we can.
			return password->text();
		}
		QString getChatServer() const {
			return chat_server->currentText();
		}
		QStringList getChatServers() const {
			QStringList servers;
			for(int i=0; i<chat_server->count(); i++)
				servers.append(chat_server->text(i));
			return servers;
		}
		int getChatPort() const {
			return chat_port->text().toInt();
		}
		QString getGotoUser() const {
			return goto_user->currentText();
		}
		QStringList getGotoUsers() const {
			QStringList users;
			for(int i=0; i<goto_user->count(); i++)
				users.append(goto_user->text(i));
			return users;
		}
		QString getGotoRoom() const {
			return goto_room->currentText();
		}
		QStringList getGotoRooms() const {
			QStringList rooms;
			for(int i=0; i<goto_room->count(); i++)
				rooms.append(goto_room->text(i));
			return rooms;
		}
		bool gotoRoomClicked() const {
			return goto_room_clicked;
		}

		// Mutators
		void setUsername(QString name) {
			username->setEditText(name);
		}
		void setUserNames(QStringList names) {	// destructive
			username->clear();
			username->insertStringList(names);
			if(!names.isEmpty())
				username->setEditText(names[0]);
		}
		void setPassword(QString pwd) {
			// again, security bad
			password->setText(pwd);
			password->selectAll();
		}
		void setChatServer(QString name) {
			chat_server->setEditText(name);
		}
		void setChatServers(QStringList names) {
			chat_server->clear();
			chat_server->insertStringList(names);
			if(!names.isEmpty())
				chat_server->setEditText(names[0]);
		}
		void setChatPort(int port) {
			chat_port->setText(QString::number(port));
		}
		void setGotoUser(QString name) {
			goto_user->setEditText(name);
		}
		void setGotoUsers(QStringList names) {
			goto_user->clear();
			goto_user->insertStringList(names);
			if(!names.isEmpty())
				goto_user->setEditText(names[0]);
		}
		void setGotoRoom(QString name) {
			goto_room->setEditText(name);
		}
		void setGotoRooms(QStringList names) {
			goto_room->clear();
			goto_room->insertStringList(names);
			if(!names.isEmpty())
				goto_room->setEditText(names[0]);
		}

	public slots:
		void clearPassword() {
			password->clear();
		}

	protected:
		QComboBox *username;
		QLineEdit *password;
		QComboBox *chat_server;
		QLineEdit *chat_port;
		QPushButton *goto_room_button;
		QPushButton *goto_user_button;
		QComboBox *goto_room;
		QComboBox *goto_user;
		QLabel *labels[6];
		QPushButton *cancel;
		bool goto_room_clicked;

	private slots:
		void handleGotoRoomClicked();
		void handleGotoUserClicked();
};

#endif	// _LOGIN_H
