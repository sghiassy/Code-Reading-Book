#include "Tree.h"
#include <iostream.h>

// Tree Edit - interactive tree creater/browser/printer

// menu class
class MenuItem {
	public:
		MenuItem(const char *label, int index);
		~MenuItem();

		int getIndex() {
			return i;
		}
		const char *getLabel() {
			return lbl;
		}
		int setItem(const char *label, int index);

	protected:
		char *lbl;
		int i;
};

class Menu {
	public:
		Menu(int num_items);
		~Menu();

		int addItem(MenuItem i);
		void removeItem(MenuItem i);
		void changeItem(MenuItem old, MenuItem new);

		void print();	// print menu choices
		MenuItem getChoice();	// prompt for menu choice

	protected:
		MenuItem *items;
};

int main(int argc, char *argv[]) {
	
