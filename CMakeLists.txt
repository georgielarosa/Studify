cmake_minimum_required(VERSION 3.16)
project(StudyPlannerAssignmentTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(StudyPlannerAssignmentTracker
    main.cpp
    Assignment.h
    Assignment.cpp
    PlannerManager.h
    PlannerManager.cpp
    FileHandler.h
    FileHandler.cpp
    MainWindow.h
    MainWindow.cpp
)

target_link_libraries(StudyPlannerAssignmentTracker PRIVATE Qt6::Widgets)
