cmake_minimum_required(VERSION 3.10)
project(StudentManagementSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(StudentManagementSystem
    main.cpp
    MainWindow.h
    MainWindow.cpp
    Models.h
    FileManager.h
    FileManager.cpp
)

target_link_libraries(StudentManagementSystem PRIVATE Qt6::Widgets)