#pragma once
#include <string>
using namespace std;

template <typename T>    // template helps class and function do operations to generic functions. // typename
class DynamicArray      // typename makes a datatype T which is a generic type
{
    T* arr;
    int cap;
    int count;

    void grow()  // array full ho gaya to size double
    {                
        cap = cap * 2;
        T* bigger = new T[cap];
        for (int i = 0; i < count; i++) bigger[i] = arr[i];
        delete[] arr;
        arr = bigger;
    }

public:
    DynamicArray()    // constructor
    { 
        cap = 4; count = 0; arr = new T[cap]; 
    }
    ~DynamicArray()   // destructor
    { 
        delete[] arr; 
    }

    void push_back(T value) 
    {
        if (count == cap) 
        grow();
        arr[count] = value;
        count++;
    }

    int size() const 
    { 
        return count; 
    }

    bool empty() const 
    { 
        return count == 0; 
    
    }

    T& operator[](int i)   // operator overloading for [] -->because DynamicArray class ke object ke liye [] ky h compiler ko ni pta
    { 
        return arr[i]; 
    }
    const T& operator[](int i) const { return arr[i]; }

    T* begin() { return arr; }
    T* end() { return arr + count; }
    const T* begin() const { return arr; }
    const T* end() const { return arr + count; }
};


const int BUCKETS = 101;     // size of unordered map array of pointers 

template <typename V>
class HashMap 
{
    struct Node     // struct for the nodes of the linked list which will be pointed by pointers of array table[BUCKETS]
    {
        string key;
        V value;
        Node* next;
    };

    Node* table[BUCKETS];
    int count;

    int hashOf(string key) const      // calculating the index on which we will add or remove or find the keys 
    {
        unsigned long h = 0;
        for (int i = 0; i < (int)key.size(); i++)
            h = h * 31 + (unsigned char)key[i];
        return h % BUCKETS;
    }

public:
    HashMap() 
    {
        count = 0;
        for (int i = 0; i < BUCKETS; i++) table[i] = nullptr;
    }

    ~HashMap() 
    {
        for (int i = 0; i < BUCKETS; i++) 
        {
            Node* cur = table[i];
            while (cur) 
            {
                Node* nxt = cur->next;
                delete cur;
                cur = nxt;
            }
        }
    }

    void put(string key, V value) 
    {       
        int b = hashOf(key);                                    // index dhunda hashOf se 
        for (Node* cur = table[b]; cur; cur = cur->next)        // check krne ke liye ki linked list me koi same key hogi to usse overwrite
        {
            if (cur->key == key) { cur->value = value; return; }
        }
        table[b] = new Node{key, value, table[b]};              // agr koi linked list me esi node ni thi to starting me add krdo list ke 
        count++;
    }

    bool contains(string key) const 
    {
        for (Node* cur = table[hashOf(key)]; cur; cur = cur->next)
            if (cur->key == key) return true;
        return false;
    }

    V get(string key) const 
    {                
        for (Node* cur = table[hashOf(key)]; cur; cur = cur->next)
            if (cur->key == key) return cur->value;
        return V();
    }

    void remove(string key) 
    {
        int b = hashOf(key);
        Node* cur = table[b];
        Node* prev = nullptr;
        while (cur) {
            if (cur->key == key) 
            {
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

    void getKeys(DynamicArray<string>& out) const             // saari keys nikaalo aur jo out naamka vector(kisi aur verctor ka reference h) usme daal do
    {                                                         // used in showhistory()
        for (int i = 0; i < BUCKETS; i++)
            for (Node* cur = table[i]; cur; cur = cur->next)
                out.push_back(cur->key);
    }
};

template <typename T>
class MaxHeap 
{
    T* arr;
    int cap;
    int count;

    void grow()       // array full ho gaya to size double
    {
        cap = cap * 2;
        T* bigger = new T[cap];
        for (int i = 0; i < count; i++) bigger[i] = arr[i];
        delete[] arr;
        arr = bigger;
    }

    void swapItems(T& a, T& b) 
    { 
        T temp = a; 
        a = b; 
        b = temp; 
    
    }

    void heapifyUp(int i)      // tree me element daale smy swap kro jisse ek valid maxheap bn jaye
    {
        while (i > 0) 
        {
            int parent = (i - 1) / 2;
            if (arr[parent] < arr[i]) 
            {
                swapItems(arr[parent], arr[i]);
                i = parent;
            } 
            else 
            break;
        }
    }

    void heapifyDown(int i)     // pop krte time arr[0] pe last element le aao fir pure tree ko max heap ke according sort krdo 
    {
        while (true) 
        {
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

    void push(T value) 
    {
        if (count == cap) grow();
        arr[count] = value;
        heapifyUp(count);
        count++;
    }

    T top() const 
    { 
        return arr[0]; 
    }

    void pop() 
    {
        if (count == 0) return;
        arr[0] = arr[count - 1];
        count--;
        heapifyDown(0);
    }

    bool empty() const { return count == 0; }
    int size() const { return count; }
};
