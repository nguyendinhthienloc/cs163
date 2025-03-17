#ifndef AVLTREE_H
#define AVLTREE_H

#include <vector>

// Node structure for the AVL tree
struct Node {
    int data;
    Node* left;
    Node* right;

    Node(int val);
};

// AVLTree class with core operations and visualization support
class AVLTree {
public:
    Node* root;

    void destroyTree(Node* node);
    Node* copyTree(const Node* node);

    //constructor & destructor
    AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree& other);
    AVLTree& operator=(const AVLTree& other);

    int getHeight(Node* node) const;
    int getBalanceFactor(Node* node);
    Node* minValueNode(Node* node);


    void rightRotate(Node*& root);
    void leftRotate(Node*& root);
    void insert(Node*& node, int value);
    void remove(Node*& node, int value);
    bool search(int value) const;
    std::vector<Node*> getInsertionPath(int value);

};

#endif // AVLTREE_H