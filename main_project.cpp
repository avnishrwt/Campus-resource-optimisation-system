// Compile: g++ main.cpp -o campus   (baaki .cpp files include ho jaati hain)
#include <iostream>
#include <string>
#include "user.cpp"
#include "transaction.cpp"
#include "TimeSpace.cpp"
#include "Resource.cpp"
using namespace std;

UserManager        users;
ResourceManager    items;
TimetableManager   timetable;
TransactionManager txManager;


int readInt(string msg) {
    int x;
    while (true) {
        cout << msg;
        if (cin >> x) return x;
        if (cin.eof()) return 0;               // input khatam -> exit
        cout << "  Please enter a number.\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

string readWord(string msg) {
    string s;
    cout << msg;
    cin >> s;
    return s;
}

string readDay() {
    printDayOptions();
    while (true) {
        string day = dayFromChoice(readInt("  Choose day: "));
        if (day != "" || !cin) return day;
        cout << "  Invalid choice.\n";
    }
}

string readTime() {
    printTimeOptions();
    while (true) {
        string time = timeFromChoice(readInt("  Choose slot: "));
        if (time != "" || !cin) return time;
        cout << "  Invalid choice.\n";
    }
}

string readCategory() {
    cout << "  1.Calculator  2.Book  3.Charger  4.Lab_Coat\n";
    while (true) {
        int c = readInt("  Choose category: ");
        if (c == 1) return "Calculator";
        if (c == 2) return "Book";
        if (c == 3) return "Charger";
        if (c == 4) return "Lab_Coat";
        if (!cin) return "";
        cout << "  Invalid choice.\n";
    }
}

void printTitle(string title) {
    cout << "\n==================================================\n";
    cout << "  " << title << "\n";
    cout << "==================================================\n";
}



void borrowItem(string studentId, string itemId) {
    if (!users.isStudent(studentId)) { cout << "  X Student " << studentId << " not found.\n"; return; }

    SharedResource* r = items.findItem(itemId);
    if (!r) { cout << "  X Item " << itemId << " not found.\n"; return; }
    if (r->getOwner() == studentId) { cout << "  X This is your own item.\n"; return; }
    if (r->getStatus() != "AVAILABLE") { cout << "  X Item is " << r->getStatus() << ".\n"; return; }

    r->setStatus("BORROWED");
    items.saveToFile();
    txManager.addTransaction(r->getOwner(), studentId, itemId, "BORROWED");
    cout << "  OK " << r->getName() << " given to " << studentId
         << " (owner " << r->getOwner() << "). Return within " << r->getDuration() << " hrs.\n";
}

void returnItem(string studentId, string itemId, int condition) {
    SharedResource* r = items.findItem(itemId);
    if (!r) { cout << "  X Item " << itemId << " not found.\n"; return; }
    if (txManager.getBorrower(itemId) != studentId) {
        cout << "  X " << itemId << " is not borrowed by " << studentId << ".\n";
        return;
    }
    if (condition < 1 || condition > 100) { cout << "  X Condition must be 1-100.\n"; return; }

    r->setCondition(condition);
    r->setStatus(condition < 50 ? "DAMAGED" : "AVAILABLE");
    items.saveToFile();
    txManager.addTransaction(studentId, r->getOwner(), itemId, "RETURNED");
    cout << "  OK " << r->getName() << " returned to " << r->getOwner() << ".\n";
    if (condition < 50) cout << "  ! Condition below 50%, item marked DAMAGED.\n";
}

void allocateItemToTopRequest() {
    if (!txManager.hasItemRequests()) { cout << "  No pending item requests.\n"; return; }

    ItemRequest req = txManager.popItemRequest();
    cout << "  Highest priority request: " << req.studentId << " needs a " << req.category
         << " (priority " << req.priority << ")\n";

    SharedResource* r = items.findAvailable(req.category, req.studentId);
    if (!r) { cout << "  X No " << req.category << " available right now. Request closed.\n"; return; }
    borrowItem(req.studentId, r->getId());
}

void allocateRoomToTopRequest() {
    if (!txManager.hasRoomRequests()) { cout << "  No pending room requests.\n"; return; }

    RoomRequest req = txManager.popRoomRequest();
    cout << "  Highest priority request: " << req.teacherId << " wants a room for " << req.purpose
         << " on " << req.day << " " << timeLabel(req.time) << " (priority " << req.priority << ")\n";

    string activity = "Booked_by_" + req.teacherId + "_" + req.purpose;
    ClassroomSpace* rm = timetable.bookFreeRoom(req.day, req.time, activity);
    if (!rm) { cout << "  X No free room at that time. Request closed.\n"; return; }

    txManager.addTransaction("ADMIN", req.teacherId, rm->getId() + "_" + req.day + "_" + req.time, "ROOM_BOOKED");
    cout << "  OK " << rm->getName() << " booked. timetable.txt updated.\n";
}

// ============================================================================
// MENUS
// ============================================================================
void showDashboard() {
    printTitle("CAMPUS DASHBOARD");
    cout << "  Users (users.txt)              : " << users.count() << "\n";
    cout << "  Shared items (resources.txt)   : " << items.count() << "\n";
    cout << "  Items currently borrowed       : " << txManager.borrowedCount() << "\n";
    cout << "  Classrooms (timetable.txt)     : " << timetable.count() << "\n";
    cout << "  Pending priority requests      : " << txManager.pendingRequests() << "\n";
}

void userMenu() {
    while (true) {
        printTitle("MODULE 1: USERS");
        cout << "  1. Show all users\n  2. Find user by ID\n  3. Add student\n  4. Add teacher\n  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) return;

        string id, name, dept, sub, slot;
        if (ch == 1) {
            users.showAllUsers();
        } else if (ch == 2) {
            id = readWord("  User ID (e.g. S101, T201): ");
            User* u = users.findUser(id);
            if (u) u->showProfile();
            else cout << "  X User not found.\n";
        } else if (ch == 3) {
            id = readWord("  Student ID: ");
            name = readWord("  Name (no spaces): ");
            dept = readWord("  Dept: ");
            int sem = readInt("  Semester: ");
            if (users.addStudent(id, name, dept, sem)) cout << "  OK Student saved.\n";
            else cout << "  X ID already exists.\n";
        } else if (ch == 4) {
            id = readWord("  Teacher ID: ");
            name = readWord("  Name (no spaces): ");
            dept = readWord("  Dept: ");
            sub = readWord("  Subject (no spaces): ");
            slot = readWord("  Doubt slot (e.g. 02:10): ");
            if (users.addTeacher(id, name, dept, sub, slot)) cout << "  OK Teacher saved.\n";
            else cout << "  X ID already exists.\n";
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

void itemMenu() {
    while (true) {
        printTitle("MODULE 2: SHARED ITEMS");
        cout << "  1. Show all items\n  2. Share your item\n  3. Search item by ID (hash map)\n  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) return;

        if (ch == 1) {
            items.showAllItems();
        } else if (ch == 2) {
            string owner = readWord("  Your Student ID: ");
            if (!users.isStudent(owner)) { cout << "  X Student not found.\n"; continue; }
            string id = readWord("  Item ID (e.g. BK02): ");
            string name = readWord("  Item name (use_underscore): ");
            string cat = readCategory();
            int dur = readInt("  Max borrow time (hours): ");
            int cond = readInt("  Condition (1-100): ");
            if (items.addItem(id, name, cat, owner, dur, cond)) cout << "  OK Item saved.\n";
            else cout << "  X Item ID already exists.\n";
        } else if (ch == 3) {
            SharedResource* r = items.findItem(readWord("  Item ID: "));
            if (r) r->show();
            else cout << "  X Item not found.\n";
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

void roomMenu() {
    while (true) {
        printTitle("MODULE 3: CLASSROOMS (B.Tech CSE Sem III)");
        cout << "  1. Find free rooms (student: study)\n  2. Find free rooms (teacher: extra class)\n"
             << "  3. Show one room's timetable\n  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) return;

        if (ch == 1 || ch == 2) {
            string day = readDay();
            string time = readTime();
            timetable.showFreeRooms(day, time, ch == 1 ? "Student" : "Teacher");
        } else if (ch == 3) {
            timetable.showRoomList();
            timetable.showRoomTimetable(readWord("  Room ID: "));
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

void transactionMenu() {
    while (true) {
        printTitle("MODULE 4: REQUESTS & TRANSACTION");
        cout << "  1. Borrow an item\n  2. Return an item\n"
             << "  3. Student: request item (priority)\n  4. Allocate item to top request\n"
             << "  5. Teacher: request room (priority)\n  6. Allocate room to top request\n"
             << "  7. Show transaction history\n  0. Back\n";
        int ch = readInt("  Choice: ");
        if (ch == 0) return;

        if (ch == 1) {
            string sid = readWord("  Your Student ID: ");
            borrowItem(sid, readWord("  Item ID: "));
        } else if (ch == 2) {
            string sid = readWord("  Your Student ID: ");
            string iid = readWord("  Item ID: ");
            returnItem(sid, iid, readInt("  Condition now (1-100): "));
        } else if (ch == 3) {
            string sid = readWord("  Your Student ID: ");
            if (!users.isStudent(sid)) { cout << "  X Student not found.\n"; continue; }
            string cat = readCategory();
            cout << "  1.Exam tomorrow (10)  2.Assignment (8)  3.Normal (2)\n";
            int u = readInt("  Urgency: ");
            int prio = (u == 1) ? 10 : (u == 2) ? 8 : 2;
            txManager.addItemRequest(sid, cat, prio);
            cout << "  OK Request added with priority " << prio << ".\n";
        } else if (ch == 4) {
            allocateItemToTopRequest();
        } else if (ch == 5) {
            string tid = readWord("  Teacher ID: ");
            if (!users.isTeacher(tid)) { cout << "  X Teacher not found.\n"; continue; }
            string day = readDay();
            string time = readTime();
            cout << "  1.Quiz (10)  2.Extra_Class (8)  3.Doubt_Session (4)\n";
            int p = readInt("  Purpose: ");
            string purpose = (p == 1) ? "Quiz" : (p == 2) ? "Extra_Class" : "Doubt_Session";
            int prio = (p == 1) ? 10 : (p == 2) ? 8 : 4;
            txManager.addRoomRequest(tid, day, time, purpose, prio);
            cout << "  OK Request added with priority " << prio << ".\n";
        } else if (ch == 6) {
            allocateRoomToTopRequest();
        } else if (ch == 7) {
            txManager.showHistory();
        } else {
            cout << "  Invalid choice.\n";
        }
    }
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    // Program start: saari files se data load
    users.loadFromFile();
    items.loadFromFile();
    timetable.loadFromFile();
    txManager.loadFromFile();

    while (true) {
        printTitle("CAMPUS RESOURCE & SPACE SYSTEM");
        cout << "  1. Dashboard\n"
             << "  2. Users                    (Module 1)\n"
             << "  3. Shared items             (Module 2)\n"
             << "  4. Classrooms               (Module 3)\n"
             << "  5. Requests and transaction (Module 4)\n"
             << "  0. Exit\n";
        int ch = readInt("  Choice: ");

        if (ch == 0) break;
        else if (ch == 1) showDashboard();
        else if (ch == 2) userMenu();
        else if (ch == 3) itemMenu();
        else if (ch == 4) roomMenu();
        else if (ch == 5) transactionMenu();
        else cout << "  Invalid choice.\n";
    }

    cout << "  All data is saved in .txt files. Bye!\n";
    return 0;
}
