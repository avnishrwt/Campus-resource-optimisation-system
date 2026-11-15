# Campus Resource Optimization & Second-Life System

A menu-driven C++ console app we built for our 3rd semester PBL (B.Tech CSE, Graphic Era).

The idea came from two everyday campus problems:

1. **Stuff that sits unused.** Someone has a calculator lying in their bag, someone else has an exam tomorrow and doesn't. We wanted a way for students to lend things to each other and keep track of who has what.
2. **Rooms that sit empty.** Our timetable has plenty of free slots in different rooms, but nobody knows which room is free when. Students want a quiet place to study, teachers want a room for a quiz or an extra class.

So the system lets students share items, request them on priority, and lets anyone find free classrooms, with teachers able to book them.

---

## Team


 Prerika Singhal      Module 1 : User Module ('user.cpp')|
 Priyanshu Adhikari   Module 2 : Resource Module ('Resource.cpp') |
 Mayank Chaturvedi    Module 3 : Time & Spaace Module ('TimeSpace.cpp') |
 Avnish Rawat         Module 4: Requests & Transactions (`transaction.cpp`) |

---

## What it can do

**Users**
- Add students and teachers, look up anyone by ID.

**Shared items**
- A student can put up an item for sharing (calculator, book, charger, lab coat) with its condition (%) and the max hours it can be borrowed for.
- Search any item by its ID.

**Classrooms**
- Pick a day and time slot and see every free room.
- View the full weekly timetable of a room.

**Requests & borrowing**
- Borrow an item directly if you know its ID, and return it later with its current condition. If it comes back below 50%, it's marked `DAMAGED`.
- Students can raise a request by urgency: Exam tomorrow (10),