// DataStructures.cpp
// STL ki jagah apne data structures:
// DynamicArray = vector, HashMap = unordered_map, MaxHeap = priority_queue
#pragma once
#include <string>
using namespace std;

// ---------------- DynamicArray (vector jaisa) ----------------
template <typename T>
class DynamicArray {
    T* arr;
    int cap;
    int count;

    void grow() {                 // array full ho gaya to size double
        cap = cap * 2;
        T* bigger = new T[cap];
        for (int i = 0; i < count; i++) bigger[i] = arr[i];
        delete[] arr;
        arr = bigger;
    }

public:
    DynamicArray() { cap = 4; count = 0; arr = new T[cap]; }
    ~DynamicArray() { delete[] arr; }

    void push_back(T value) {
        if (count == cap) grow();
        arr[count] = value;
        count++;
    }

    int size() const { return count; }
    bool empty() const { return count == 0; }
    T& operator[](int i) { return arr[i]; }
    const T& operator[](int i) const { return arr[i]; }

    // inki wajah se "for (auto x : array)" chal jaata hai
    T* begin() { return arr; }
    T* end() { return arr + count; }
    const T* begin() const { return arr; }
    const T* end() const { return arr + count; }
};

// ---------------- HashMap (unordered_map<string, V> jaisa) ----------------
// key string hai, collision hone pe chaining (har bucket ek linked list)
const int BUCKETS = 101;

template <typename V>
class HashMap {
    struct Node {
        string key;
        V value;
        Node* next;
    };
    Node* table[BUCKETS];
    int count;

    int hashOf(string key) const {
        unsigned long h = 0;
        for (int i = 0; i < (int)key.size(); i++)
            h = h * 31 + (unsigned char)key[i];
        return h % BUCKETS;
    }

public:
    HashMap() {
        count = 0;
        for (int i = 0; i < BUCKETS; i++) table[i] = nullptr;
    }

    ~HashMap() {
        for (int i = 0; i < BUCKETS; i++) {
            Node* cur = table[i];
            while (cur) {
                Node* nxt = cur->next;
                delete cur;
                cur = nxt;
            }
        }
    }

    void put(string key, V value) {          // hai to update, nahi to naya
        int b = hashOf(key);
        for (Node* cur = table[b]; cur; cur = cur->next) {
            if (cur->key == key) { cur->value = value; return; }
        }
        table[b] = new Node{key, value, table[b]};
        count++;
    }

    bool contains(string key) const {
        for (Node* cur = table[hashOf(key)]; cur; cur = cur->next)
            if (cur->key == key) return true;
        return false;
    }

    V get(string key) const {                // nahi mila to nullptr / ""
        for (Node* cur = table[hashOf(key)]; cur; cur = cur->next)
            if (cur->key == key) return cur->value;
        return V();
    }

    void remove(string key) {
        int b = hashOf(key);
        Node* cur = table[b];
        Node* prev = nullptr;
        while (cur) {
            if (cur->key == key) {
                if (prev) prev->next = cur->next;
                else table[b] = cur->next;
                delete cur;
                count--;
                return;
            }
            prev = cur;
            cur = cur->next;
        }
    }

    int size() const { return count; }
    bool empty() const { return count == 0; }

    void getKeys(DynamicArray<string>& out) const {   // saari keys nikaalo
        for (int i = 0; i < BUCKETS; i++)
            for (Node* cur = table[i]; cur; cur = cur->next)
                out.push_back(cur->key);
    }
};

// ---------------- MaxHeap (priority_queue jaisa) ----------------
// sabse badi priority hamesha top (arr[0]) pe
template <typename T>
class MaxHeap {
    T* arr;
    int cap;
    int count;

    void grow() {
        cap = cap * 2;
        T* bigger = new T[cap];
        for (int i = 0; i < count; i++) bigger[i] = arr[i];
        delete[] arr;
        arr = bigger;
    }

    void swapItems(T& a, T& b) { T temp = a; a = b; b = temp; }

    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (arr[parent] < arr[i]) {
                swapItems(arr[parent], arr[i]);
                i = parent;
            } else break;
        }
    }

    void heapifyDown(int i) {
        while (true) {
            int left = 2 * i + 1, right = 2 * i + 2, biggest = i;
            if (left < count && arr[biggest] < arr[left]) biggest = left;
            if (right < count && arr[biggest] < arr[right]) biggest = right;
            if (biggest == i) break;
            swapItems(arr[i], arr[biggest]);
            i = biggest;
        }
    }

public:
    MaxHeap() { cap = 4; count = 0; arr = new T[cap]; }
    ~MaxHeap() { delete[] arr; }

    void push(T value) {
        if (count == cap) grow();
        arr[count] = value;
        heapifyUp(count);
        count++;
    }

    T top() const { return arr[0]; }

    void pop() {
        if (count == 0) return;
        arr[0] = arr[count - 1];
        count--;
        heapifyDown(0);
    }

    bool empty() const { return count == 0; }
    int size() const { return count; }
};
