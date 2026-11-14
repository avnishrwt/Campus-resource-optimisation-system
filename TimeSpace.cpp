// MODULE 3: TIME & SPACE MODULE (Database: timetable.txt)
// Data B.Tech CSE Sem III timetable se aata hai (btech_timetable.txt)
// Room ke timetable me jo slot likha hai -> class hai, jo nahi likha -> FREE
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "DataStructures.cpp"
using namespace std;

const int DAY_COUNT = 5;
const string DAYS[DAY_COUNT] = {"MON", "TUE", "WED", "THU", "FRI"};

const int TIME_COUNT = 10;
const string TIMES[TIME_COUNT]     = {"08:00", "08:55", "10:10", "11:05", "12:00",
                                      "12:55", "02:10", "03:05", "04:00", "04:55"};
const string TIME_ENDS[TIME_COUNT] = {"08:55", "09:50", "11:05", "12:00", "12:55",
                                      "01:50", "03:05", "04:00", "04:55", "05:50"};

// ---------------- Din / slot choose karne ke helpers ----------------
void printDayOptions() {
    for (int i = 0; i < DAY_COUNT; i++) cout << "  " << i + 1 << "." << DAYS[i];
    cout << "\n";
}

void printTimeOptions() {
    for (int i = 0; i < TIME_COUNT; i++) {
        cout << "  " << (i + 1 < 10 ? " " : "") << i + 1 << ". " << TIMES[i] << "-" << TIME_ENDS[i];
        if (i % 2 == 1) cout << "\n";
    }
}

string dayFromChoice(int choice) {          // 1 -> MON, galat -> ""
    if (choice < 1 || choice > DAY_COUNT) return "";
    return DAYS[choice - 1];
}

string timeFromChoice(int choice) {         // 1 -> 08:00, galat -> ""
    if (choice < 1 || choice > TIME_COUNT) return "";
    return TIMES[choice - 1];
}

string timeLabel(string start) {            // 08:00 -> 08:00-08:55
    for (int i = 0; i < TIME_COUNT; i++)
        if (TIMES[i] == start) return TIMES[i] + "-" + TIME_ENDS[i];
    return start;
}

// ---------------- Classes ----------------
struct TimeSlot {
    string day;        // MON ... FRI
    string time;       // slot start: 08:00 ...
    string activity;   // TCS307[A,B] ya Booked_by_T201_Quiz
};

class ClassroomSpace {
    string roomId, roomName;
    int capacity;
    DynamicArray<TimeSlot> schedule;
public:
    ClassroomSpace(string id, string name, int cap) : roomId(id), roomName(name), capacity(cap) {}

    string getId() const { return roomId; }
    string getName() const { return roomName; }
    int getCapacity() const { return capacity; }
    const DynamicArray<TimeSlot>& getSchedule() const { return schedule; }

    void setSlot(string day, string time, string activity) {
        for (TimeSlot& s : schedule) {
            if (s.day == day && s.time == time) { s.activity = activity; return; }
        }
        schedule.push_back({day, time, activity});
    }

    string getActivity(string day, string time) const {   // "" = free
        for (const TimeSlot& s : schedule)
            if (s.day == day && s.time == time) return s.activity;
        return "";
    }

    bool isFree(string day, string time) const {
        return getActivity(day, time) == "";
    }

    void showSchedule() const {
        cout << "\n  " << roomName << " (ID: " << roomId << ", capacity ~" << capacity << ")\n";
        for (int d = 0; d < DAY_COUNT; d++) {
            cout << "  " << DAYS[d] << ":\n";
            for (int t = 0; t < TIME_COUNT; t++) {
                string act = getActivity(DAYS[d], TIMES[t]);
                cout << "     " << TIMES[t] << "-" << TIME_ENDS[t] << "  "
                     << (act == "" ? "FREE" : act) << "\n";
            }
        }
    }
};

class TimetableManager {
    DynamicArray<ClassroomSpace*> rooms;

    // File format:  ROOM <id> <name> <capacity>
    //               SLOT <id> <day> <time> <activity>
    bool readFile(string fileName) {
        ifstream in(fileName);
        if (!in) return false;
        string type, roomId, roomName, day, time, activity;
        int cap;
        while (in >> type) {
            if (type == "ROOM") {
                in >> roomId >> roomName >> cap;
                rooms.push_back(new ClassroomSpace(roomId, roomName, cap));
            } else if (type == "SLOT") {
                in >> roomId >> day >> time >> activity;
                ClassroomSpace* rm = findRoom(roomId);
                if (rm) rm->setSlot(day, time, activity);
            }
        }
        in.close();
        return rooms.size() > 0;           // khaali file = load nahi hui
    }

public:
    ~TimetableManager() {
        for (ClassroomSpace* r : rooms) delete r;
    }

    void loadFromFile() {
        if (readFile("timetable.txt")) return;          // bookings wali file
        if (readFile("btech_timetable.txt")) {          // pehli baar: college timetable
            saveToFile();
            return;
        }
        cout << "  WARNING: btech_timetable.txt nahi mili. Isko .exe wale folder me rakho.\n";
    }

    void saveToFile() const {
        ofstream out("timetable.txt");
        for (ClassroomSpace* rm : rooms) {
            out << "ROOM " << rm->getId() << " " << rm->getName() << " " << rm->getCapacity() << "\n";
            for (const TimeSlot& s : rm->getSchedule())
                out << "SLOT " << rm->getId() << " " << s.day << " " << s.time << " " << s.activity << "\n";
        }
        out.close();
    }

    ClassroomSpace* findRoom(string id) const {
        for (ClassroomSpace* rm : rooms)
            if (rm->getId() == id) return rm;
        return nullptr;
    }

    void showRoomList() const {
        cout << "  Room IDs: ";
        for (int i = 0; i < rooms.size(); i++)
            cout << rooms[i]->getId() << (i + 1 < rooms.size() ? ", " : "\n");
    }

    void showRoomTimetable(string id) const {
        ClassroomSpace* rm = findRoom(id);
        if (!rm) { cout << "  X Room " << id << " not found.\n"; return; }
        rm->showSchedule();
    }

    void showFreeRooms(string day, string time, string forWhom) const {
        cout << "\n  Free rooms on " << day << " at " << timeLabel(time) << ":\n";
        int found = 0;
        for (ClassroomSpace* rm : rooms) {
            if (rm->isFree(day, time)) {
                cout << "  -> " << rm->getName() << " (ID: " << rm->getId() << ")\n";
                found++;
            }
        }
        if (found == 0) cout << "  No room is free at this time.\n";
        else if (forWhom == "Student") cout << "  " << found << " rooms free. Go study there!\n";
        else cout << "  " << found << " rooms free for an extra class.\n";
    }

    // pehla free room book karo, nahi mila to nullptr
    ClassroomSpace* bookFreeRoom(string day, string time, string activity) {
        for (ClassroomSpace* rm : rooms) {
            if (rm->isFree(day, time)) {
                rm->setSlot(day, time, activity);
                saveToFile();
                return rm;
            }
        }
        return nullptr;
    }

    int count() const { return rooms.size(); }
};
