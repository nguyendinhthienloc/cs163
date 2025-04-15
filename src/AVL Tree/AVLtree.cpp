#include "../../header/AVL Tree/AVLtree.h"


// AVLNode constructor
AVLNode::AVLNode(int data) : data(data), left(nullptr), right(nullptr) {}

// AVLTree implementation
AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    destroyTree(root);
}

AVLTree::AVLTree(const AVLTree& other) : root(nullptr) {
    root = copyTree(other.root);
}

AVLTree& AVLTree::operator=(const AVLTree& other) {
    if (this != &other) {
        destroyTree(root);
        root = copyTree(other.root);
    }
    return *this;
}

void AVLTree::destroyTree(AVLNode* node) {
    if (!node) return;

    destroyTree(node->left);
    node->left = nullptr;
    destroyTree(node->right);
    node->right = nullptr;

    delete node;
}

AVLNode* AVLTree::copyTree(const AVLNode* node) {
    if (!node) return nullptr;
    AVLNode* newNode = new AVLNode(node->data);
    newNode->left = copyTree(node->left);
    newNode->right = copyTree(node->right);
    return newNode;
}

int AVLTree::getHeight(AVLNode* node) const {
    if (!node) return 0;

    int left = getHeight(node->left);
    int right = getHeight(node->right);

    return (left >= right ? left : right) + 1;
}

int AVLTree::getBalanceFactor(AVLNode* node) {
    if (!node) return 0;

    return getHeight(node->left) - getHeight(node->right);
}

float AVLTree::getSubtreeWidth(AVLNode* node) {
    if (!node) return 50.0f;

    float leftWidth = getSubtreeWidth(node->left);
    float rightWidth = getSubtreeWidth(node->right);

    return leftWidth + rightWidth + 40.0f;
}

AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left) {
        current = current->left;
    }
    return current;
}

void AVLTree::rightRotate(AVLNode*& node) {
    AVLNode* newNode = node->left;
    AVLNode* subRight = newNode->right;

    //begin rotate
    newNode->right = node;
    node->left = subRight;
    node = newNode;

}

void AVLTree::leftRotate(AVLNode*& node) {
    AVLNode* newNode = node->right;
    AVLNode* subLeft = newNode->left;

    //begin rotate
    newNode->left = node;
    node->right = subLeft;
    node = newNode;

}

void AVLTree::insert(AVLNode*& node, int value) {
    if (!node) {
        node = new AVLNode(value);
        return;
    }

    if (value < node->data) {
        insert(node->left, value);
    }
    else if (value > node->data) {
        insert(node->right, value);
    }
    else {   //value == root->data
        return;
    }

    //check for possible rotation
    int bf = getHeight(node->left) - getHeight(node->right);

    //case L-L
    if (bf >= 2 && value < node->left->data) {
        rightRotate(node);
        return;
    }

    //case R-R
    if (bf <= -2 && value > node->right->data) {
        leftRotate(node);
        return;
    }

    //case L-R
    if (bf >= 2 && value > node->left->data) {
        leftRotate(node->left);
        rightRotate(node);
        return;
    }

    //case R-L
    if (bf <= -2 && value < node->right->data) {
        rightRotate(node->right);
        leftRotate(node);
        return;
    }
}

void AVLTree::remove(AVLNode*& node, int value) {
    if (!node) return;

    //normal bst deletion

    if (value > node->data) {
        remove(node->right, value);
    }

    else if (value < node->data){
        remove(node->left, value);
    }
    else {
        if (!node->left) {
            AVLNode* temp = node;
            node = node->right;
            delete temp;
        }

        else if (!node->right) {
            AVLNode* temp = node;
            node = node->left;
            delete temp;
        }

        else {
            AVLNode* curr = minValueNode(node->right);
            node->data = curr->data;
            remove(node->right, curr->data);
        }
    }

    if (!node) return;

    //check for possible rotation
    int bf = getBalanceFactor(node);

    if (bf > 1 && getBalanceFactor(node->left) >= 0) {
        rightRotate(node);
        return;
    }

    if (bf > 1 && getBalanceFactor(node->left) == -1) {
        leftRotate(node->left);
        rightRotate(node);
        return;
    }

    if (bf < -1 && getBalanceFactor(node->right) <= 0) {
        leftRotate(node);
        return;
    }

    if (bf < -1 && getBalanceFactor(node->right) == 1) {
        rightRotate(node->right);
        leftRotate(node);
        return;
    }  
}

bool AVLTree::search(int value) {
    AVLNode* current = root;
    while (current) {
        if (value == current->data) return true;
        if (value < current->data) current = current->left;
        else current = current->right;
    }
    return false;
}

std::vector<AVLNode*> AVLTree::getInsertionPath(int value) {
    std::vector<AVLNode*> path;
    AVLNode* current = root;

    while (current) {
        path.push_back(current);
        if (value < current->data) {
            current = current->left;
        }
        else if (value > current->data) {
            current = current->right;
        }
        else {
            break;
        }
    }
    return path;
}

