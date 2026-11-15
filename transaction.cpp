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
        while (head) 
        {
            TransactionNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void addTransaction(string giver, string receiver, string itemId, string action, bool saveToDisk = true) 
    {
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

        if (saveToDisk) 
        {                // 3. file me save
            ofstream out("transactions.txt", ios::app);
            out << currentId << " " << giver << " " << receiver << " " << itemId << " " << action << "\n";
            out.close();
        }
    }

    void loadFromFile()   // makes list and map in the ram(current) by reading the transaction.txt file
    {
        ifstream in("transactions.txt");
        if (!in) return;     // in is true when file cannot be opened 
        int id;
        string giver, receiver, item, action;

        while (in >> id >> giver >> receiver >> item >> action)  // >> extraction operator read words 
        {
              txCounter = id;
            addTransaction(giver, receiver, item, action, false); // false: dobara file me mat likho
        }
        in.close();
    }

    void showHistory() const {
        cout << "\n  --- HISTORY (linked list) ---\n";
        if (!head) 
        {
            cout << "  No transactions yet.\n";
        } 
        else 
        {
            for (TransactionNode* temp = head; temp; temp = temp->next) {
                cout << "  TX#" << temp->txId << " | " << temp->itemId
                     << " | " << temp->giverId << " -> " << temp->receiverId
                     << " | " << temp->status << "\n";
            }
        }

        cout << "\n  --- CURRENTLY BORROWED (hash map) ---\n";
        if (activeLendMap.empty()) 
        {
            cout << "  All items are with their owners.\n";
        } 
        else 
        {
            DynamicArray<string> items;
            activeLendMap.getKeys(items);     // filling items vextor with help of active lend map
            for (string item : items)
                cout << "  " << item << " is with " << activeLendMap.get(item) << "\n";
        }
    }

    string getBorrower(string itemId) const 
    { 
        return activeLendMap.get(itemId); 
    }   // "" = kisi ke paas nahi


    int borrowedCount() const 
    { 
        return activeLendMap.size(); 
    }

    // Student item requests
    void addItemRequest(string studentId, string category, int priority) 
    {
        itemQueue.push({studentId, category, priority, requestCounter++});
    }


    bool hasItemRequests() const 
    { 
        return !itemQueue.empty(); 
    }


    ItemRequest popItemRequest() 
    {       // pehle hasItemRequests() check karna
        ItemRequest top = itemQueue.top();
        itemQueue.pop();
        return top;
    }

    // Teacher room requests 
    void addRoomRequest(string teacherId, string day, string time, string purpose, int priority) 
    {
        roomQueue.push({teacherId, day, time, purpose, priority, requestCounter++});
    }
    bool hasRoomRequests() const 
    { 
       
        return !roomQueue.empty(); 
    }
    RoomRequest popRoomRequest() 
    {       // pehle hasRoomRequests() check karna
        RoomRequest top = roomQueue.top();
        roomQueue.pop();
        return top;
    }

    int pendingRequests() const 
    { 
        return itemQueue.size() + roomQueue.size(); 
    }
};



/*    demo main for my module

void line(string title) {
    cout << "\n==================== " << title << " ====================\n";
}

int main() {
    remove("transactions.txt");   // DEMO ke liye: purani file hatao, fresh start

    {   // ---------- PART A: pehla run ----------
        TransactionManager tm;                         // constructor chala

        line("1. START ");
        tm.loadFromFile();                             // file nahi hai, to kuch nahi hoga
        tm.showHistory();

        line("2. ITEMS BORROW HUE");
        tm.addTransaction("S101", "S205", "ITEM_042", "BORROWED");
        tm.addTransaction("S102", "S207", "ITEM_010", "BORROWED");
        cout << "  ITEM_042 kiske paas? " << tm.getBorrower("ITEM_042") << "\n";
        cout << "  ITEM_999 kiske paas? \"" << tm.getBorrower("ITEM_999") << "\" (khaali = available)\n";
        cout << "  Kitne items bahar hain? " << tm.borrowedCount() << "\n";

        line("3. ITEM_042 WAPAS AAYA");
        tm.addTransaction("S205", "S101", "ITEM_042", "RETURNED");
        cout << "  ITEM_042 kiske paas? \"" << tm.getBorrower("ITEM_042") << "\"\n";
        tm.showHistory();

        line("4. STUDENT ITEM REQUESTS");
        tm.addItemRequest("S301", "Calculator", 2);    // Normal
        tm.addItemRequest("S302", "Calculator", 10);   // Exam
        tm.addItemRequest("S303", "DrawingKit", 8);    // Assignment
        tm.addItemRequest("S304", "Calculator", 10);   // Exam (S302 ke baad aaya)
        cout << "  Pending requests: " << tm.pendingRequests() << "\n";
        cout << "  Heap se nikalne ka order:\n";
        while (tm.hasItemRequests()) {
            ItemRequest r = tm.popItemRequest();
            cout << "    " << r.studentId << " | " << r.category
                 << " | priority " << r.priority << " | order " << r.order << "\n";
        }

        line("5. TEACHER ROOM REQUESTS");
        tm.addRoomRequest("T01", "Mon", "10AM", "Doubt", 4);
        tm.addRoomRequest("T02", "Mon", "11AM", "Quiz", 10);
        tm.addRoomRequest("T03", "Tue", "2PM", "ExtraClass", 8);
        cout << "  Pending requests: " << tm.pendingRequests() << "\n";
        cout << "  Heap se nikalne ka order:\n";
        while (tm.hasRoomRequests()) {
            RoomRequest r = tm.popRoomRequest();
            cout << "    " << r.teacherId << " | " << r.day << " " << r.time
                 << " | " << r.purpose << " | priority " << r.priority << "\n";
        }
        cout << "  Pending requests ab: " << tm.pendingRequests() << "\n";
    }   // yahan tm khatam, destructor chala, RAM saaf

    {   // ---------- PART B: program "restart" ----------
        line("6. RESTART: NAYA OBJECT, FILE SE LOAD");
        TransactionManager tm2;
        tm2.loadFromFile();                            // history wapas aayi
        tm2.showHistory();

        line("7. RESTART KE BAAD NAYA TRANSACTION");
        tm2.addTransaction("S103", "S210", "ITEM_077", "BORROWED");
        tm2.showHistory();                             // TX#4 banna chahiye, TX#1 nahi
    }

    return 0;
}

*/