#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <vector>

struct Node {
    int data;
    Node* next;
};

class LinkedList {
public:
    LinkedList();
    ~LinkedList();

    void insertAtBeginning(int value);
    void insertAtEnd(int value);
    void insertAtPosition(int value, int position);

    void deleteAtBeginning();
    void deleteAtEnd();
    void deleteAtPosition(int position);

    bool search(int value);
    std::vector<int> getListValues();

    void clear();

private:
    Node* head;
};

#endif // LINKEDLIST_H
