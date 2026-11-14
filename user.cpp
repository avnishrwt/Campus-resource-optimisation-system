// MODULE 1: USER MODULE (Database: users.txt)
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "DataStructures.cpp"
using namespace std;

class User {
protected:
    string userId, name, role, dept;
public:
    User(string id, string n, string r, string d) : userId(id), name(n), role(r), dept(d) {}
    virtual ~User() {}

    string getId() const { return userId; }
    string getName() const { return name; }
    string getRole() const { return role; }
    string getDept() const { return dept; }

    virtual void showProfile() const = 0;
    virtual string toFileLine() const = 0;   // file me save karne ke liye
};

class Student : public User {
    int semester;
public:
    Student(string id, string n, string d, int sem) : User(id, n, "Student", d), semester(sem) {}

    void showProfile() const override {
        cout << "  [Student] " << userId << " | " << name << " | " << dept << " | Sem " << semester << "\n";
        cout << "     Can: lend items, borrow items, find free rooms to study\n";
    }

    string toFileLine() const override {
        return "Student " + userId + " " + name + " " + dept + " " + to_string(semester);
    }
};

class Teacher : public User {
    string subject, freeSlot;
public:
    Teacher(string id, string n, string d, string sub, string slot)
        : User(id, n, "Teacher", d), subject(sub), freeSlot(slot) {}

    string getSubject() const { return subject; }

    void showProfile() const override {
        cout << "  [Teacher] " << userId << " | " << name << " | " << dept
             << " | Subject: " << subject << " | Doubt slot: " << freeSlot << "\n";
        cout << "     Can: book free classrooms on priority, mentor students\n";
    }

    string toFileLine() const override {
        return "Teacher " + userId + " " + name + " " + dept + " " + subject + " " + freeSlot;
    }
};

class UserManager {
    DynamicArray<User*> users;
public:
    ~UserManager() {
        for (User* u : users) delete u;
    }

    void saveToFile() const {
        ofstream out("users.txt");
        for (User* u : users) out << u->toFileLine() << "\n";
        out.close();
    }

    void loadFromFile() {
        ifstream in("users.txt");          // file nahi hai to loop chalega hi nahi
        string type, id, name, dept, sub, slot;
        int sem;
        while (in >> type) {
            if (type == "Student") {
                in >> id >> name >> dept >> sem;
                users.push_back(new Student(id, name, dept, sem));
            } else if (type == "Teacher") {
                in >> id >> name >> dept >> sub >> slot;
                users.push_back(new Teacher(id, name, dept, sub, slot));
            }
        }
        in.close();

        if (users.empty()) {   // file nahi thi ya khaali thi: sample users
            users.push_back(new Student("S101", "Aman", "CSE", 3));
            users.push_back(new Student("S102", "Rohan", "CSE", 3));
            users.push_back(new Student("S103", "Priya", "ECE", 3));
            users.push_back(new Teacher("T201", "Dr.Sharma", "CSE", "DSA", "02:10"));
            users.push_back(new Teacher("T202", "Prof.Verma", "ECE", "Digital_Logic", "12:00"));
            saveToFile();
        }
    }

    bool addStudent(string id, string name, string dept, int sem) {
        if (findUser(id)) return false;          // ID pehle se hai
        users.push_back(new Student(id, name, dept, sem));
        saveToFile();
        return true;
    }

    bool addTeacher(string id, string name, string dept, string sub, string slot) {
        if (findUser(id)) return false;
        users.push_back(new Teacher(id, name, dept, sub, slot));
        saveToFile();
        return true;
    }

    User* findUser(string id) const {
        for (User* u : users)
            if (u->getId() == id) return u;
        return nullptr;
    }

    bool isStudent(string id) const {
        User* u = findUser(id);
        return u && u->getRole() == "Student";
    }

    bool isTeacher(string id) const {
        User* u = findUser(id);
        return u && u->getRole() == "Teacher";
    }

    void showAllUsers() const {
        if (users.empty()) { cout << "  No users yet.\n"; return; }
        for (User* u : users) u->showProfile();
    }

    int count() const { return users.size(); }
};