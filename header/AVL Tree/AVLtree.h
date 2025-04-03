#ifndef AVLTREE_H
#define AVLTREE_H

#include <vector>
#include <algorithm>
#include <queue>

// AVLNode structure for the AVL tree
struct AVLNode {
    int data;
    AVLNode* left;
    AVLNode* right;

    AVLNode(int val);
};

// AVLTree class with core operations and visualization support
class AVLTree {
public:
    AVLNode* root;

    void destroyTree(AVLNode* node);
    AVLNode* copyTree(const AVLNode* node);

    //constructor & destructor
    AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree& other);
    AVLTree& operator=(const AVLTree& other);

    int getHeight(AVLNode* node) const;
    int getBalanceFactor(AVLNode* node);
    float getSubtreeWidth(AVLNode* node);
    AVLNode* minValueNode(AVLNode* node);


    void rightRotate(AVLNode*& root);
    void leftRotate(AVLNode*& root);
    void insert(AVLNode*& node, int value);
    void remove(AVLNode*& node, int value);
    bool search(int value);
    std::vector<AVLNode*> getInsertionPath(int value);

};

#endif // AVLTREE_H