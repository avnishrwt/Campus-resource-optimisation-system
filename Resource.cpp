#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>  // treating string as stream

using namespace std;



// ============================================================================
// MODULE 2: PEER-TO-PEER RESOURCE MODULE (Database: resources.txt)
// ============================================================================
class SharedResource {
    string resId, itemName, category, ownerStudentId, status;
    int durationHours, condition;

public:
    SharedResource(string id, string name, string cat, string ownerId, int dur, int cond, string st = "AVAILABLE")
        : resId(id), itemName(name), category(cat), ownerStudentId(ownerId),
          durationHours(dur), condition(cond), status(st) {}

    string getId() const { return resId; }
    string getName() const { return itemName; }
    string getCategory() const { return category; }
    string getOwner() const { return ownerStudentId; }
    int getDuration() const { return durationHours; }
    int getCondition() const { return condition; }
    string getStatus() const { return status; }

    void setStatus(string s) { status = s; }
    void setCondition(int c) { condition = c; }

    void display() const {
        cout << "[" << category << "] ID: " << resId << " | " << itemName
             << " | Owner: " << ownerStudentId << " | MaxTime: " << durationHours << "hrs"
             << " | Cond: " << condition << "% | Status: " << status << "\n";
    }

    string toFileString() const {
        return resId + " " + itemName + " " + category + " " + ownerStudentId + " " +
               to_string(durationHours) + " " + to_string(condition) + " " + status;
    }
};