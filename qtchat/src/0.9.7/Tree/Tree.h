#ifndef _TREE_H
#define _TREE_H
#include <iostream.h>
#include <deque>

// ********** Exception Types **********
	enum TreeNodeException {
	TREENODE_EXCEPTION_BAD_ARGS
	};

	enum TreeException {
	TREE_EXCEPTION_BAD_ARGS
	};

// ********** TreeNode Abstract Class **********
	extern const char *TREENODE_STRING_DFL;	// used by toString()

	class TreeNode {
	public:
	// Constructors/Destructors
		TreeNode();
		virtual ~TreeNode();
	
	// Accessors
		int getDepth() const {
			return depth;
		}
		int getMaxDepth();
		int getNumChildren() const {
			return num_children;
		}
		int getNumDescendants();
		int getNumSiblings() const {
			return parent ? parent->num_children : 0;
		}
		TreeNode *getParent() const {
			return parent;
		}
		bool isRoot() const {
			return parent==0;
		}
		bool isLeaf() const {
			return first_child==0;
		}
		TreeNode *getFirstChild() const {
			return first_child;
		}
	
		TreeNode *getLastChild();
		TreeNode *getFirstSibling();
		TreeNode *getLastSibling();
		TreeNode *getNextSibling();
		TreeNode *getPrevSibling();

		virtual int compare(const TreeNode &n)=0;	//  0 if n == *this
														// +1 if n >  *this
														//	-1 if n <  *this
		bool operator==(const TreeNode &n);
		bool operator<(const TreeNode &n);
		bool operator<=(const TreeNode &n);
		bool operator>(const TreeNode &n);
		bool operator>=(const TreeNode &n);
		bool operator!=(const TreeNode &n);

		virtual const char *toString()=0;	// return string rep
		virtual int getStringLength() {	// return length of string rep
			return strlen(toString());		// default to finding out from toString()
		}
	
	// Mutators
		void addChild(TreeNode *node) throw (TreeNodeException);
	
	protected:
		friend class Tree;
	
		TreeNode *parent;	// pointer to node one level closer to root
		TreeNode *first_child;	// pointer to first child in linked list
		TreeNode *next;	// pointer to next node on same level
		TreeNode *prev;	// pointer to previous node on same level
		int num_children;	// number of children this node has
		int depth;	// depth of this node, 0=root

		void setDepth(int d);	// init the depth of this node to d
	};

class TreeIterator;		// forward declaration

class Tree {
	public:
		Tree();
		Tree(TreeNode *_root) : root(0), num_nodes(0) {
			setRoot(_root);
		}
		virtual ~Tree();
	
	// Accessors
		int getMaxDepth();	// maximum depth of tree, or -1 if empty
		TreeNode *getRoot() {
			return root;
		}
		int getNumNodes() {
			return num_nodes;
		}
		void print();			// print tree
	
	// Mutators
		void clear();			// destroy tree
		virtual void addChild(TreeNode *parent, TreeNode *child);
		virtual void addChild(TreeNode *parent, Tree *child);
		virtual void setRoot(TreeNode *_root);
		virtual void deleteNode(TreeNode *node);	// delete node pointed to
		virtual void deleteNode(TreeNode& node);	// find the node to delete
	
	protected:
		TreeNode *root;	// pointer to root of tree, or 0 if empty
		int num_nodes;
		void print_subtree(const char *leader, int depth);
	};	

// ******* Iterator class **********
enum TreeTraversalMode {
	PRE_ORDER,
	POST_ORDER,
	LEVEL_ORDER	// i.e. Breadth-first
};

class TreeIterator {
	public:
		TreeIterator(Tree * tree, TreeTraversalMode m);

		TreeNode *operator->() { return current_node; }
		operator TreeNode*() { return current_node; }
		void operator++();
		void operator++(int);	// identical to operator++()
		bool end() const { return current_node==0; }

	private:
		TreeTraversalMode mode;
		TreeNode *current_node;
		deque<TreeNode *> *container;
};
#endif	//_TREE_H
