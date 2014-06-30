#include "Tree.h"
#include <string.h>

const char *TREENODE_STRING_DFL = "[TreeNode]";

TreeNode::TreeNode() {
	parent = 0;
	first_child = 0;
	next = prev = 0;
	num_children = 0;
	depth = 0;
};

TreeNode::~TreeNode() {
}	// empty for now, nothing to dstroy

TreeNode *TreeNode::getLastChild() {
	return first_child ? first_child->getLastSibling() : 0;
}

TreeNode *TreeNode::getFirstSibling() {
	if(parent)
		return parent->getFirstChild();
	else
		return 0;
}

TreeNode *TreeNode::getLastSibling() {
	if(!parent) 
		return 0;
	TreeNode *nodeptr = this;
	if(nodeptr)
		while(nodeptr->next)
			nodeptr = nodeptr->next;
	return nodeptr;
}

TreeNode *TreeNode::getNextSibling() {
	return next;
}

TreeNode *TreeNode::getPrevSibling() {
	return prev;
}

int TreeNode::getMaxDepth() {
	int max_depth = depth;
	int temp_depth;
	TreeNode *nodeptr = getFirstChild();
	while(nodeptr) {
		temp_depth = nodeptr->getMaxDepth();
		if(temp_depth > max_depth)
			max_depth = temp_depth;
		nodeptr = nodeptr->next;
	}
	return max_depth;
}

int TreeNode::getNumDescendants() {
	int num_descendants = 0;
	TreeNode *nodeptr = getFirstChild();
	while(nodeptr) {
		num_descendants += nodeptr->getNumDescendants() + 1;
		nodeptr = nodeptr->next;
	}
	return num_descendants;
}

void TreeNode::addChild(TreeNode *node) throw (TreeNodeException) {
		// is this a root node with no siblings?
	if(node->parent || node->next || node->prev) {
		throw TREENODE_EXCEPTION_BAD_ARGS;
	}

		// modify (*this) members
	num_children++;

		// modify node members
	node->parent = this;
	node->next = 0;
	TreeNode *nodeptr = getLastChild();
	node->prev = nodeptr;
	node->setDepth(depth+1);

		// modify children members
	if(nodeptr)
		nodeptr->next = node;
	else
		first_child = node;
}

void TreeNode::setDepth(int d) {
	depth = d;
	TreeNode *nodeptr = getFirstChild();
		// I really need an iterator function
	while(nodeptr) {
		nodeptr->setDepth(depth+1);
		nodeptr = nodeptr->next;
	}
}

const char *TreeNode::toString() {
	return TREENODE_STRING_DFL;
}

bool TreeNode::operator==(const TreeNode &n) {
	return compare(n) == 0;
}

bool TreeNode::operator!=(const TreeNode &n) {
	return compare(n) !=  0;
}

bool TreeNode::operator<(const TreeNode &n) {
	return compare(n) < 0;
}

bool TreeNode::operator<=(const TreeNode &n) {
	return !(compare(n) > 0);
}

bool TreeNode::operator>(const TreeNode &n) {
	return compare(n) > 0;
}

bool TreeNode::operator>=(const TreeNode &n) {
	return !(compare(n) < 0);
}


Tree::Tree() {
	root = 0;
	num_nodes = 0;
};

Tree::~Tree() {
	clear();
}

// Accessors
int Tree::getMaxDepth() {	// maximum depth of tree, or -1 if empty
	return root ? root->getMaxDepth() : -1;
}

// Mutators
void Tree::clear() {			// destroy tree
	if(num_nodes) {
		delete root;
		num_nodes = 0;
		root = 0;
	}
}

void Tree::addChild(TreeNode *parent, TreeNode *childnode) {
	parent->addChild(childnode);
	num_nodes++;
}

void Tree::addChild(TreeNode *parent, Tree *childtree) {
	parent->addChild(childtree->getRoot());
	num_nodes += childtree->getRoot()->getNumDescendants();
}

void Tree::setRoot(TreeNode *_root) {
	clear();
	root = _root;
}

void Tree::deleteNode(TreeNode *) {	// delete node pointed to
}

void Tree::deleteNode(TreeNode& ) {	// find the node to delete
}

void Tree::print() {						// print tree structure
	print_subtree("", 0);
}

void Tree::print_subtree( const char *leader, int depth) {
	TreeNode *nodeptr;	/* pointer to current node */
	const int BUFSIZE=depth*4+1;
	char *buf = new char[BUFSIZE];	// char buffer
	if(!buf) return;
	char *cp;

	if(depth) {
		cout << leader; 	// print leader string
		if(root->getNextSibling())
			cout << " +--";
		else
			cout << " `--";
	}
	cout << root->toString() << endl;;	// print tree node
	nodeptr = root->getFirstChild();
	strncpy(buf, leader, BUFSIZE);
	cp = buf + strlen(buf);
	while(nodeptr) {
		if(depth) {
			if(root->getNextSibling())
				strncpy(cp, " |  ", BUFSIZE - (cp - buf));
			else
				strncpy(cp, "    ", BUFSIZE - (cp - buf));
		}
		((Tree)(nodeptr)).print_subtree(buf, depth+1);
		nodeptr = nodeptr->getNextSibling();
	}
	delete buf;
}

void postorder(TreeNode *tn, deque<TreeNode *> *container) {
	TreeNode *child = tn->getFirstChild();
	while(child) {
		postorder(child, container);
		child = child->getNextSibling();
	}
	container->push_front(tn);
}

TreeIterator::TreeIterator(Tree * tree, TreeTraversalMode m) {
	mode = m;
	current_node = tree->getRoot();
	container = new deque<TreeNode *>();

	if(mode == POST_ORDER) {
		postorder(current_node, container);
		current_node = container->back();
		container->pop_back();
	} else if(mode == LEVEL_ORDER) {
		for(TreeNode *tn = current_node->getFirstChild(); tn; tn = tn->getNextSibling()) {
			container->push_front(tn);
		}
	}
}

void TreeIterator::operator++() {
	if(current_node) {
		switch(mode) {
			case PRE_ORDER:
				if(current_node->getFirstChild())
					current_node = current_node->getFirstChild();
				else {
					while(current_node) {
						if(current_node->getNextSibling()) {
							current_node = current_node->getNextSibling();
							break;
						} else
							current_node = current_node->getParent();
					}
				}
				break;

			case POST_ORDER:
				if(container->size()) {
					current_node = container->back();
					container->pop_back();
				} else
					current_node = 0;
				break;

			case LEVEL_ORDER:
				if(container->size()) {
					current_node = container->back();
					container->pop_back();
					for(TreeNode *tn = current_node->getFirstChild(); tn; tn = tn->getNextSibling()) {
						container->push_front(tn);
					}
				} else
					current_node = 0;
				break;
		}
	}
}

void TreeIterator::operator++(int) {
	operator++();	// can't implement postincrement at the moment
}

