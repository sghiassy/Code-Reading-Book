#include "Tree.h"
#include "Menu.h"
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

   class IntTreeNode : public TreeNode {
   public:
      IntTreeNode(int _i) {
         i=_i;
			str_i = new char[20];
			sprintf(str_i, "%20d", i);
      }
      ~IntTreeNode() {
			delete str_i;
      }
		virtual const char *IntTreeNode::toString() {
			return str_i;	// bad... allows our string to be corrupted
		}

		virtual int compare(const TreeNode &n) {
			return i - ((IntTreeNode &)n).i;
		}
			
   protected:
      int i;
		char *str_i;
   };

   class StringTreeNode : public TreeNode {
   public:
      StringTreeNode(const char *str) {
         s = strdup(str);
      }
      ~StringTreeNode() {
         free(s);
      }
		virtual const char *toString() {
			return s;
		}
		virtual int compare(const TreeNode &n) {
			return strcmp(s, ((StringTreeNode &)n).s);
		}
   
   protected:
      char *s;
   };
	
   int main(int, char **) {
      Tree tree(new StringTreeNode("00"));
		TreeNode *tnp;
//		Menu menu(7);
//		MenuItem *itemptr;

//		itemptr = new MenuItem("Add Tree Node", 1);
//		menu.addItem(itemptr);
//		itemptr = new MenuItem("Delete Tree Node", 2);
//		menu.addItem(itemptr);
//		itemptr = new MenuItem("Move Up One Level", 3);
//		menu.addItem(itemptr);
//		itemptr = new MenuItem("Move Down One Level", 4);
//		menu.addItem(itemptr);
//		itemptr = new MenuItem("Display Tree", 5);
//		menu.addItem(itemptr);
//		itemptr = new MenuItem("Exit", 6);
//		menu.addItem(itemptr);

//		menu.print();
//		menu.getChoice();

		tree.addChild(tree.getRoot(), new StringTreeNode("10"));
		tree.addChild(tree.getRoot(), new StringTreeNode("11"));
		tree.addChild(tree.getRoot(), tnp = new StringTreeNode("12"));
		tree.addChild(tnp, new StringTreeNode("20"));
		TreeNode *tnp2;
		tree.addChild(tnp, tnp2 = new StringTreeNode("21"));
		tree.addChild(tree.getRoot(), new StringTreeNode("13"));
		tree.addChild(tnp2, new StringTreeNode("30"));
      cout << "Tree has " << tree.getNumNodes() << " children. " << endl;
      cout << "Tree is " << tree.getMaxDepth() << " deep. " << endl;
		tree.print();

		cout << "Pre-order iteration:" << endl;
		TreeIterator ti1(&tree, PRE_ORDER);
		while(!ti1.end()) {
			cout << ti1->toString() << endl;
			ti1++;
		}
  		cout << "Post-order iteration:" << endl;
		TreeIterator ti2(&tree, POST_ORDER);
		while(!ti2.end()) {
			cout << ti2->toString() << endl;
			ti2++;
		}
  		cout << "Level-order iteration:" << endl;
		TreeIterator ti3(&tree, LEVEL_ORDER);
		while(!ti3.end()) {
			cout << ti3->toString() << endl;
			ti3++;
		}
   };
