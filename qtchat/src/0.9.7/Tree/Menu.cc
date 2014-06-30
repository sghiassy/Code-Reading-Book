#include "Menu.h"
#include <string.h>
#include <iostream.h>
#include <iomanip.h>

	MenuItem::MenuItem() {
		setItem(0, 0);
	}

	MenuItem::MenuItem(const char *label, int index) {
		setItem(label, index);
	}

	MenuItem::MenuItem(const MenuItem& item) {
		setItem(item.lbl, item.i);
	}
			
	MenuItem::~MenuItem() {
		if(lbl)
			delete lbl;
	}

	int MenuItem::setItem(const char *label, int index) {
		if(label) {
			lbl = new char[strlen(label)+1];
			strcpy(lbl, label);
			i = index;
		} else {
			lbl = 0;
			i = 0;
		}
		return i;
	}

		bool MenuItem::operator==(const MenuItem& item) const {
			if(getLabel() && item.getLabel())
				return strcmp(getLabel(), item.getLabel()) == 0;
			else
				return (!getLabel() && !item.getLabel());
		}

		MenuItem& MenuItem::operator=(const MenuItem& item) {
			setItem(item.getLabel(), item.getIndex());
			return *this;
		}

	Menu::Menu(int _num_items) {
		if(num_items > 0) {
			items = new MenuItem*[num_items];
			num_items = _num_items;
			for(int i=0; i<num_items; i++)
				items[i] = 0;
		} else {
			items = 0;
			num_items = 0;
		}
	}

	Menu::Menu(const Menu& menu) {
		num_items = menu.num_items;
		if(num_items)
			items = new MenuItem*[menu.num_items];
		else
			items = 0;
		for(int i=0; i<num_items; i++)
			items[i] = menu.items[i];
	}

	Menu::~Menu() {
		for(int i=0; i<num_items; i++)
			if(items[i]) delete items[i];
		if(items) delete items;
	}

	int Menu::addItem(MenuItem *i) {
		if(i->getIndex() < num_items && !items[i->getIndex()]) {	// okay to add
			items[i->getIndex()]=i;
		}
		return i->getIndex();
	}

	int Menu::removeItem(MenuItem *item) {
		for(int i=0; i<num_items; i++) {
			if(items[i] && i == items[i]->getIndex()) {
				items[i]->setItem(0, 0);
				return i;
			}
		}
		return -1;
	}
				

	void Menu::print() const {	// print menu choices
		// find max width of items
		int width = 0;
		for(int i=0; i<num_items; i++) {
			int j = (items[i] && items[i]->getLabel()) ? strlen(items[i]->getLabel()) : 0;
			if(j > width) width = j;
		}
		// print menu
		for(int i=0; i<width+9; i++) cout << '-';	// top border
		cout << endl << '|';	// blank line
		for(int i=0; i<width+7; i++) cout << ' ';
		cout << '|' << endl;
		for(int i=0; i<num_items; i++) {	// menu item
			if(items[i] && items[i]->getLabel()) {
				cout.setf(ios::right);
				cout << "| " << setw(3) << items[i]->getIndex() << ". ";
				cout.setf(ios::left);
				cout << setw(width) << items[i]->getLabel() << " |" << endl;
			}
		}
		cout << '|';
		for(int i=0; i<width+7; i++) cout << ' ';	// blank line
		cout << '|' << endl;
		for(int i=0; i<width+9; i++) cout << '-'; // bottom border
		cout << endl;
	}

	MenuItem Menu::getChoice() const {	// prompt for menu choice
		int answer;
		while(1) {
			cout << "\tEnter choice: ";
			cin >> answer;
			for(int i=0; i<num_items; i++) {
				if(items[i] && items[i]->getLabel() && items[i]->getIndex() == answer) {
					return *items[i];
				}
			}
			cout << "Error!!! Invalid choice (" << answer << ")!!!  Try again!!!" << endl;
		}
	}

