#include "roomtree.h"
#include <iostream>
#include <string.h>

const char *parseRoomList(ChatRoomName *current_node, const char *htmlcode);

ChatRoomName::ChatRoomName(const char *_name, int _number) {
	if(_name) {
		name = new char[strlen(_name)+1];
		strcpy(name, _name);
	} else
		name = 0;
	number = _number;
}

ChatRoomName::~ChatRoomName() {
	if(name) delete name;
}

ChatRoomCategoryName::ChatRoomCategoryName(const char *name) :
	ChatRoomName(name, 0) {}

ChatRoomTree::ChatRoomTree(ifstream &ifs) {	// HTML file with roomlist
	char buf[32768];	// last roomlist HTML page retrieved was 17,996 bytes
	memset(buf, '\0', 32768);

	cerr << "Loading room list input stream...";
	ifs.read(buf, 32768);

	cerr << "done." << endl;

	ChatRoomCategoryName *root = new ChatRoomCategoryName("Root");
	setRoot(root);
	cerr << "Parsing room list input buffer...";
	if(parseRoomList(root, buf))
		cerr << "error!" << endl;
	else
		cerr << "done." << endl;
}

ChatRoomTree::ChatRoomTree(NetConnection &conn) {	// socket stream connection to roomlist URL
	char buf[32768];	// last roomlist HTML page retrieved was 17,996 bytes

	memset(buf, '\0', 32768);

	cerr << "Loading room list input stream";
	char *cp = buf;	// init current write pointer into buffer
	int num_bytes_read = 0;
	do {
		conn.isDataAvailable();
		num_bytes_read = conn.read(cp, 32767-((int)(cp-buf)));
		cp += num_bytes_read;
		cerr << '.';
	} while(num_bytes_read);

	cerr << "done." << endl;

	ChatRoomCategoryName *root = new ChatRoomCategoryName("Root");
	setRoot(root);

	cerr << "Parsing room list input stream...";
	if(parseRoomList(root, buf))
		cerr << "error!" << endl;
	else
		cerr << "done." << endl;
}

const char *parseRoomList(ChatRoomName *current_node, const char *htmlcode) {
	const char *category_start_tag = CATEGORY_START_TAG;
	const char *category_end_tag = CATEGORY_END_TAG;
	const char *room_start_tag = ROOM_START_TAG;
	int category_start_tag_index = 0;
	int category_end_tag_index = 0;
	int room_start_tag_index = 0;

	if(!current_node || !htmlcode) return 0;
	if(current_node->getRoomNumber() > 0)	// this is not a category name
		return htmlcode;
	char ch;
	while((ch=*htmlcode)) {
		if(ch == category_start_tag[category_start_tag_index]) {
			category_start_tag_index++;
		} else {
			if(!category_start_tag[category_start_tag_index] && category_start_tag_index) {
				// match
				char buf[256];
				int i;	// index into buf[]
				memset(buf, '\0', 256);

				for(i=0; i<256 && htmlcode[i] && htmlcode[i] != '<'; i++)
					buf[i] = htmlcode[i];
				htmlcode += i;
				ChatRoomCategoryName *subcategory = new ChatRoomCategoryName(buf);
				if(!subcategory) return 0;	// allocation error
				current_node->addChild(subcategory);	// add to tree
				// parse this subcategory
				const char *newhtmlcode = parseRoomList(subcategory, htmlcode);
				if(!newhtmlcode) return 0;	// forward error to caller
				htmlcode = newhtmlcode;
				category_start_tag_index = 0;
				continue;
			} else {	// no match
				category_start_tag_index = 0;	// reset tag index
			}
		}

		if(ch == category_end_tag[category_end_tag_index]) {
			category_end_tag_index++;
		} else {
			if(!category_end_tag[category_end_tag_index] && category_end_tag_index) {
				// match
				return htmlcode;
			} else {
				category_end_tag_index = 0;	// reset tag index
			}
		}

		if(ch == room_start_tag[room_start_tag_index]) {
			room_start_tag_index++;
		} else {
			if(!room_start_tag[room_start_tag_index] && room_start_tag_index) {
				// match
				char buf[256];
				int i;	// index into buf[]
				memset(buf, '\0', 256);

				while(*htmlcode && *htmlcode != '>')	// seek to start of name
					htmlcode++;
				htmlcode++;	// htmlcode-> first char of name
				for(i=0; i<256 && htmlcode[i] && htmlcode[i] != '<'; i++)
					buf[i] = htmlcode[i];
				htmlcode += i;
				ChatRoomName *roomname = new ChatRoomName(buf);
				if(!roomname) return 0;	// allocation error
				current_node->addChild(roomname);	// add to tree
				// parse this subcategory
				const char *newhtmlcode = parseRoomList(roomname, htmlcode);
				if(!newhtmlcode) return 0;	// forward error to caller
				htmlcode = newhtmlcode;
				room_start_tag_index = 0;
				continue;
			} else {	// no match
				room_start_tag_index = 0;	// reset tag index
			}
		}
		htmlcode++;
	}
	return 0;
}
