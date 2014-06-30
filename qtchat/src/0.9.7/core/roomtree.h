#ifndef _ROOMTREE_H
#define _ROOMTREE_H

#include "Tree.h"
#include "serializable.h"
#include "nc.h"
#include <fstream.h>
#include <string.h>

#define ROOM_START_TAG "room="
#define ROOM_END_TAG "</a>"
#define CATEGORY_START_TAG "<big>"
#define CATEGORY_END_TAG "</ul>"

class ChatRoomName : public TreeNode {
	public:
		ChatRoomName(const char *name, int number=1);
		~ChatRoomName();

		const char *toString() { return (const char *)name; }
		const char *getRoomName() { return toString(); }
		operator const char *() { return toString(); }
		int getRoomNumber() const { return number; }
		int compare(const TreeNode &x) {
			return strcmp(name, dynamic_cast<const ChatRoomName*>(&x)->name);
		}

	protected:
		char *name;
		int number;
};

class ChatRoomCategoryName : public ChatRoomName {
	public:
		ChatRoomCategoryName(const char *name);
};

class ChatRoomTree : public Tree {
	public:
		ChatRoomTree(ifstream &ifs);	// HTML file with roomlist
		ChatRoomTree(NetConnection &conn);	// socket stream connection to roomlist URL
};
#endif	// _ROOMTREE_H
