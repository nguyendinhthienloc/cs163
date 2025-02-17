#include "linkedlist.h"
#include <iostream>

LinkedList::LinkedList() {
    head = nullptr;
}

LinkedList::~LinkedList() {
    clear();
}

void LinkedList::insertAtBeginning(int value) {
    Node* newNode = new Node{value, head};
    head = newNode;
}

void LinkedList::insertAtEnd(int value) {
    Node* newNode = new Node{value, nullptr};
    if (!head) {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void LinkedList::insertAtPosition(int value, int position) {
    if (position <= 0) {
        insertAtBeginning(value);
        return;
    }

    Node* newNode = new Node{value, nullptr};
    Node* temp = head;
    for (int i = 0; temp && i < position - 1; i++) {
        temp = temp->next;
    }

    if (!temp) {
        insertAtEnd(value);
        return;
    }

    newNode->next = temp->next;
    temp->next = newNode;
}

void LinkedList::deleteAtBeginning() {
    if (!head) return;
    Node* temp = head;
    head = head->next;
    delete temp;
}

void LinkedList::deleteAtEnd() {
    if (!head) return;
    if (!head->next) {
        delete head;
        head = nullptr;
        return;
    }
    Node* temp = head;
    while (temp->next->next) {
        temp = temp->next;
    }
    delete temp->next;
    temp->next = nullptr;
}

void LinkedList::deleteAtPosition(int position) {
    if (!head) return;
    if (position == 0) {
        deleteAtBeginning();
        return;
    }
    Node* temp = head;
    for (int i = 0; temp->next && i < position - 1; i++) {
        temp = temp->next;
    }
    if (!temp->next) return;
    Node* toDelete = temp->next;
    temp->next = temp->next->next;
    delete toDelete;
}

bool LinkedList::search(int value) {
    Node* temp = head;
    while (temp) {
        if (temp->data == value) return true;
        temp = temp->next;
    }
    return false;
}

std::vector<int> LinkedList::getListValues() {
    std::vector<int> values;
    Node* temp = head;
    while (temp) {
        values.push_back(temp->data);
        temp = temp->next;
    }
    return values;
}

void LinkedList::clear() {
    while (head) {
        deleteAtBeginning();
    }
}
