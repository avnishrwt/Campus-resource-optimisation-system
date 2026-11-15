// MODULE 2: SHARED RESOURCE MODULE (Database: resources.txt)
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "DataStructures.cpp"
using namespace std;

class SharedResource {
    string resId, itemName, category, ownerStudentId, status;
    int durationHours, condition;
public:
    SharedResource(string id, string name, string cat, string owner, int dur, int cond, string st = "AVAILABLE")
        : resId(id), itemName(name), category(cat), ownerStudentId(owner), status(st),
          durationHours(dur), condition(cond) {}

    string getId() const { return resId; }
    string getName() const { return itemName; }
    string getCategory() const { return category; }
    string getOwner() const { return ownerStudentId; }
    string getStatus() const { return status; }
    int getDuration() const { return durationHours; }
    int getCondition() const { return condition; }

    void setStatus(string s) { status = s; }
    void setCondition(int c) { condition = c; }

    void show() const {
        cout << "  [" << category << "] " << resId << " | " << itemName
             << " | Owner: " << ownerStudentId << " | Max " << durationHours << " hrs"
             << " | Condition: " << condition << "% | " << status << "\n";
    }

    string toFileLine() const {
        return resId + " " + itemName + " " + category + " " + ownerStudentId + " " +
               to_string(durationHours) + " " + to_string(condition) + " " + status;
    }
};

class ResourceManager {
    DynamicArray<SharedResource*> resources;   // order ke saath list
    HashMap<SharedResource*> resourceMap;      // ID -> item, O(1) search
public:
    ~ResourceManager() {
        for (SharedResource* r : resources) delete r;
    }

    void saveToFile() const {
        ofstream out("resources.txt");
        for (SharedResource* r : resources) out << r->toFileLine() << "\n";
        out.close();
    }

    void loadFromFile() {
        ifstream in("resources.txt");      // file nahi hai to loop chalega hi nahi
        string id, name, cat, owner, status;
        int dur, cond;
        while (in >> id >> name >> cat >> owner >> dur >> cond >> status) {
            addItem(id, name, cat, owner, dur, cond, status, false);
        }
        in.close();

        if (resources.empty()) {   // file nahi thi ya khaali thi: sample items
            addItem("CAL01", "Casio_991EX", "Calculator", "S101", 24, 95, "AVAILABLE", false);
            addItem("BK01", "Cormen_DSA", "Book", "S103", 48, 88, "AVAILABLE", false);
            addItem("CHG01", "TypeC_65W", "Charger", "S101", 4, 90, "AVAILABLE", false);
            saveToFile();
        }
    }

    bool addItem(string id, string name, string cat, string owner, int dur, int cond,
                 string st = "AVAILABLE", bool save = true) {
        if (resourceMap.contains(id)) return false;      // ID pehle se hai
        SharedResource* r = new SharedResource(id, name, cat, owner, dur, cond, st);
        resources.push_back(r);
        resourceMap.put(id, r);
        if (save) saveToFile();
        return true;
    }

    SharedResource* findItem(string id) const {          // O(1)
        return resourceMap.get(id);
    }

    // is category ka pehla AVAILABLE item jo maangne wale ka apna na ho
    SharedResource* findAvailable(string category, string requesterId) const {
        for (SharedResource* r : resources) {
            if (r->getCategory() == category && r->getStatus() == "AVAILABLE" &&
                r->getOwner() != requesterId)
                return r;
        }
        return nullptr;
    }

    void showAllItems() const {
        if (resources.empty()) { cout << "  No items shared yet.\n"; return; }
        for (SharedResource* r : resources) r->show();
    }

    int count() const { return resources.size(); }
};
