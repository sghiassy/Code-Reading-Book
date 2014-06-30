#include "Tree.h"
#include <iostream.h>

// Tree Edit - interactive tree creater/browser/printer

// menu class
class MenuItem {
	public:
		MenuItem();
		MenuItem(const char *label, int index);
		MenuItem(const MenuItem& item);
		~MenuItem();

		int getIndex() const {
			return i;
		}
		const char *getLabel() const {
			return lbl;
		}
		bool operator==(const MenuItem& item) const;
		MenuItem &operator=(const MenuItem& item);
		int setItem(const char *label, int index);

	protected:
		char *lbl;
		int i;
};

class Menu {
	public:
		Menu(int num_items);
		Menu(const Menu& menu);
		~Menu();

		int addItem(MenuItem *i);
		int removeItem(MenuItem *i);
		int removeItem(int index);

		void print() const;	// print menu choices
		MenuItem getChoice() const;	// prompt for menu choice

	protected:
		MenuItem **items;
		int num_items;
};
