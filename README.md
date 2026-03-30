# Study Planner and Assignment Tracker

Beginner-friendly Qt Widgets app for managing assignments, due dates, and completion status. Built with CMake for use in Visual Studio Code.

**Build Requirements**
- CMake 3.16+
- Qt 6 Widgets
- A C++17 compiler (MSVC recommended)

**VS Code Setup**
1. Install the Microsoft C/C++ extension and CMake Tools extension.
2. Install Qt 6 and make sure `Qt6_DIR` is set or discoverable by CMake.
3. Open this folder in VS Code.
4. Use the CMake Tools status bar:
   - Configure
   - Build
   - Run

**Configure with CMake (optional command line)**
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvcXXXX_64"
cmake --build build
```

**Run**
```bash
./build/StudyPlannerAssignmentTracker
```

**Data Storage**
Assignments are saved automatically to a JSON file in your user app data directory:
`%APPDATA%/StudyPlannerAssignmentTracker/assignments.json`

**Sample Data**
See `sample_assignments.json` for a starter file you can copy into the app data path if desired.
