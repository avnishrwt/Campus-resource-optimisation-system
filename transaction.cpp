// MODULE 4: REQUEST & TRANSACTION MODULE (Database: transactions.txt)
// Linked list = len-den history, HashMap = abhi item kiske paas hai,
// 2 MaxHeaps = student item requests aur teacher room requests
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "DataStructures.cpp"
using namespace std;

struct ItemRequest 
{
    string studentId, category;
    int priority;   // 10 = Exam, 8 = Assignment, 2 = Normal
    int order;      // same priority pe jo pehle aaya use pehle milega
    bool operator<(const ItemRequest& other) const 
    {
        if (priority != other.priority) return priority < other.priority;
        return order > other.order;
    }
};

struct RoomRequest 
{
    string teacherId, day, time, purpose;
    int priority;   // 10 = Quiz, 8 = Extra Class, 4 = Doubt
    int order;
    bool operator<(const RoomRequest& other) const 
    {
        if (priority != other.priority) return priority < other.priority;
        return order > other.order;
    }
};

struct TransactionNode 
{
    int txId;
    string giverId, receiverId, itemId, status;
    TransactionNode* next;

    TransactionNode(int id, string from, string to, string item, string st)
        : txId(id), giverId(from), receiverId(to), itemId(item), status(st), next(nullptr) {}
};

class TransactionManager 
{
    TransactionNode* head;
    TransactionNode* tail;               // last node, isse naya node O(1) me judta hai
    HashMap<string> activeLendMap;       // ItemID -> BorrowerID
    MaxHeap<ItemRequest> itemQueue;
    MaxHeap<RoomRequest> roomQueue;
    int txCounter;
    int requestCounter;

public:
    TransactionManager() : head(nullptr), tail(nullptr), txCounter(1), requestCounter(0) {}

    ~TransactionManager() 
    {
        while (head) {
            TransactionNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // ---------------- Len-Den ----------------
    void addTransaction(string giver, string receiver, string itemId, string action, bool saveToDisk = true) {
        int currentId = txCounter++;
        TransactionNode* newNode = new TransactionNode(currentId, giver, receiver, itemId, action);

        if (!head) {                     // 1. list ke end me jodo
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }

        if (action == "BORROWED") {      // 2. map update
            activeLendMap.put(itemId, receiver);
        } else if (action == "RETURNED") {
            activeLendMap.remove(itemId);
        }

        if (saveToDisk) {                // 3. file me save
            ofstream out("transactions.txt", ios::app);
            out << currentId << " " << giver << " " << receiver << " " << itemId << " " << action << "\n";
            out.close();
        }
    }

    void loadFromFile() {
        ifstream in("transactions.txt");
        if (!in) return;
        int id;
        string giver, receiver, item, action;
        while (in >> id >> giver >> receiver >> item >> action) {
            txCounter = id;
            addTransaction(giver, receiver, item, action, false);   // false: dobara file me mat likho
        }
        in.close();
    }

    void showHistory() const {
        cout << "\n  --- LEN-DEN HISTORY (linked list) ---\n";
        if (!head) {
            cout << "  No transactions yet.\n";
        } else {
            for (TransactionNode* temp = head; temp; temp = temp->next) {
                cout << "  TX#" << temp->txId << " | " << temp->itemId
                     << " | " << temp->giverId << " -> " << temp->receiverId
                     << " | " << temp->status << "\n";
            }
        }

        cout << "\n  --- CURRENTLY BORROWED (hash map) ---\n";
        if (activeLendMap.empty()) {
            cout << "  All items are with their owners.\n";
        } else {
            DynamicArray<string> items;
            activeLendMap.getKeys(items);
            for (string item : items)
                cout << "  " << item << " is with " << activeLendMap.get(item) << "\n";
        }
    }

    string getBorrower(string itemId) const { return activeLendMap.get(itemId); }   // "" = kisi ke paas nahi
    int borrowedCount() const { return activeLendMap.size(); }

    // ---------------- Student item requests ----------------
    void addItemRequest(string studentId, string category, int priority) {
        itemQueue.push({studentId, category, priority, requestCounter++});
    }
    bool hasItemRequests() const { return !itemQueue.empty(); }
    ItemRequest popItemRequest() {       // pehle hasItemRequests() check karna
        ItemRequest top = itemQueue.top();
        itemQueue.pop();
        return top;
    }

    // ---------------- Teacher room requests ----------------
    void addRoomRequest(string teacherId, string day, string time, string purpose, int priority) {
        roomQueue.push({teacherId, day, time, purpose, priority, requestCounter++});
    }
    bool hasRoomRequests() const { return !roomQueue.empty(); }
    RoomRequest popRoomRequest() {       // pehle hasRoomRequests() check karna
        RoomRequest top = roomQueue.top();
        roomQueue.pop();
        return top;
    }

    int pendingRequests() const { return itemQueue.size() + roomQueue.size(); }
};
